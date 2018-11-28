#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <stdlib.h>
#include <string>
#include <unordered_set>
#include <vector>
using namespace std;

/* 
 * structure for each activity, as seen in the inputs
 * command: initiate, request, compute, release, or terminate
 * one, two, three: unsigned integers that are needed as parameters for each command
 */
struct activity
{
	string command;
	int one;
	int two;
	int three;
	string status;
};

/* 
 * structure for each task
 * task_number: task-number as seen in the assignment pdf
 * resource_type: holds the resource type that the task is CURRENTLY working with
 * busy_cycles: the cycle in which the task is computing until
 * resources_held: holds a vector of all the resources the task is using
 * status: used to identify what state the task is in (deadlocked, aborted, blocked, terminated, etc.)
 * activities: vector of all activities that need to be performed on the specific task
 * 			   all activities have a task-number in its first unsigned integer after the activity
 * initial_claim: the tasks' initial claim from the initiate activity
 * time_taken: the cycle in which the task terminates
 * waiting_time: the amount of cycles in which the task is blocked
 * percent_waiting: the percentage of all cycles before the task terminates that are spent blocked
 */
struct task
{
	int task_number;
	int resource_type;
	int busy_cycles;
	vector<int> resources_held;
	string status;
	vector<activity> activities;
	int initial_claim;
	int time_taken;
	int waiting_time;
	double percent_waiting;
};

// method to check for deadlock
bool deadlock(vector<task> tasks, vector<int> units, vector<activity> blocked)
{
	bool deadlock = true;
	
	// if task isn't blocked, return false
	for(int i = 0; i < tasks.size(); i++)
	{
		// ignore tasks that have been aborted or terminated
		if(tasks[i].status == "terminated" || tasks[i].status == "aborted")
		{
			continue;
		}
		if(tasks[i].status != "blocked" && tasks[i].status != "denied waiting")
		{
			return false;
		}
	}
	
	// iterate through blocked vector
	for(int i = 0; i < blocked.size(); i++)
	{
		int resource_type = blocked[i].two - 1;
		int units_left = units[resource_type];
		int units_requested = blocked[i].three;
		
		// if the units left exceeds the number of units requested, return false
		if(units_left >= units_requested)
		{
			deadlock = false;
		}
	}
	return deadlock;
}

// method to print final output
void print_output(vector<task> tasks, string resource_manager)
{
	cout << "\t\t\t" << resource_manager << endl;	
	int total_time_taken = 0;
	int total_waiting_time = 0;
	double total_percent_waiting = 0;
	for(auto i : tasks)
	{
		if(i.status == "aborted")
		{
			cout << "\tTask " << i.task_number << "\t\taborted" << endl;
		}
		else
		{
			cout << setw(11) << "\tTask " << i.task_number << "\t\t" << i.time_taken << "\t" << i.waiting_time << "\t" << i.percent_waiting << "%" << endl;
		}
		total_time_taken = total_time_taken + i.time_taken;
		total_waiting_time = total_waiting_time + i.waiting_time;
		total_percent_waiting = total_percent_waiting + i.waiting_time;
	}
	total_percent_waiting = 100.0 * total_percent_waiting / total_time_taken;
	cout << "\t" << "total" << "\t\t" << total_time_taken << "\t" << total_waiting_time << "\t" << total_percent_waiting << "%";
}

