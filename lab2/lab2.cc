#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

/*
struct for processes
A: arrival time of the process
B: CPU burst
C: total CPU time needed
M: I/O burst = CPU burst * M
*/
struct process
{
	int A;
	int B;
	int C;
	int M;
};

// function for comparing two processes for sorting
bool process_sorter(process const& left, process const& right)
{
	return left.A < right.A;
}

/*
reads a random non-negative integer X from a file named random-numbers and
returns the value 1 + (X mod U)
*/
int randomOS(int U)
{
	// vector containing all the random numbers in the text file
	vector<int> randomNumbers;
	
	// push back items from the text file into the vector
	ifstream inputFile("random-numbers.txt");
	if(inputFile)
	{
		int num;
		while(inputFile >> num)
		{
			randomNumbers.push_back(num);
		}
	}
	inputFile.close();
	
	// get a random index between 0 and size of randomNumbers - 1
	int max = randomNumbers.size();
	int randomIndex = rand() % max;

	// calculate return value
	int finalRandomNumber = 1 + (randomNumbers[randomIndex] % U);
	
	return finalRandomNumber;
}

int main(int argc, char ** argv)
{
	// open file
	ifstream inputFile(argv[1]);
	
	// get number of processes
	int NumOfProcesses;
	inputFile >> NumOfProcesses;
	
	vector<process> processesVector;
	for(int i = 0; i < NumOfProcesses; i++)
	{
		int A,B,C,M;
		inputFile >> A >> B >> C >> M;
		processesVector.push_back({A,B,C,M});
	}
	
	// output original input
	cout << "The original input was:\t" << NumOfProcesses;
	for(int i = 0; i < processesVector.size(); i++)
	{
		cout << " (" << processesVector[i].A << " " << processesVector[i].B << " " << processesVector[i].C << " " << processesVector[i].M << ")";
	}
	cout << endl;
	
	// sort processesVector
	stable_sort(processesVector.begin(),processesVector.end(),&process_sorter);
	
	// output sorted input
	cout << "The (sorted) input is:\t" << NumOfProcesses;
	for(int i = 0; i < processesVector.size(); i++)
	{
		cout << " (" << processesVector[i].A << " " << processesVector[i].B << " " << processesVector[i].C << " " << processesVector[i].M << ")";
	}
	cout << endl;
	
	// verbose option
	if(argc == 3 && (string(argv[2]) == "-verbose" || (string(argv[2]) == "--verbose")))
	{
		cout << endl << "This detailed printout gives the state and remaining burst for each process" << endl << endl;
		cout << "Before cycle \t 0: \t";
		int cycle = 0;
		for(int i = 0; i < processesVector.size(); i++)
		{
			if(processesVector[i].C != 0)
			{
				if(processesVector[i].A == cycle)
				{
					
				}
			}
		}
	}
	
	// regular option
	else if(argc == 2)
	{
	}
	
	// invalid command line arguments
	else
	{
		cout << "Invalid arguments" << endl;
		cout << "Received " << argc << " argument(s), expected 2 or 3" << endl;
		cout << "Third argument, if provided, must be '-verbose' or '--verbose'\n";
	}
	return 0;
}