#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>
using namespace std;

/*
output:
	for each process:
		A, B, C, and M
		Finishing time
		Turnaround time (finishing time - A)
		I/O time (time in blocked state)
		Waiting time (time in ready state)
	summary data:
		finishing time (when all processes have finished)
		CPU utilization (percentage of time some job is running)
		I/O utilization (percentage of time some job is blocked)
		Throughput, expressed in processes completed per hundred time units
		average turnaround time
		average waiting time
*/

bool verbose;
bool busy = false;

/*
struct for processes

order: for tiebreaking
A: arrival time of the process
B: CPU burst = randomOS(B)
	if(randomOS(B) > CPU time remaining) {CPU burst = remaining time}
C: total CPU time needed
M: I/O burst = preceding CPU burst * M
cycle: cycle number
status: unstarted, ready, running, blocked, terminated
*/
struct process
{
	int order;
	int A;
	int B;
	int C;
	int M;
	int finishTime;
	int turnaroundTime;
	int ioBurst;
	int ioTotalTime;
	int waitingTime;
	int cpuBurst;
	int cpuBurstTimeLeft;
	int quantum;
	string status;
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

// verbose print method
void verboseOutput(int cycle, vector<process> p)
{
	cout << endl;
	cout << "Before cycle" << setw(5) << cycle << ":";
	for(int i = 0; i < p.size(); i++)
	{
		cout << setw(12) << p[i].status;
		if(p[i].status == "unstarted")
		{
			cout << setw(3) << "0" << setw(5);
		}
		if(p[i].status == "ready")
		{
			cout << setw(3) << "0" << setw(5);
		}
		if(p[i].status == "running")
		{
			cout << setw(3) << p[i].cpuBurstTimeLeft + 1 << setw(5);
		}
		if(p[i].status == "blocked")
		{
			cout << setw(3) << p[i].ioBurst + 1 << setw(5);
		}
		if(p[i].status == "terminated")
		{
			cout << setw(3) << "0" << setw(5);
		}
	}
}

// first come first serve
void FCFS(vector<process> pVector)
{
	int totalFinishedProcesses = 0;
	int cycle = 0;
	bool busy = false;
	queue<process> readyQ;
	while(totalFinishedProcesses != pVector.size())
	{
		if(verbose == true)
		{
			verboseOutput(cycle,pVector);
		}
		for(int i = 0; i < pVector.size(); i++)
		{
			// if process is unstarted and has arrived
			if(pVector[i].status == "unstarted" && pVector[i].A <= cycle)
			{
				pVector[i].status = "ready";
				readyQ.push(pVector[i]);
				pVector[i].waitingTime++;
			}
			
			// if process at the top of the queue has arrival time less than cycle, and cpu is not busy
			if(readyQ.front().A < cycle && !busy)
			{
				process temp = readyQ.front();
				int index;
				for(int j = 0; j < pVector.size(); j++)
				{
					if(pVector[i].order == temp.order)
					{
						index = j;
					}
				}
				pVector[index].status = "running";
				readyQ.pop();
				busy = true;
			}
			
			// if process is running and still has cpu burst time
			if(pVector[i].status == "running" && pVector[i].cpuBurstTimeLeft > 0)
			{
				pVector[i].cpuBurstTimeLeft--;
				pVector[i].C--;
				busy = true;
			}
			
			// if process is running but cpu burst runs out
			else if(pVector[i].status == "running" && pVector[i].cpuBurstTimeLeft <= 0)
			{
				pVector[i].status = "blocked";
				busy = false;
			}
			
			// if process is running but completes all cpu time
			else if(pVector[i].status == "running" && pVector[i].C <= 0)
			{
				pVector[i].status = "terminated";
				totalFinishedProcesses++;
				pVector[i].finishTime = cycle;
				pVector[i].turnaroundTime = pVector[i].finishTime - pVector[i].A;
				busy = false;
			}
			
			// if process is blocked and still has io burst time
			if(pVector[i].status == "blocked" && pVector[i].ioBurst > 0)
			{
				pVector[i].ioBurst--;
				pVector[i].ioTotalTime++;
			}
			
			// if process is blocked, but finished io, and cpu is not busy with another process
			else if(pVector[i].status == "blocked" && pVector[i].ioBurst <= 0 && !busy)
			{
				pVector[i].status = "running";
				pVector[i].C--;
				pVector[i].cpuBurstTimeLeft = pVector[i].cpuBurst;
				pVector[i].cpuBurstTimeLeft--;
				busy = true;
			}
			
			// if process is blocked, but finished io, but cpu is busy with another process
			else if(pVector[i].status == "blocked" && pVector[i].ioBurst <= 0 && busy)
			{
				pVector[i].status = "ready";
				readyQ.push(pVector[i]);
				pVector[i].waitingTime++;
			}
		}
		cycle++;
	}
}

// round robin, quantum 2
void roundRobin(vector<process> pVector)
{
}

// last come first serve
void LCFS(vector<process> pVector)
{
}

// HPRN
void HPRN(vector<process> pVector)
{
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
		processesVector.push_back({i,A,B,C,M,0,0,randomOS(B) * M,0,0,randomOS(B),randomOS(B),2,"unstarted"});
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
		verbose = true;
		FCFS(processesVector);
		// RoundRobin();
		// LCFS();
		// HPRN();
	}
	
	// regular option
	else if(argc == 2)
	{
		verbose = false;
		FCFS(processesVector);
		// roundRobin();
		// LCFS();
		// vHPRN();
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