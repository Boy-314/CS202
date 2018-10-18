#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>
using namespace std;

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
	int previousCPUBurst;
	int cpuBurst;
	int cpuBurstTimeLeft;
	int quantum;
	string status;
};

// function for comparing two processes for sorting
bool process_sorter(process const& left, process const& right)
{
	if(left.A == right.A)
	{
		return left.order < right.order;
	}
	return left.A < right.A;
}

/*
reads a random non-negative integer X from a file named random-numbers and
returns the value 1 + (X mod U)
*/
// vector containing all the random numbers in the text file
vector<int> randomNumbers;
int randomOS(int U)
{
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

	// calculate return value
	// cout << "\nFind burst when choosing ready process to run " << randomNumbers[0];
	int finalRandomNumber = 1 + (randomNumbers[0] % U);
	
	randomNumbers.erase(randomNumbers.begin());
	
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

// verbose print method
void verboseOutputRR(int cycle, vector<process> p)
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
			if(p[i].quantum < p[i].cpuBurstTimeLeft)
			{
				cout << setw(3) << p[i].quantum << setw(5);
			}
			else
			{
				cout << setw(3) << p[i].cpuBurstTimeLeft << setw(5);
			}
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
	bool countedRunning = false;
	int totalTimeRunning = 0;
	bool countedIO = false;
	int totalTimeIO = 0;
	int cycle = 0;
	bool busy = false;
	vector<process> blocked;
	vector<process> ready;
	queue<process> readyQ;
	
	// while not all processes are finished
	while(totalFinishedProcesses <= pVector.size())
	{
		// check if processes are done
		for(int i = 0; i < pVector.size(); i++)
		{
			// if process finishes all cpu time
			if(pVector[i].C == 0 && pVector[i].status != "terminated")
			{
				// set status to terminated
				pVector[i].status = "terminated";
				pVector[i].finishTime = cycle - 1;
				pVector[i].turnaroundTime = pVector[i].finishTime - pVector[i].A;
				
				// increment total processes finished
				totalFinishedProcesses++;
				
				busy = false;
			}
			
			if(pVector[i].status == "ready")
			{
				pVector[i].waitingTime++;
			}
			
			if(pVector[i].status == "running" && countedRunning == false)
			{
				countedRunning = true;
				totalTimeRunning++;
			}
			
			if(pVector[i].status == "blocked" && countedIO == false)
			{
				countedIO = true;
				totalTimeIO++;
			}
		}
		
		if(totalFinishedProcesses >= pVector.size())
		{
			busy = false;
			break;
		}
		
		// if verbose mode is active, print out verbose information
		if(verbose == true)
		{
			verboseOutput(cycle,pVector);
		}
		
		// iterate through blocked vector
		for(int i = 0; i < blocked.size(); i++)
		{
			// find corresponding index in pVector
			int index = 0;
			while(pVector[index].order != blocked[i].order)
			{
				index++;
			}
			
			// decrement ioBurst and increment ioTotalTime
			pVector[index].ioBurst--;
			pVector[index].ioTotalTime++;
			
			// if process is done with io
			if(pVector[index].ioBurst <= 0)
			{
				// add to vector of processes that are ready
				pVector[index].ioBurst = pVector[index].previousCPUBurst * pVector[index].M;
				ready.push_back(pVector[index]);
			}
			
			// sort vector of processes that are ready
			stable_sort(ready.begin(),ready.end(),&process_sorter);
		}
		
		vector<int> toBeRemoved;
		// iterate through ready vector
		for(int i = 0; i < ready.size(); i++)
		{
			// cout << endl << "i: " << i << endl << "ready.size(): " << ready.size() << endl;
			// find corresponding index in pVector
			int index = 0;
			while(pVector[index].order != ready[i].order)
			{
				index++;
			}
			toBeRemoved.push_back(index);
			// find corresponding index in blocked vector
			int indexBlocked = 0;
			while(blocked[indexBlocked].order != ready[i].order)
			{
				indexBlocked++;
			}
			
			// set status to ready
			pVector[index].status = "ready";
			
			// push onto ready queue
			readyQ.push(pVector[index]);
			
			// remove from blocked vector
			blocked.erase(blocked.begin() + indexBlocked);
		}
		// remove from ready vector
		for(int i = 0; i < toBeRemoved.size(); i++)
		{
			ready.erase(ready.begin() + i);
		}
		toBeRemoved.clear();
		
		// iterate through each process
		bool busyTest = false;
		for(int i = 0; i < pVector.size(); i++)
		{
			// if process is running
			if(pVector[i].status == "running")
			{
				busyTest = true;
				// decrement cpu burst time left and C
				pVector[i].cpuBurstTimeLeft--;
				pVector[i].C--;
				if(pVector[i].C <= 0)
				{
					busyTest = false;
				}
				
				// if cpuBurstTime is up
				if(pVector[i].cpuBurstTimeLeft <= 0)
				{
					if(pVector[i].C > 0)
					{
						// set status to blocked
						pVector[i].status = "blocked";
						pVector[i].ioBurst = pVector[i].previousCPUBurst * pVector[i].M;
						// add to blocked vector
						blocked.push_back(pVector[i]);
					}
					// change cpu to not blocked
					busy = false;
				}
			}
			
			// if process is unstarted
			if(pVector[i].status == "unstarted")
			{
				// if process has arrived
				if(pVector[i].A <= cycle)
				{
					// set status to ready
					pVector[i].status = "ready";
					
					// push to ready queue
					readyQ.push(pVector[i]);
				}
			}
		}
		
		if(busyTest == false)
		{
			busy = false;
		}
		
		// if cpu isnt busy and there is something in the ready queue
		if(!busy && !readyQ.empty())
		{
			// find corresponding index in pVector
			int index = 0;
			while(pVector[index].order != readyQ.front().order)
			{
				index++;
			}
			
			// set process at front of ready queue status to running
			pVector[index].status = "running";
			int burst = randomOS(pVector[index].B);	
			pVector[index].cpuBurstTimeLeft = burst;
			pVector[index].previousCPUBurst = burst;
			// pop it off the ready queue
			readyQ.pop();
			
			// set cpu to busy
			busy = true;
		}
		countedRunning = false;
		countedIO = false;
		cycle++;
	}
	
	cout << endl << "The scheduling algorithm used was First Come First Served" << endl;
	
	for(int i = 0; i < pVector.size(); i++)
	{
		cout << endl;
		cout << "Process " << i << ":\n\t";
		cout << "(A,B,C,M) = (" << pVector[i].A << "," << pVector[i].B << "," << pVector[i].cpuBurst << "," << pVector[i].M << ")\n\t";
		cout << "Finishing time: " << pVector[i].finishTime << "\n\t";
		cout << "Turnaround time: " << pVector[i].turnaroundTime << "\n\t";
		cout << "I/O time: " << pVector[i].ioTotalTime << "\n\t";
		cout << "Waiting time: " << pVector[i].waitingTime;
		cout << endl;
	}
	
	// calculate summary data
	double sumTATime = 0;
	double sumWTime = 0;
	for(int i = 0; i < pVector.size(); i++)
	{
		sumTATime += pVector[i].turnaroundTime;
		sumWTime += pVector[i].waitingTime;
	}
	
	cout << endl;
	cout << "Summary Data:\n\t";
	cout << "Finishing time: " << cycle - 1 << "\n\t";
	cout << "CPU Utilization: " << totalTimeRunning/(cycle - 1.0) << "\n\t";
	cout << "I/O Utilization: " << totalTimeIO/(cycle - 1.0) << "\n\t";
	cout << "Throughput: " << pVector.size()/((cycle - 1)/100.0) << " processes per hundred cycles" << "\n\t";
	cout << "Average turnaround time: " << sumTATime/pVector.size() << "\n\t";
	cout << "Average waiting time: " << sumWTime/pVector.size() << endl;
}