// optimistic resource manager
void optimistic(int T, int R, vector<int> units, vector<activity> activities)
{
	int cycle = 0;
	int tasks_completed = 0;
	
	vector<activity> blocked;
	
	// setup vector of tasks
	vector<task> tasks;
	int task_num = 1;
	for(int i = 0; i < activities.size(); i++)
	{
		// add all tasks to vector
		if(activities[i].command == "initiate" && task_num <= T)
		{
			vector<activity> a;
			vector<int> type;
			for(int i = 0; i < R; i++)
			{
				type.push_back(0);
			}
			task temp = {task_num,activities[i].two,0,type,"unstarted",a,0,0,0,0.0};
			task_num++;
			tasks.push_back(temp);
		}
		
		// add all commands to tasks in vector
		int task_number = activities[i].one - 1;
		tasks[task_number].activities.push_back(activities[i]);
	}
	
	// keep looping until all tasks are completed
	while(tasks_completed != T)
	{
		
		bool release = false; // if we need to release some resources at the end of a cycle
		int to_release; // how many resources we might need to release at the end of a cycle
		int release_type; // type of resource we might need to release at the end of a cycle
				
		// check blocked vector to see if we can grant any requests)
		int size = blocked.size();
		for(int i = 0; i < size; i++)
		{
			int task_number = blocked[i].one - 1;
			int resource_type = blocked[i].two - 1;
			int units_left = units[resource_type];
			int request = blocked[i].three;
			
			// if the task is aborted, don't parse any more of its activities
			if(tasks[task_number].status == "aborted")
			{
				continue;
			}
			
			// if we can grant the request, grant it
			if(units_left >= request)
			{
				units[resource_type] = units[resource_type] - request;
				tasks[task_number].resource_type = resource_type + 1;
				tasks[task_number].resources_held[resource_type] = tasks[task_number].resources_held[resource_type] + request;
				tasks[task_number].status = "granted from blocked vector";
				tasks[task_number].activities[0].status = "fulfilled";
				blocked.erase(blocked.begin() + i);
				i--;
				size--;
			}
			
			// otherwise keep it in the blocked vector
			else
			{
				tasks[task_number].status = "grant denied in blocked vector";
				tasks[task_number].waiting_time++;
			}
		}
		
		// iterate through each tasks' commands and deal with each activity
		for(int i = 0; i < tasks.size(); i++)
		{
			// if a task has no more activities
			if(tasks[i].activities.size() == 0)
			{
				continue;
			}
			
			// if the task is computing, don't parse any of the activities
			if(tasks[i].status == "computing")
			{
				if(tasks[i].busy_cycles == cycle)
				{
					tasks[i].status = "ready";
				}
				else
				{
					continue;
				}
			}
			
			// if task already performed an activity in the blocked vector
			if(tasks[i].status == "granted from blocked vector")
			{
				tasks[i].status = "granted waiting";
				continue;
			}
			if(tasks[i].status == "grant denied in blocked vector")
			{
				tasks[i].status = "denied waiting";
				continue;
			}
			
			// initiate activity
			if(tasks[i].activities[0].command == "initiate")
			{
				int task_number = tasks[i].activities[0].one - 1;
				tasks[task_number].status = "initiated";
				tasks[task_number].activities[0].status = "fulfilled";
				tasks[i].activities.erase(tasks[i].activities.begin());
				continue;
			}
			
			// request activity
			if(tasks[i].activities[0].command == "request")
			{
				int task_number = tasks[i].activities[0].one - 1;
				int resource_type = tasks[i].activities[0].two - 1;
				int units_left = units[resource_type];
				int request = tasks[i].activities[0].three;
				
				// grant the request if we can
				if(units_left >= request)
				{
					units[resource_type] = units[resource_type] - request;
					tasks[task_number].resource_type = resource_type + 1;
					tasks[task_number].resources_held[resource_type] = tasks[task_number].resources_held[resource_type] + request;
					tasks[task_number].status = "request granted";
					tasks[task_number].activities[0].status = "fulfilled";
					tasks[i].activities.erase(tasks[i].activities.begin());
					continue;
				}
				
				// otherwise move it to the blocked queue
				else
				{
					blocked.push_back(tasks[i].activities[0]);
					tasks[task_number].waiting_time++;
					tasks[task_number].status = "blocked";
					tasks[task_number].activities[0].status = "fulfilled";
					tasks[i].activities.erase(tasks[i].activities.begin());
					continue;
				}
			}
						
			// release activity
			if(tasks[i].activities[0].command == "release" && tasks[tasks[i].activities[0].one - 1].status != "aborted" && tasks[tasks[i].activities[0].one - 1].status != "blocked")
			{
				int task_number = tasks[i].activities[0].one - 1;
				int resource_type = tasks[i].activities[0].two - 1;
				int units_released = tasks[i].activities[0].three;
				to_release = units_released;
				release_type = resource_type;
				release = true;
				tasks[task_number].resources_held[resource_type] = 0;
				tasks[task_number].status = "released resources";
				tasks[task_number].activities[0].status = "fulfilled";
				tasks[i].activities.erase(tasks[i].activities.begin());
				continue;
			}
			
			// compute activity
			if(tasks[i].activities[0].command == "compute" && tasks[tasks[i].activities[0].one - 1].status != "aborted" && tasks[tasks[i].activities[0].one - 1].status != "blocked")
			{
				int task_number = tasks[i].activities[0].one - 1;
				int busy_for = tasks[i].activities[0].two;
				tasks[task_number].busy_cycles = busy_for + cycle;
				tasks[task_number].status = "computing";
				tasks[i].activities.erase(tasks[i].activities.begin());
				continue;
			}
			
			// terminate activity
			if(tasks[i].activities[0].command == "terminate" && tasks[tasks[i].activities[0].one - 1].status != "aborted" && tasks[tasks[i].activities[0].one - 1].status != "blocked")
			{
				int task_number = tasks[i].activities[0].one - 1;
				int resource_type = tasks[task_number].resource_type - 1;
				int release = tasks[task_number].resources_held[resource_type];
				
				// release the resources
				units[resource_type] = units[resource_type] + release;
				
				tasks[task_number].status = "terminated";
				tasks[task_number].activities[0].status = "fulfilled";
				tasks[task_number].time_taken = cycle;
				tasks[task_number].percent_waiting = 100.0 * tasks[task_number].waiting_time / cycle;
				
				tasks_completed++;
				tasks[i].activities.erase(tasks[i].activities.begin());
				continue;
			}
		}
		
		// check for deadlock
		if(blocked.size() != 0)
		{
			while(deadlock(tasks,units,blocked))
			{
				// release lowest numbered deadlocked task
				for(int i = 0; i < tasks.size(); i++)
				{
					if(tasks[i].status == "blocked" || tasks[i].status == "denied waiting")
					{
						for(int j = 0; j < R; j++)
						{
							units[j] = units[j] + tasks[tasks[i].task_number - 1].resources_held[j];
							tasks[tasks[i].task_number - 1].resources_held[j] = 0;
						}
						tasks[tasks[i].task_number - 1].status = "aborted";
						tasks[tasks[i].task_number - 1].waiting_time = 0;
						tasks[tasks[i].task_number - 1].percent_waiting = 0;
						tasks[tasks[i].task_number - 1].activities[0].status = "fulfilled";
						tasks[tasks[i].task_number - 1].activities.clear();
						tasks_completed++;
						break;
					}
				}
				if(deadlock(tasks,units,blocked))
				{
					continue;
				}
				else
				{
					break;
				}
			}
		}
		
		// release resources if a release activity is called this cycle
		if(release)
		{
			units[release_type] = units[release_type] + to_release;
			release = false;
		}
		
		cycle++;
	}
	
	// output to stdout
	print_output(tasks,"FIFO");
}

