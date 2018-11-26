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

struct task
{
	int task_number;
	int resource_type;
	int busy_cycles;
	int resources_held;
	string status;
	vector<activity> activities;
	int initial_claim;
	int time_taken;
	int waiting_time;
	double percent_waiting;
};

bool deadlock(vector<task> tasks, vector<int> units, vector<activity> blocked)
{
	bool deadlock = true;
	for(int i = 0; i < tasks.size(); i++)
	{
		if(tasks[i].status == "terminated" || tasks[i].status == "aborted")
		{
			continue;
		}
		if(tasks[i].status != "blocked")
		{
			return false;
		}
	}
	for(int i = 0; i < blocked.size(); i++)
	{
		int resource_type = blocked[i].two - 1;
		int units_left = units[resource_type];
		int units_requested = blocked[i].three;
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
	for(int i = 0; i < activities.size(); i++)
	{
		// add all tasks to vector
		if(activities[i].command == "initiate")
		{
			vector<activity> a;
			task temp = {activities[i].one,activities[i].two,0,0,"unstarted",a,0,0,0,0.0};
			tasks.push_back(temp);
		}
		
		// add all commands to tasks in vector
		int task_number = activities[i].one - 1;
		tasks[task_number].activities.push_back(activities[i]);
	}
	
	// keep looping until all tasks are completed
	while(tasks_completed != T)
	{
		bool terminated = false;
		bool release = false;
		int to_release;
		int release_type;
		
		// setup this cycle's commands to be executed
		vector<activity> commands_per_cycle;
		for(int i = 0; i < tasks.size(); i++)
		{
			commands_per_cycle.push_back(tasks[i].activities[0]);
			if(tasks[i].activities[0].status == "fulfilled")
			{
				// cout << "removing " << tasks[i].activities[0].command << " " << tasks[i].activities[0].one << " " << tasks[i].activities[0].two << " " << tasks[i].activities[0].three << endl;
				tasks[i].activities.erase(tasks[i].activities.begin());
			}
		}
		cout << "\ncycle: " << cycle << endl;
		
		// check blocked vector to see if we can grant any requests)
		int size = blocked.size();
		for(int i = 0; i < size; i++)
		{
			int task_number = blocked[i].one - 1;
			int resource_type = blocked[i].two - 1;
			int units_left = units[resource_type];
			int request = blocked[i].three;
			if(units_left >= request)
			{
				units[resource_type] = units[resource_type] - request;
				tasks[task_number].resource_type = resource_type + 1;
				tasks[task_number].resources_held = request;
				tasks[task_number].status = "from blocked vector";
				cout << "\tgrant " << blocked[i].one << " " << blocked[i].two << " " << blocked[i].three << "\t\tfrom blocked" << endl;
				cout << "\tunits left " << units[resource_type] << endl;
				blocked.erase(blocked.begin() + i);
				i--;
				size--;
			}
			else
			{
				tasks[task_number].status = "from blocked vector";
				cout << "\tcannot grant " << blocked[i].one << " " << blocked[i].two << " " << blocked[i].three << "\tfrom blocked" << endl;
			}
		}
		
		// remove tasks that had activities in the blocked vector
		int s = commands_per_cycle.size();
		for(int i = 0; i < s; i++)
		{
			if(tasks[commands_per_cycle[i].one - 1].status == "from blocked vector")
			{
				tasks[commands_per_cycle[i].one - 1].status = "waiting for next cycle";
				tasks[commands_per_cycle[i].one - 1].activities[0].status = "unfulfilled";
				commands_per_cycle.erase(commands_per_cycle.begin() + i);
				i--;
				s--;
				// cout << commands_per_cycle[i].one << " is waiting for next cycle\n";
			}
		}
		
		for(int i = 0; i < commands_per_cycle.size(); i++)
		{
			cout << "   " << commands_per_cycle[i].command << " " << commands_per_cycle[i].one << " " << commands_per_cycle[i].two << " " << commands_per_cycle[i].three << endl;
		}
		
		// iterate through commands_per_cycle and deal with each activity
		for(int i = 0; i < commands_per_cycle.size(); i++)
		{
			// initiate activity
			if(commands_per_cycle[i].command == "initiate")
			{
				int task_number = commands_per_cycle[i].one - 1;
				tasks[task_number].status = "initiated";
				tasks[task_number].activities[0].status = "fulfilled";
				cout << "\tinitiate " << task_number + 1 << endl;
			}
			
			// request activity
			if(commands_per_cycle[i].command == "request")
			{
				int task_number = commands_per_cycle[i].one - 1;
				int resource_type = commands_per_cycle[i].two - 1;
				int units_left = units[resource_type];
				int request = commands_per_cycle[i].three;
				
				// grant the request if we can
				if(units_left >= request)
				{
					units[resource_type] = units[resource_type] - request;
					tasks[task_number].resource_type = resource_type + 1;
					tasks[task_number].resources_held = request;
					tasks[task_number].status = "request granted";
					tasks[task_number].activities[0].status = "fulfilled";
					cout << "\tgrant " << commands_per_cycle[i].one << " " << commands_per_cycle[i].two << " " << commands_per_cycle[i].three << endl;
					cout << "\tunits left " << units[resource_type] << endl;
				}
				
				// otherwise move it to the blocked queue
				else
				{
					blocked.push_back(commands_per_cycle[i]);
					tasks[task_number].busy_cycles++;
					tasks[task_number].waiting_time++;
					tasks[task_number].status = "blocked";
					tasks[task_number].activities[0].status = "fulfilled";
					cout << "\tcannot grant " << commands_per_cycle[i].one << " " << commands_per_cycle[i].two << " " << commands_per_cycle[i].three << endl;
				}
			}
						
			// release activity
			if(commands_per_cycle[i].command == "release" && tasks[commands_per_cycle[i].one - 1].status != "aborted" && tasks[commands_per_cycle[i].one - 1].status != "blocked")
			{
				int task_number = commands_per_cycle[i].one - 1;
				int resource_type = commands_per_cycle[i].two - 1;
				int units_released = commands_per_cycle[i].three;
				to_release = units_released;
				release_type = resource_type;
				release = true;
				tasks[task_number].resources_held = 0;
				tasks[task_number].status = "released resources";
				tasks[task_number].activities[0].status = "fulfilled";
				cout << "\trelease " << task_number + 1 << " " << resource_type + 1 << " " << units_released << endl;
			}
			
			// TODO: compute activity
			
			// terminate activity (does not require a cycle)
			if(commands_per_cycle[i].command == "terminate" && tasks[commands_per_cycle[i].one - 1].status != "aborted" && tasks[commands_per_cycle[i].one - 1].status != "blocked")
			{
				int task_number = commands_per_cycle[i].one - 1;
				int resource_type = tasks[task_number].resource_type - 1;
				int release = tasks[task_number].resources_held;
				
				// release the resources
				units[resource_type] = units[resource_type] + release;
				
				tasks[task_number].status = "terminated";
				tasks[task_number].activities[0].status = "fulfilled";
				tasks[task_number].time_taken = cycle;
				tasks[task_number].percent_waiting = 100.0 * tasks[task_number].waiting_time / cycle;
				cout << "\tterminate " << task_number + 1 << endl;
				
				tasks_completed++;
				terminated = true;
			}
		}
		
		// check for deadlock
		if(blocked.size() != 0)
		{
			while(deadlock(tasks,units,blocked))
			{
				// release lowest numbered deadlocked task
				// cout << "\tdeadlock\n";
				for(int i = 0; i < tasks.size(); i++)
				{
					if(tasks[i].status == "blocked")
					{
						units[tasks[i].resource_type - 1] = units[tasks[i].resource_type - 1] + tasks[tasks[i].task_number - 1].resources_held;
						tasks[tasks[i].task_number - 1].resources_held = 0;
						tasks[tasks[i].task_number - 1].status = "aborted";
						tasks[tasks[i].task_number - 1].waiting_time = 0;
						tasks[tasks[i].task_number - 1].percent_waiting = 0;
						tasks[tasks[i].task_number - 1].activities[0].status = "fulfilled";
						tasks_completed++;
						blocked.erase(blocked.begin());
						cout << "\taborting task " << tasks[i].task_number << endl;
						cout << "\tunits left " << units[tasks[i].resource_type - 1] << endl;
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
		
		if(release)
		{
			units[release_type] = units[release_type] + to_release;
			release = false;
			cout << "\tunits left: " << units[release_type] << endl;
		}
		
		if(!terminated)
		{
			cycle++;
		}
	}
	
	print_output(tasks,"FIFO");
}

// banker's algorithm
void banker(int T, int R, vector<int> units, vector<activity> activities)
{
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
			activity a = {command,one,two,three,"fulfilled"};
			activities.push_back(a);
		}
	}
	
	optimistic(T,R,units,activities);
	
	return 0;
}