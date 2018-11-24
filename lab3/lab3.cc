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
	queue<activity> activities;
	int initial_claim;
	int time_taken;
	int waiting_time;
	double percent_waiting;
};

bool deadlock(vector<task> tasks, vector<int> units, queue<activity> requests)
{
	bool deadlock = true;
	for(int i = 0; i < tasks.size(); i++)
	{
		if(tasks[i].status == "request granted")
		{
			int units_avail = units[requests.front().two - 1];
			int res_type = requests.front().two;
			int units_requested = requests.front().three;
			if(units_avail - units_requested >= 0)
			{
				deadlock = false;
			}
		}
	}
	return deadlock;
}

// method to print final output
void print_output(vector<task> tasks, string resource_manager)
{
	cout << "\t\t" << resource_manager << endl;
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
	
	vector<task> tasks;
	queue<activity> requests;
	
	int pushed = 0;
	
	bool last_deadlock = false;
	
	// put all requests into each task object
	int total_requests = 0;
	for(int i = 0; i < T; i++)
	{
		queue<activity> task_activity;
		for(auto j : activities)
		{
			if(j.one == i + 1 && j.command == "request")
			{
				task_activity.push(j);
				total_requests++;
			}
		}
		task t = {i + 1,0,0,0,"unstarted",task_activity,0,0,0,0};
		tasks.push_back(t);
	}
	
	while(tasks_completed != T)
	{
		// initialize each task
		bool initialized = false;
		for(auto i : activities)
		{
			if(i.command == "initiate")
			{
				int t_num = i.one;
				cout << "initial status: " << tasks[t_num - 1].status << endl;
				if(tasks[t_num - 1].status != "initialized" && tasks[t_num - 1].status != "aborted" && tasks[t_num - 1].status != "blocked")
				{
					initialized = true;
					cout << "initializing " << tasks[t_num - 1].task_number << endl;
					tasks[t_num - 1].status = "initialized";
				}
			}
		}
		if(initialized)
		{
			cycle++;
			cout << "cycle: " << cycle << endl;
		}
		
		// put requests into queue
		while(pushed != total_requests)
		{
			// for(auto i : tasks)
			for(int i = 0; i < tasks.size(); i++)
			{
				// cout << "task_number: " << tasks[i].task_number << endl;
				// cout << "size before: " << tasks[i].activities.size() << endl;
				if(!tasks[i].activities.empty())
				{
					cout << "pushing to request queue: " << tasks[i].activities.front().one << "," << tasks[i].activities.front().two << "," << tasks[i].activities.front().three << endl;
					requests.push(tasks[i].activities.front());
					tasks[i].activities.pop();
					// cout << "size after: " << tasks[i].activities.size() << endl << endl;
					pushed++;
				}
			}
		}
		
		// cycle++;
		// cout << "cycle: " << cycle << endl;
		
		// grant requests
		// queue that holds all requests that cannot be granted this cycle
		string previous = "XXXXXX";
		queue<activity> ungranted_requests;
		while(!requests.empty())
		{
			int task_number = requests.front().one;
			if(tasks[task_number - 1].status == "aborted")
			{
				requests.pop();
				previous = "abort";
			}
			int units_avail = units[requests.front().two - 1];
			int res_type = requests.front().two;
			int units_requested = requests.front().three;
			task_number = requests.front().one;
			cout << units_avail << "," << units_requested << endl;
			if(units_avail - units_requested >= 0)
			{
				if(previous == "block" || previous == "abort")
				{
					cycle++;
					cout << "cycle: " << cycle << endl;
				}
				cout << "granting request: " << requests.front().one << ", " << res_type << ", " << units_requested << endl;
				tasks[requests.front().one - 1].resource_type = res_type;
				tasks[requests.front().one - 1].status = "request granted";
				tasks[requests.front().one - 1].resources_held = units_requested;
				units[requests.front().two - 1] = units[requests.front().two - 1] - units_requested;
				cout << "units left: " << units[requests.front().two - 1] << endl;
				requests.pop();
				previous = "grant";
			}
			else
			{
				if(previous == "grant")
				{
					cycle++;
					cout << "cycle: " << cycle << endl;
				}
				cout << "cannot grant request for task " << requests.front().one << endl;
				ungranted_requests.push(requests.front());
				tasks[requests.front().one - 1].status = "blocked";
				if(!last_deadlock)
				{
					tasks[requests.front().one - 1].waiting_time++;
					cout << "incrementing waiting time on task " << requests.front().one << ": " << tasks[requests.front().one - 1].waiting_time++ << endl;
				}
				requests.pop();
				previous = "block";
			}
		}
		requests = ungranted_requests;
		
		if(requests.size() > 0)
		{
			while(deadlock(tasks,units,requests))
			{
				cout << "deadlock\n";
				for(int i = 0; i < tasks.size(); i++)
				{
					if(tasks[i].waiting_time > 0 && !last_deadlock)
					{
						tasks[i].waiting_time--;
						cout << "tasks[" << i << "].waiting_time: " << tasks[i].waiting_time << endl;
					}
				}
				last_deadlock = true;
				for(int i = 0; i < tasks.size(); i++)
				{
					// cout << "status: " << tasks[i].status << endl;
					if(tasks[i].status == "blocked")
					{
						cout << "aborting task " << tasks[i].task_number << endl;
						tasks[i].status = "aborted";
						tasks[i].time_taken = 0;
						tasks[i].waiting_time = 0;
						tasks_completed++;
						units[tasks[i].resource_type - 1] = units[tasks[i].resource_type - 1] + tasks[i].resources_held;
						cout << "units left: " << units[tasks[i].resource_type - 1] << endl;
						break;
					}
				}
				if(!deadlock(tasks,units,requests))
				{
					cycle++;
					cout << "cycle: " << cycle << endl;
				}
				break;
			}
		}
				
		if(requests.size() <= 0)
		{
			// release resources
			cycle++;
			cout << "cycle: " << cycle << endl;
			for(auto i : activities)
			{
				if(i.command == "release" && tasks[i.one - 1].status == "request granted")
				{
					cout << "releasing " << i.one << endl;
					tasks[i.one - 1].status == "released";
					units[i.two - 1] = units[i.two - 1] + i.three;
					cout << "units left: " << units[i.two - 1] << endl;
				}
			}
			cycle++;
			cout << "cycle: " << cycle << endl;
			
			// handle terminates
			for(auto i : activities)
			{
				if(i.command == "terminate")
				{
					if(tasks[i.one - 1].status == "aborted")
					{
						continue;
					}
					cout << "terminating " << i.one - 1 << endl;
					tasks[i.one - 1].status = "terminated";
					tasks[i.one - 1].time_taken = cycle;
					tasks[i.one - 1].percent_waiting = 100.0 * tasks[i.one - 1].waiting_time / tasks[i.one - 1].time_taken;
					tasks_completed++;
				}
			}
		}
	}
	
	print_output(tasks, "FIFO");
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
			activity a = {command,one,two,three,"unstarted"};
			activities.push_back(a);
		}
	}
	
	optimistic(T,R,units,activities);
	
	return 0;
}