// method to check whether a state is safe
bool safe_state(vector<int> units, vector<task> tasks, activity request)
{
	// grant the request, then see if the state is safe
	units[request.two - 1] = units[request.two - 1] - request.three;
	if(units[request.two - 1] < 0)
	{
		return false;
	}
	tasks[request.one - 1].resource_type = request.two;
	tasks[request.one - 1].resources_held[request.two - 1] += request.three;
	tasks[request.one - 1].status = "request granted to check if state is safe";
	
	while(true)
	{
		// if there are no processes remaining, return true
		int completed_processes = 0;
		for(int i = 0; i < tasks.size(); i++)
		{
			if(tasks[i].status == "terminated" || tasks[i].status == "aborted")
			{
				completed_processes++;
			}
		}
		if(completed_processes == tasks.size())
		{
			return true;
		}
		
		int index = -1;
		
		// try to find a process that we can grant all future requests for
		bool found;
		for(int i = 0; i < tasks.size(); i++)
		{
			if(tasks[i].status == "terminated" || tasks[i].status == "aborted")
			{
				continue;
			}

			for(int j = 0; j < units.size(); j++)
			{
				if(units[j] < tasks[i].initial_claim - tasks[i].resources_held[j])
				{
					found = false;
					index = -1;
					break;
				}
				else
				{
					index = i;
					found = true;
				}
			}
			if(!found){continue;}
			else{break;}
		}
		if(index == -1)
		{
			return false;
		}
		else
		{	
			// pretend the task terminated
			for(int j = 0; j < units.size(); j++)
			{
				units[j] = units[j] + tasks[index].resources_held[j];
			}
			tasks[index].status = "terminated";
		}
	}
}

