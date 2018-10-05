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
B: CPU burst = randomOS(B)
	if(randomOS(B) > CPU time remaining) {CPU burst = remaining time}
C: total CPU time needed
M: I/O burst = preceding CPU burst * M
status: unstarted, ready, running, blocked, terminated
*/
struct process
{
	int A;
	int B;
	int C;
	int M;
	string status;
	int advanceTime;
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

// verbose first come first serve algorithm
int cycle;
int ioTime;
int waitingTime;
void vFCFS(vector<process> pVector)
{
	cycle = 0;
	ioTime = 0;
	waitingTime = 0;
	cout << endl << "This detalied printout gives the state and remaining burst for each process" << endl << endl;
	cout << "Before cycle \t " << cycle << ": \t" << pVector[0].status << " 0.\n";
	cycle++;
	int cpuTime = pVector[0].C;
	int cpuBurst = randomOS(pVector[0].B);
	int ioBurst = cpuBurst * pVector[0].M;
	while(cpuTime != 0)
	{
		int currentCPUBurst = cpuBurst;
		int currentIOBurst = ioBurst;

		// change status to running, run cpu burst, decrement cpuBurst
		while(currentCPUBurst != 0)
		{
			pVector[0].status = "running";
			cout << "Before cycle \t " << cycle << ": \t" << pVector[0].status << " " << currentCPUBurst << "." << endl;
			currentCPUBurst--;
		}
		cycle++;
		cpuTime--;
		
		// if cpuTime = 0, stop
		if(cpuTime == 0)
		{
			cycle--;
			break;
		}
		
		// if cpuBurst == 0, pVector[0].status = blocked, begin i/o burst	
		while(currentIOBurst != 0)
		{
			pVector[0].status = "blocked";
			cout << "Before cycle \t " << cycle << ": \t" << pVector[0].status << " " << currentIOBurst << "." << endl;
			currentIOBurst--;
			ioTime++;
		}
		cycle++;
	}
}

// regular FCFS
void FCFS(vector<process> pVector)
{
	vector<int> turnaroundVector;
	vector<int> waitingtimeVector;
	cout << "The scheduling algorithm used was First Come First Served\n\n";
	for(int i = 0; i < pVector.size(); i++)
	{
		cout << "Process " << i << ":\n\t";
		cout << "(A,B,C,M) = (" << pVector[i].A << "," << pVector[i].B << "," << pVector[i].C << "," << pVector[i].M << ")\n\t";
		cout << "Finishing time: " << cycle << "\n\t";
		cout << "Turnaround time: " << cycle - pVector[i].A << "\n\t";
		turnaroundVector.push_back(cycle - pVector[i].A);
		cout << "I/O time: " << ioTime << "\n\t";
		cout << "Waiting time: " << waitingTime << "\n\n";
		waitingtimeVector.push_back(waitingTime);
	}
	
	double cpuU = (double)(cycle - ioTime)/(double)cycle;
	double ioU = (double)ioTime/(double)cycle;
	
	cout << "Summary Data:\n\t";
	cout << "Finishing time: " << cycle << "\n\t";
	cout << "CPU Utilization: " << cpuU << "\n\t";
	cout << "I/O Utilization: " << ioU << "\n\t";
	cout << "Throughput: " << 100.0/cycle << "processes per hundred cycles" << "\n\t";
	
	// TODO: fix code for average turnaround time and average waiting time, dont use global variables
	int turnaroundSum = 0;
	for(int i = 0; i < turnaroundVector.size(); i++)
	{
		turnaroundSum += turnaroundVector[i];
	}
	double averageTurnaround = (double)turnaroundSum/turnaroundVector.size();
	cout << "Average turnaround time: " << averageTurnaround << "\n\t";
	int waitingSum = 0;
	for(int i = 0; i < waitingtimeVector.size(); i++)
	{
		waitingSum += waitingtimeVector[i];
	}
	double averageWaitingtime = (double)waitingSum/waitingtimeVector.size();
	cout << "Average waiting time: " << averageWaitingtime;
}

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
		processesVector.push_back({A,B,C,M,"unstarted",-1});
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
		vFCFS(processesVector);
		FCFS(processesVector);
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