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
	int initial_claim;
	int time_taken;
	int waiting_time;
	double percent_waiting;
};

// optimistic resource manager
void optimistic(int T, int R, vector<int> units, vector<activity> activities)
{
	int cycle = 0;
	int tasks_completed = 0;
	
	vector<task> tasks;
	queue<activity> requests;
	
	for(int i = 0; i < T; i++)
	{
		task t = {i + 1,0,0,"unstarted",0,0,0,0};
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
					tasks[t_num - 1].status = "initialized";
					cout << "initializing " << tasks[t_num - 1].task_number << endl;
				}
			}
		}
		cycle++;
		
		// put requests into queue
		for(auto i : activities)
		{
			if(i.command == "request")
			{
				requests.push(i);
			}
		}
		
		// grant requests
		// queue that holds all requests that cannot be granted this cycle
		queue<activity> ungranted_requests;
		while(!requests.empty())
		{
			int units_avail = units[requests.front().one - 1];
			int res_type = requests.front().two;
			int units_requested = requests.front().three;
			if(units_avail - units_requested >= 0)
			{
				tasks[requests.front().one - 1].resource_type = res_type;
				tasks[requests.front().one - 1].status = "request granted";
				units[requests.front().one - 1] = units[requests.front().one - 1] - units_requested;
				requests.pop();
			}
			else
			{
				ungranted_requests.push(requests.front());
				requests.pop();
			}
		}
		requests = ungranted_requests;
		cycle++;
		
		// release resources
		
	}
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