// banker's algorithm
void banker(int T, int R, vector<int> units, vector<activity> activities)
{
	int cycle = 0;
	int tasks_completed = 0;
	
	vector<activity> blocked;
	
	// setup vector of tasks
	vector<task> tasks;
	int task_num = 1;
	for(int i = 0; i < activities.size(); i++)
	{
		// add all tasks to vector
		if(activities[i].command == "initiate" && task_num <= T)
		{
			vector<activity> a;
			vector<int> type;
			for(int i = 0; i < R; i++)
			{
				type.push_back(0);
			}
			task temp = {task_num,activities[i].two,0,type,"unstarted",a,0,0,0,0.0};
			task_num++;
			tasks.push_back(temp);
		}
		
		// add all commands to tasks in vector
		int task_number = activities[i].one - 1;
		tasks[task_number].activities.push_back(activities[i]);
	}
	
	bool safe = false;
	
	while(tasks_completed != T)
	{		
		bool release = false; // if we need to release some resources at the end of a cycle
		int to_release; // how many resources we might need to release at the end of a cycle
		int release_type; // type of resource we might need to release at the end of a cycle
		bool safe = false;
		
		// check blocked vector to see if we can grant any requests)
		int size = blocked.size();
		for(int i = 0; i < size; i++)
		{	
			int task_number = blocked[i].one - 1;
			int resource_type = blocked[i].two - 1;
			int task_claim = tasks[task_number].initial_claim;
			int units_left = units[resource_type];
			int request = blocked[i].three;
			
			// if the task is aborted, don't parse any more of its activities
			if(tasks[task_number].status == "aborted")
			{
				continue;
			}
			
			// if state is safe, grant the request
			if(safe_state(units,tasks,blocked[i]))
			{
				units[resource_type] = units[resource_type] - request;
				tasks[task_number].resource_type = resource_type + 1;
				tasks[task_number].resources_held[resource_type] = tasks[task_number].resources_held[resource_type] + request;
				tasks[task_number].status = "granted from blocked vector";
				tasks[task_number].activities[0].status = "fulfilled";
				blocked.erase(blocked.begin() + i);
				i--;
				size--;
			}
			
			// otherwise keep it in the blocked vector
			else
			{
				tasks[task_number].status = "grant denied in blocked vector";
				tasks[task_number].waiting_time++;
			}
		}
		
		// iterate through each tasks' commands and deal with each activity
		for(int i = 0; i < tasks.size(); i++)
		{
			// if the task is aborted, don't parse any more of its activities
			if(tasks[i].status == "aborted")
			{
				continue;
			}
			
			// if a task has no more activities
			if(tasks[i].activities.size() == 0)
			{
				continue;
			}
			
			// if the task is computing, don't parse any of the activities
			if(tasks[i].status == "computing")
			{
				if(tasks[i].busy_cycles == cycle)
				{
					tasks[i].status = "ready";
				}
				else
				{
					continue;
				}
			}
			
			// if task already performed an activity in the blocked vector
			if(tasks[i].status == "granted from blocked vector")
			{
				tasks[i].status = "granted waiting";
				continue;
			}
			if(tasks[i].status == "grant denied in blocked vector")
			{
				tasks[i].status = "denied waiting";
				continue;
			}
			
			// initiate activity
			if(tasks[i].activities[0].command == "initiate")
			{
				int task_number = tasks[i].activities[0].one - 1;
				int resource_type = tasks[i].activities[0].two - 1;
				int claim = tasks[i].activities[0].three;
				int units_left = units[resource_type];
				
				// if initial claim exceeds units present, abort the process
				if(claim > units_left)
				{
					units[resource_type] = units[resource_type] + tasks[task_number].resources_held[resource_type];
					for(int i = 0; i < R; i++)
					{
						tasks[task_number].resources_held[i] = 0;
					}
					tasks[task_number].status = "aborted";
					tasks[task_number].waiting_time = 0;
					tasks[task_number].percent_waiting = 0;
					tasks[task_number].activities[0].status = "fulfilled";
					tasks[task_number].activities.clear();
					tasks_completed++;
					continue;
				}
				
				else
				{
					tasks[task_number].status = "initiated";
					tasks[task_number].activities[0].status = "fulfilled";
					tasks[task_number].initial_claim = claim;
					tasks[i].activities.erase(tasks[i].activities.begin());
					continue;
				}
			}
			
			// request activity
			if(tasks[i].activities[0].command == "request")
			{
				int task_number = tasks[i].activities[0].one - 1;
				int resource_type = tasks[i].activities[0].two - 1;
				int task_claim = tasks[i].initial_claim;
				int units_left = units[resource_type];
				int request = tasks[i].activities[0].three;
				
				// if state is safe, grant the request
				if(safe_state(units,tasks,tasks[i].activities[0]))
				{					
					// if request goes over initial claim, abort the task
					if(tasks[task_number].resources_held[resource_type] + request > tasks[task_number].initial_claim)
					{
						units[resource_type] = units[resource_type] + tasks[task_number].resources_held[resource_type];
						for(int i = 0; i < tasks[task_number].resources_held.size(); i++)
						{
							tasks[task_number].resources_held[i] = 0;
						}
						tasks[task_number].status = "aborted";
						tasks[task_number].waiting_time = 0;
						tasks[task_number].percent_waiting = 0;
						tasks[task_number].activities[0].status = "fulfilled";
						tasks[task_number].activities.clear();
						tasks_completed++;
						continue;
					}
					units[resource_type] = units[resource_type] - request;
					tasks[task_number].resource_type = resource_type + 1;
					tasks[task_number].resources_held[resource_type] = tasks[task_number].resources_held[resource_type] + request;
					tasks[task_number].status = "request granted";
					tasks[task_number].activities[0].status = "fulfilled";
					tasks[i].activities.erase(tasks[i].activities.begin());
					continue;
				}

				// otherwise move it to the blocked queue
				else
				{
					blocked.push_back(tasks[i].activities[0]);
					tasks[task_number].waiting_time++;
					tasks[task_number].status = "blocked";
					tasks[task_number].activities[0].status = "fulfilled";
					tasks[i].activities.erase(tasks[i].activities.begin());
					continue;
				}
			}
			
			// release activity
			if(tasks[i].activities[0].command == "release" && tasks[tasks[i].activities[0].one - 1].status != "aborted" && tasks[tasks[i].activities[0].one - 1].status != "blocked")
			{
				int task_number = tasks[i].activities[0].one - 1;
				int resource_type = tasks[i].activities[0].two - 1;
				int units_released = tasks[i].activities[0].three;
				to_release = units_released;
				release_type = resource_type;
				release = true;
				for(int i = 0; i < tasks[task_number].resources_held.size(); i++)
				{
					tasks[task_number].resources_held[i] = 0;
				}
				tasks[task_number].status = "released resources";
				tasks[task_number].activities[0].status = "fulfilled";
				tasks[i].activities.erase(tasks[i].activities.begin());
				continue;
			}
			
			// compute activity
			if(tasks[i].activities[0].command == "compute" && tasks[tasks[i].activities[0].one - 1].status != "aborted" && tasks[tasks[i].activities[0].one - 1].status != "blocked")
			{
				int task_number = tasks[i].activities[0].one - 1;
				int busy_for = tasks[i].activities[0].two;
				tasks[task_number].busy_cycles = busy_for + cycle;
				tasks[task_number].status = "computing";
				tasks[i].activities.erase(tasks[i].activities.begin());
				continue;
			}
			
			// terminate activity
			if(tasks[i].activities[0].command == "terminate" && tasks[tasks[i].activities[0].one - 1].status != "aborted" && tasks[tasks[i].activities[0].one - 1].status != "blocked")
			{
				int task_number = tasks[i].activities[0].one - 1;
				int resource_type = tasks[task_number].resource_type - 1;
				vector<int> release = tasks[task_number].resources_held;
				
				// release the resources
				for(int i = 0; i < release.size(); i++)
				{
					units[i] = units[i] + release[i];
				}
				
				tasks[task_number].status = "terminated";
				tasks[task_number].activities[0].status = "fulfilled";
				tasks[task_number].time_taken = cycle;
				tasks[task_number].percent_waiting = 100.0 * tasks[task_number].waiting_time / cycle;
				tasks_completed++;
				tasks[i].activities.erase(tasks[i].activities.begin());
				continue;
			}
		}
		
		// release resources if a release activity is called this cycle
		if(release)
		{
			units[release_type] = units[release_type] + to_release;
			release = false;
		}

		cycle++;
	}
	
	// output to stdout
	print_output(tasks,"BANKER'S");
}

int main(int argc, char * argv[])
{
	int T;
	int R;
	vector<int> units;
	vector<activity> activities;
	
	// if user did not include a filename, output an error
	if(argc == 1)
	{
		cout << "error: missing command line argument FILENAME\n";
		return 0;
	}
	
	else if(argc == 2)
	{
		ifstream input;
		input.open(argv[1]);
		
		// save the number of tasks and the number of resource types
		input >> T >> R;
		
		for(int i = 0; i < R; i++)
		{
			int unit;
			input >> unit;
			
			// save the number of units for each resource type
			units.push_back(unit);
		}
		
		// push back activities into the vector of activities
		string command;
		int one, two, three;
		while(input >> command >> one >> two >> three)
		{
			activity a = {command,one,two,three,"unfulfilled"};
			activities.push_back(a);
		}
	}
	
	// run optimistic manager
	optimistic(T,R,units,activities);
	cout << endl << endl;
	
	// run bankers algorithm
	banker(T,R,units,activities);
	
	return 0;
}