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
	string status;
	vector<activity> activities;
	int initial_claim;
	int time_taken;
	int waiting_time;
	double percent_waiting;
};

// method to print final output
void print_output(vector<task> tasks, string resource_manager)
{
	cout << setw(19) << resource_manager << endl;
	int total_time_taken = 0;
	int total_waiting_time = 0;
	double total_percent_waiting = 0;
	for(auto i : tasks)
	{
		cout << setw(11) << "Task " << i.task_number << setw(7) << i.time_taken << setw(4) << i.waiting_time << setw(4) << i.percent_waiting << "%" << endl;
		total_time_taken = total_time_taken + i.time_taken;
		total_waiting_time = total_waiting_time + i.waiting_time;
		total_percent_waiting = total_percent_waiting + i.waiting_time;
	}
	total_percent_waiting = 100.0 * total_percent_waiting / total_time_taken;
	cout << setw(11) << "total" << setw(8) << total_time_taken << setw(4) << total_waiting_time << setw(4) << total_percent_waiting << "%";
}

// optimistic resource manager
void optimistic(int T, int R, vector<int> units, vector<activity> activities)
{
	int cycle = 0;
	int tasks_completed = 0;
	
	vector<task> tasks;
	queue<activity> requests;
	
	int total_requests = 0;
	for(int i = 0; i < T; i++)
	{
		vector<activity> task_activity;
		for(auto j : activities)
		{
			if(j.one == i + 1 && j.command == "request")
			{
				task_activity.push_back(j);
				total_requests++;
			}
		}
		task t = {i + 1,0,0,"unstarted",task_activity,0,0,0,0};
		tasks.push_back(t);
	}
	
	while(tasks_completed != T)
	{
		// initialize each task
		for(auto i : activities)
		{
			if(i.command == "initiate")
			{
				int t_num = i.one;
				if(tasks[t_num - 1].status != "initialized")
				{
					cout << "initializing " << tasks[t_num - 1].task_number << endl;
					tasks[t_num - 1].status = "initialized";
				}
			}
		}
		cycle++;
		
		// put requests into queue
		int pushed = 0;
		while(pushed != total_requests)
		{
			for(auto i : activities)
			{
				for(int j = 0; j < T; j++)
				{
					cout << i.command << "," << i.one << "," << j + 1 << endl;
					if(i.command == "request" && i.one == j + 1)
					{
						cout << "pushing to request queue: " << i.one << "," << i.two << "," << i.three << endl;
						requests.push(i);
						i.status = "accounted";
						pushed++;
					}
				}
			}
		}
		
		// grant requests
		// queue that holds all requests that cannot be granted this cycle
		queue<activity> ungranted_requests;
		while(!requests.empty())
		{
			int units_avail = units[requests.front().two - 1];
			int res_type = requests.front().two;
			int units_requested = requests.front().three;
			cout << units_avail << "," << units_requested << endl;
			if(units_avail - units_requested >= 0)
			{
				cout << "granting request: " << requests.front().one << ", " << res_type << ", " << units_requested << endl;
				tasks[requests.front().one - 1].resource_type = res_type;
				tasks[requests.front().one - 1].status = "request granted";
				units[requests.front().two - 1] = units[requests.front().two - 1] - units_requested;
				cout << "units left: " << units[requests.front().two - 1] << endl;
				requests.pop();
			}
			else
			{
				cout << "cannot grant request\n";
				ungranted_requests.push(requests.front());
				tasks[requests.front().one - 1].waiting_time++;
				requests.pop();
			}
		}
		requests = ungranted_requests;
		cycle++;
		
		// release resources
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
		
		// handle terminates
		for(auto i : activities)
		{
			if(i.command == "terminate")
			{
				cout << "terminating " << i.one - 1 << endl;
				tasks[i.one - 1].status = "terminated";
				tasks[i.one - 1].time_taken = cycle;
				tasks[i.one - 1].percent_waiting = 100.0 * tasks[i.one - 1].waiting_time / tasks[i.one - 1].time_taken;
				tasks_completed++;
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