// round robin, quantum 2
void roundRobin(vector<process> pVector)
{
	int totalFinishedProcesses = 0;
	bool countedRunning = false;
	int totalTimeRunning = 0;
	bool countedIO = false;
	int totalTimeIO = 0;
	int cycle = 0;
	bool busy = false;
	vector<process> blocked;
	vector<process> ready;
	vector<int> toBeRemoved;
	vector<process> tiebreaking;
	queue<process> readyQ;
	
	// while not all processes are finished
	while(totalFinishedProcesses <= pVector.size())
	{
		// check if processes are done
		for(int i = 0; i < pVector.size(); i++)
		{
			// if process finishes all cpu time
			if(pVector[i].C == 0 && pVector[i].status != "terminated")
			{
				// set status to terminated
				pVector[i].status = "terminated";
				pVector[i].finishTime = cycle - 1;
				pVector[i].turnaroundTime = pVector[i].finishTime - pVector[i].A;
				
				// increment total processes finished
				totalFinishedProcesses++;
				
				busy = false;
			}
			
			if(pVector[i].status == "ready")
			{
				pVector[i].waitingTime++;
			}
			
			if(pVector[i].status == "running" && countedRunning == false)
			{
				countedRunning = true;
				totalTimeRunning++;
			}
			
			if(pVector[i].status == "blocked" && countedIO == false)
			{
				countedIO = true;
				totalTimeIO++;
			}
		}
		
		if(totalFinishedProcesses >= pVector.size())
		{
			busy = false;
			break;
		}
		
		// if verbose mode is active, print out verbose information
		if(verbose == true)
		{
			verboseOutputRR(cycle,pVector);
		}
		
		// iterate through blocked vector
		for(int i = 0; i < blocked.size(); i++)
		{
			// find corresponding index in pVector
			int index = 0;
			while(pVector[index].order != blocked[i].order)
			{
				index++;
			}
			
			// decrement ioBurst and increment ioTotalTime
			pVector[index].ioBurst--;
			pVector[index].ioTotalTime++;
			
			// if process is done with io
			if(pVector[index].ioBurst <= 0)
			{
				// add to vector of processes that are ready
				pVector[index].ioBurst = pVector[index].previousCPUBurst * pVector[index].M;
				ready.push_back(pVector[index]);
				
				// sort vector of processes that are ready
				stable_sort(ready.begin(),ready.end(),&process_sorter);
			}
		}
		
		// iterate through ready vector
		for(int i = 0; i < ready.size(); i++)
		{
			// cout << endl << "i: " << i << endl << "ready.size(): " << ready.size() << endl;
			// find corresponding index in pVector
			int index = 0;
			while(pVector[index].order != ready[i].order)
			{
				index++;
			}
			toBeRemoved.push_back(index);
			// find corresponding index in blocked vector
			int indexBlocked = 0;
			while(blocked[indexBlocked].order != ready[i].order)
			{
				indexBlocked++;
			}
			
			// set status to ready
			pVector[index].status = "ready";
			
			// push onto ready queue
			// readyQ.push(pVector[index]);
			tiebreaking.push_back(pVector[index]);
			
			// remove from blocked vector
			blocked.erase(blocked.begin() + indexBlocked);
		}
		// remove from ready vector
		for(int i = 0; i < toBeRemoved.size(); i++)
		{
			ready.erase(ready.begin() + i);
		}
		toBeRemoved.clear();

		// iterate through each process
		bool busyTest = false;
		for(int i = 0; i < pVector.size(); i++)
		{
			// if process is running
			if(pVector[i].status == "running")
			{
				busyTest = true;
				// decrement cpu burst time left and C
				pVector[i].cpuBurstTimeLeft--;
				pVector[i].C--;
				pVector[i].quantum--;
				
				if(pVector[i].C <= 0)
				{
					busyTest = false;
				}
				
				if(pVector[i].quantum <= 0)
				{
					if(pVector[i].cpuBurstTimeLeft > 0)
					{
						// readyQ.push(pVector[i]);
						tiebreaking.push_back(pVector[i]);
					}
					pVector[i].status = "ready";
					pVector[i].quantum = 2;
					busy = false;
				}
				
				// if cpuBurstTime is up
				if(pVector[i].cpuBurstTimeLeft <= 0)
				{
					if(pVector[i].C > 0)
					{
						// set status to blocked
						pVector[i].status = "blocked";
						pVector[i].ioBurst = pVector[i].previousCPUBurst * pVector[i].M;
						pVector[i].quantum = 2;
						// add to blocked vector
						blocked.push_back(pVector[i]);
					}
					// change cpu to not blocked
					busy = false;
				}
			}
			
			// if process is unstarted
			if(pVector[i].status == "unstarted")
			{
				// if process has arrived
				if(pVector[i].A <= cycle)
				{
					// set status to ready
					pVector[i].status = "ready";
					
					// push to ready queue
					// readyQ.push(pVector[i]);
					tiebreaking.push_back(pVector[i]);
				}
			}
		}
		
		if(busyTest == false)
		{
			busy = false;
		}
		
		stable_sort(tiebreaking.begin(),tiebreaking.end(),&process_sorter);
		for(int i = 0; i < tiebreaking.size(); i++)
		{
			int index = 0;
			while(pVector[index].order != tiebreaking[i].order)
			{
				index++;
			}
			readyQ.push(pVector[index]);
		}
		tiebreaking.clear();
		
		// if cpu isnt busy and there is something in the ready queue
		if(!busy && !readyQ.empty())
		{
			// find corresponding index in pVector
			int index = 0;
			while(pVector[index].order != readyQ.front().order)
			{
				index++;
			}
			
			// set process at front of ready queue status to running
			pVector[index].status = "running";
			if(pVector[index].cpuBurstTimeLeft <= 0)
			{
				int burst = randomOS(pVector[index].B);	
				pVector[index].cpuBurstTimeLeft = burst;
				pVector[index].previousCPUBurst = burst;	
			}
			// pop it off the ready queue
			readyQ.pop();
			
			// set cpu to busy
			busy = true;
		}
		countedRunning = false;
		countedIO = false;
		cycle++;
	}
	
	cout << endl << "The scheduling algorithm used was Round Robin" << endl;
	
	for(int i = 0; i < pVector.size(); i++)
	{
		cout << endl;
		cout << "Process " << i << ":\n\t";
		cout << "(A,B,C,M) = (" << pVector[i].A << "," << pVector[i].B << "," << pVector[i].cpuBurst << "," << pVector[i].M << ")\n\t";
		cout << "Finishing time: " << pVector[i].finishTime << "\n\t";
		cout << "Turnaround time: " << pVector[i].turnaroundTime << "\n\t";
		cout << "I/O time: " << pVector[i].ioTotalTime << "\n\t";
		cout << "Waiting time: " << pVector[i].waitingTime;
		cout << endl;
	}
	
	// calculate summary data
	double sumTATime = 0;
	double sumWTime = 0;
	for(int i = 0; i < pVector.size(); i++)
	{
		sumTATime += pVector[i].turnaroundTime;
		sumWTime += pVector[i].waitingTime;
	}
	
	cout << endl;
	cout << "Summary Data:\n\t";
	cout << "Finishing time: " << cycle - 1 << "\n\t";
	cout << "CPU Utilization: " << totalTimeRunning/(cycle - 1.0) << "\n\t";
	cout << "I/O Utilization: " << totalTimeIO/(cycle - 1.0) << "\n\t";
	cout << "Throughput: " << pVector.size()/((cycle - 1)/100.0) << " processes per hundred cycles" << "\n\t";
	cout << "Average turnaround time: " << sumTATime/pVector.size() << "\n\t";
	cout << "Average waiting time: " << sumWTime/pVector.size() << endl;
}

