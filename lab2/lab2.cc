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
			cout << setw(3) << p[i].cpuBurstTimeLeft << setw(5);
		}
		if(p[i].status == "blocked")
		{
			cout << setw(3) << p[i].ioBurst << setw(5);
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
	vector<process> blocked;
	queue<process> readyQ;
	
	/*
	while not all processes are finished
		iterate through blocked array
			if process in blocked array finished io burst
				push it onto the ready queue, remove it from the blocked vector
			else
				decrement ioBurst and increment ioTotalTime
		iterate through all processes
			if process status is unstarted and process has already arrived
				set status to ready, push on to ready queue, increment waiting time
			if cpu is not busy
				if process at the front of the ready queue has arrived
					set process status to running, pop it off the ready queue, and set cpu to busy
			if process is running
				if process runs out of cpu burst time
					add it to the blocked vector, set status to blocked, change cpu to not busy
				else
					decrement cpu burst time left, and C
			if process finishes total cpu time
				set status to terminated and increment total finished processes
		cycle++
	*/
	while(totalFinishedProcesses != pVector.size())
	{
		// if verbose mode is active, print out verbose information
		if(verbose == true)
		{
			verboseOutput(cycle,pVector);
		}
		
		// check blocked vector to see if any processes can be moved to the ready queue
		for(int j = 0; j < blocked.size(); j++)
		{
			// find corresponding element in pVector
			int index = -1;
			for(int k = 0; k < pVector.size(); k++)
			{
				if(blocked[j].order == pVector[k].order)
				{
					index = k;
				}
			}
						
			// -1 ioBurst for all processes in blocked array
			pVector[index].ioBurst--;
			pVector[index].ioTotalTime++;
			
			// if process in blocked array has finished io burst
			if(pVector[index].ioBurst <= 0)
			{
				readyQ.push(pVector[index]);
				pVector[index].status = "ready";
				blocked.erase(blocked.begin() + index);
			}
		}
		
		// iterate through processes
		for(int i = 0; i < pVector.size(); i++)
		{	
			// if process is unstarted and has arrived
			if(pVector[i].status == "unstarted" && pVector[i].A <= cycle)
			{
				pVector[i].status = "ready";
				readyQ.push(pVector[i]);
				pVector[i].waitingTime++;
			}
			
			// if cpu is not busy
			if(!busy && !readyQ.empty())
			{
				process temp = readyQ.front();
				int index = -1;
				for(int j = 0; j < pVector.size(); j++)
				{
					if(pVector[j].order == temp.order)
					{
						index = j;
					}
				}
				pVector[index].status = "running";
				readyQ.pop();
				busy = true;
			}
			
			cout << "\nstatus: " << pVector[0].status << endl;
			cout << "burst left: " << pVector[0].cpuBurstTimeLeft << endl;
			
			// if process is running
			if(pVector[i].status == "running")
			{
				// if process runs out of cpu burst time
				if(pVector[i].cpuBurstTimeLeft <= 0)
				{
					blocked.push_back(pVector[i]);
					pVector[i].status = "blocked";
					pVector[i].cpuBurstTimeLeft = pVector[i].cpuBurst;
					busy = false;
				}
				
				// if process still has cpu burst time
				else
				{
					cout << "working\n";
					pVector[i].cpuBurstTimeLeft--;
					pVector[i].C--;
				}
			}
			
			// if process finishes total cpu time
			if(pVector[i].C <= 0)
			{
				pVector[i].status = "terminated";
				pVector[i].finishTime = cycle;
				pVector[i].turnaroundTime = cycle - pVector[i].A;
				totalFinishedProcesses++;
			}
			
			for(int i = 0; i < pVector.size(); i++)
			{
				cout << endl << "i: " << i << ", " << pVector[i].status << ", " << "cpu: " << pVector[i].cpuBurstTimeLeft << ", " << "io: " << pVector[i].ioBurst << endl;
			}
		}
		cycle++;
		if(cycle > 25)
		{
			break;
		}
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
		/*
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
		*/
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