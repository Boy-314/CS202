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
};

// optimistic resource manager
void optimistic(int num_of_tasks, int num_of_rtypes, vector<int> units, vector<activity> activities)
{
}

// banker's algorithm
void banker(int num_of_tasks, int num_of_rtypes, vector<int> units, vector<activity> activities)
{
}

int main(int argc, char * argv[])
{
	int num_of_tasks;
	int num_of_rtypes;
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
		input >> num_of_tasks >> num_of_rtypes;
		
		for(int i = 0; i < num_of_rtypes; i++)
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
			activity a = {command,one,two,three};
			activities.push_back(a);
		}
	}
	return 0;
}