// last come first serve
void LCFS(vector<process> pVector)
{
}

// HPRN
void HPRN(vector<process> pVector)
{
}

vector<process> reset(string filename)
{
	int NumOfProcesses;
	vector<process> processesVector;
	ifstream inputFile(filename);
	inputFile >> NumOfProcesses;
	for(int i = 0; i < NumOfProcesses; i++)
	{
		int A,B,C,M;
		inputFile >> A >> B >> C >> M;	

		processesVector.push_back({i,A,B,C,M,0,0,0,0,0,0,C,0,2,"unstarted"});
	}
	return processesVector;
}

void resetRandom()
{
	if(!randomNumbers.size() == 0)
	{
		randomNumbers.clear();
	}
	
	// push back items from the text file into the vector
	ifstream inputFile("random-numbers.txt");
	int num;
	while(inputFile >> num)
	{
		randomNumbers.push_back(num);
	}
}

int main(int argc, char ** argv)
{
	// open file
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
	int previousCPUBurst;
	int cpuBurst;
	int cpuBurstTimeLeft;
	int quantum;
	string status;
	*/
	int NumOfProcesses;
	vector<process> processesVector;
	if(string(argv[1]) == "--verbose")
	{
		ifstream inputFile(argv[2]);
		inputFile >> NumOfProcesses;
		for(int i = 0; i < NumOfProcesses; i++)
		{
			int A,B,C,M;
			inputFile >> A >> B >> C >> M;	

			processesVector.push_back({i,A,B,C,M,0,0,0,0,0,0,C,0,2,"unstarted"});
		}
	}
	else
	{
		ifstream inputFile(argv[1]);
		inputFile >> NumOfProcesses;
		for(int i = 0; i < NumOfProcesses; i++)
		{
			int A,B,C,M;
			inputFile >> A >> B >> C >> M;	

			processesVector.push_back({i,A,B,C,M,0,0,0,0,0,0,C,0,2,"unstarted"});
		}
	}
	
	// sort processesVector
	stable_sort(processesVector.begin(),processesVector.end(),&process_sorter);	
	
	// output original input
	cout << "The original input was:\t" << NumOfProcesses;
	for(int i = 0; i < processesVector.size(); i++)
	{
		cout << " (" << processesVector[i].A << " " << processesVector[i].B << " " << processesVector[i].C << " " << processesVector[i].M << ")";
	}
	cout << endl;
	
	// output sorted input
	cout << "The (sorted) input is:\t" << NumOfProcesses;
	for(int i = 0; i < processesVector.size(); i++)
	{
		cout << " (" << processesVector[i].A << " " << processesVector[i].B << " " << processesVector[i].C << " " << processesVector[i].M << ")";
	}
	cout << endl;
	
	// verbose option
	if(argc == 3 && (string(argv[1]) == "--verbose"))
	{
		verbose = true;
		// FCFS(processesVector);
		processesVector = reset(argv[2]);
		resetRandom();
		roundRobin(processesVector);
		processesVector = reset(argv[2]);
		resetRandom();
		// LCFS();
		// HPRN();
	}
	
	// regular option
	else if(argc == 2)
	{
		verbose = false;
		FCFS(processesVector);
		processesVector = reset(argv[1]);
		resetRandom();
		roundRobin(processesVector);
		processesVector = reset(argv[1]);
		resetRandom();
		// LCFS();
		// HPRN();
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