#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <stdlib.h>
#include <string>
#include <unordered_set>
#include <vector>
using namespace std;

// pager with replacement algorithms
// last_frame: for use with lifo algorithm
struct pager
{
	int last_frame;
	int lifo(int frames);
	int random_replacement(int frames, int r_num);
	int lru(int frames, vector<int> last_reference);
};

/* globals */
pager pager;					// pager structure
int cycle;						// time counter for the entire program
int M;							// M, the machine size in words.
int P;							// P, the page size in words.
int S;							// S, the size of each process, i.e., the references are to virtual addresses 0..S-1.
int J;							// J, the ‘‘job mix’’, which determines A, B, and C, as described below.
int N;							// N, the number of references for each process.
string R;						// R, the replacement algorithm, LIFO (NOT FIFO), RANDOM, or LRU.
int num_of_processes;			// number of processes
int num_of_frames;				// number of frames
int free_frames;				// number of free frames
ifstream random_numbers;		// random numbers
vector<int> references;			// references
vector<int> page_table;			// page table
vector<int> faults;				// contains page faults per process
vector<int> evicts;				// how many evicts a process needed
vector<int> owner;				// which process owns the frame
vector<int> time_owned;			// how long a process was using a page
vector<int> time_table;			// time of frames
vector<int> last;				// how long since the last reference

vector<vector<double>> probabilities;		// vector of tasks and their probabilities

// last in first out replacement algorithm
int pager::lifo(int frames)
{
	int victim;
	
	// get victim from struct
	victim = last_frame;
	last_frame++;
	
	// special case
	if(victim == 0)
	{
		last_frame = 0;
	}
	return victim;
}

// random replacement algorithm
int pager::random_replacement(int frames, int r_num)
{
	// find victim
	int victim;
	victim = r_num % frames;
	return victim;
}

// least recently used replacement algorithm
int pager::lru(int frames, vector<int> last_reference)
{
	int victim = 0;
	int time = 0;
	for(int i = 0; i < frames; i++)
	{
		// keep track of latest reference
		if(last_reference[i] > time)
		{
			time = last_reference[i];
			victim = i;
		}
	}
	return victim;
}

// method that checks if a page fault occurs, evicting pages if necessary
void check_for_page_fault(int process)
{
	bool found = false;
	int frame;
	int page_number = references[process]/P;
	
	// find page if possible
	for(int i = 0; i < num_of_frames; i++)
	{
		if(!found)
		{
			if(owner[i] == process && page_table[i] == page_number)
			{
				found = true;
				// cout << "\nowner[" << 1 << "]: " << owner[1] << endl;
				last[i] = 0;
			}
		}
	}
	
	// if no page found, check if there are free pages first
	if(!found)
	{
		faults[process]++;
		
		// free pages exist
		if(free_frames != 0)
		{
			free_frames--;
			frame = free_frames;
			page_table[frame] = page_number;
			owner[frame] = process;
			// cout << "\nupdating owner[" << frame << "]: " << owner[frame] << endl;
			last[frame] = 0;
		}
		
		// no free pages
		else
		{
			// find victim
			if(R == "lifo")
			{
				frame = pager.lifo(num_of_frames);
			}
			if(R == "random")
			{
				int r_num;
				random_numbers >> r_num;
				frame = pager.random_replacement(num_of_frames,r_num);
			}
			if(R == "lru")
			{
				frame = pager.lru(num_of_frames,last);
				// cout << "frame: " << frame << endl;
			}
			
			// evict
			time_owned[owner[frame]] += time_table[frame];
			evicts[owner[frame]]++;
			
			// replace
			page_table[frame] = page_number;
			owner[frame] = process;
			// cout << "\nupdating owner[" << frame << "]: " << owner[frame] << endl;
			time_table[frame] = 0;
			last[frame] = 0;
		}
	}
}

// method to set the next reference of a given process
void next_reference(int process)
{
	int I_MAX = numeric_limits<int>::max();		// max integer
	
	double A = probabilities[process][0];
	double B = probabilities[process][1];
	double C = probabilities[process][2];
	double D = probabilities[process][3];
	
	// cout << A << ", " << B << ", " << C << ", " << D << endl;
	
	double p;		// probability
	int r_num;		// random number
	random_numbers >> r_num;
	p = r_num / (I_MAX + 1.0);
	// cout << "p: " << p << endl;
	// cout << "S: " << S << endl;
	
	int reference;
	
	if(p < A)
	{
		reference = (references[process] + 1) % S;
		// cout << references[process] + 1 << endl;
		// cout << "p<A;reference: " << reference << endl;
	}
	else if(p < A + B)
	{
		reference = (references[process] - 5 + S) % S;
		// cout << "p<A+B;reference: " << reference << endl;
	}
	else if(p < A + B + C)
	{
		reference = (references[process] + 4) % S;
		// cout << "p<A+B+C;reference: " << reference << endl;
	}
	else
	{
		random_numbers >> r_num;
		reference = r_num % S;
		// cout << "else;reference: " << reference << endl;
	}
	
	// update reference depending on probability generated randomly
	references[process] = reference;
}

int main(int argc, char * argv[])
{
	// M, the machine size in words.
	// P, the page size in words.
	// S, the size of each process, i.e., the references are to virtual addresses 0..S-1.
	// J, the ‘‘job mix’’, which determines A, B, and C, as described below.
	// N, the number of references for each process.
	// R, the replacement algorithm, LIFO (NOT FIFO), RANDOM, or LRU.
	M = atoi(argv[1]);
	P = atoi(argv[2]);
	S = atoi(argv[3]);
	J = atoi(argv[4]);
	N = atoi(argv[5]);
	R = argv[6];
	
	// set up quantum, and initialize global variables
	int quantum = 3;
	cycle = 1;
	pager.last_frame = 0;
	num_of_frames = M/P;
	free_frames = num_of_frames;
	random_numbers.open("random-numbers.txt");
	for(int i = 0; i < 4; i++)
	{
		references.push_back(0);
		faults.push_back(0);
		evicts.push_back(0);
		time_owned.push_back(0);
	}
	for(int i = 0; i < num_of_frames; i++)
	{
		page_table.push_back(-1);
		owner.push_back(-1);
		time_table.push_back(0);
		last.push_back(0);
	}
	
	// cout << "owner.size() = " << owner.size() << endl;
	
	// output the command line arguments
	cout << "The machine size is " << M << "." << endl;
	cout << "The page size is " << P << "." << endl;
	cout << "The process size is " << S << "." << endl;
	cout << "The job mix number is " << J << "." << endl;
	cout << "The number of references per process is " << N << "." << endl;
	cout << "The replacement algorithm is " << R << "." << endl;
	
	// A,B,C,D: as seen in the lab pdf file
	double A = 0;
	double B = 1;
	double C = 2;
	double D = 3;
	
	// One process, A=1, B=C=0
	if(J == 1)
	{
		num_of_processes = 1;
		vector<double> temp;
		probabilities.push_back(temp);
		probabilities[0].push_back(1);
		probabilities[0].push_back(0);
		probabilities[0].push_back(0);
		probabilities[0].push_back(0);
	}
	
	// Four processes, each with A=1 and B=C=0
	if(J == 2)
	{
		num_of_processes = 4;
		for(double i = 0; i < num_of_processes; i++)
		{
			vector<double> temp;
			probabilities.push_back(temp);
			probabilities[i].push_back(1);
			probabilities[i].push_back(0);
			probabilities[i].push_back(0);
			probabilities[i].push_back(0);
		}
	}
	
	// Four processes, each with A=B=C=0
	if(J == 3)
	{
		num_of_processes = 4;
		for(double i = 0; i < num_of_processes; i++)
		{
			vector<double> temp;
			probabilities.push_back(temp);
			probabilities[i].push_back(0);
			probabilities[i].push_back(0);
			probabilities[i].push_back(0);
			probabilities[i].push_back(1);
		}
	}
	
	// One process with A=0.75, B=0.25, C=0
	// One process with A=0.75, B=0, C=0.25
	// One process with A=0.75, B=0.125, C=0.125
	// One process with A=0.5, B=0.125, C=0.125
	if(J == 4)
	{
		num_of_processes = 4;
		for(int i = 0; i < num_of_processes; i++)
		{
			vector<double> temp;
			probabilities.push_back(temp);
		}
		
		probabilities[0].push_back(0.75);
		probabilities[0].push_back(0.25);
		probabilities[0].push_back(0);
		probabilities[0].push_back(0);
		
		probabilities[1].push_back(0.75);
		probabilities[1].push_back(0);
		probabilities[1].push_back(0.25);
		probabilities[1].push_back(0);
		
		probabilities[2].push_back(0.75);
		probabilities[2].push_back(0.125);
		probabilities[2].push_back(0.125);
		probabilities[2].push_back(0);
		
		probabilities[3].push_back(0.5);
		probabilities[3].push_back(0.125);
		probabilities[3].push_back(0.125);
		probabilities[3].push_back(0.25);
	}
	
	// for(auto i : probabilities)
	// {
		// for(auto j : i)
		// {
			// cout << j << " ";
		// }
		// cout << endl;
	// }
	
	// setup default references for all processes
	for(int i = 0; i < num_of_processes; i++)
	{
		references[i] = (111 * (i + 1)) % S;
	}
	
	// round robin
	for(int i = 0; i < N/quantum; i++)
	{
		for(int j = 0; j < num_of_processes; j++)
		{
			for(int k = 0; k < quantum; k++)
			{
				check_for_page_fault(j);
				next_reference(j);
				
				// increment time
				for(int l = num_of_frames - 1; l >= free_frames; l--)
				{
					last[l]++;
					time_table[l]++;
				}
				cycle++;
			}
		}
	}
	
	// edge case for when N/quantaum has a non-zero remainder
	for(int i = 0; i < num_of_processes; i++)
	{
		for(int j = 0; j < N % quantum; j++)
		{
			check_for_page_fault(i);
			next_reference(j);
			
			for(int l = num_of_frames - 1; l >= free_frames; l--)
			{
				last[l]++;
				time_table[l]++;
			}
			cycle++;
		}
	}
	
	// output results
	int num_of_page_faults = 0;
	double average_residency_time = 0;
	int total_num_of_faults = 0;
	double total_residency_time = 0;
	int total_evicts = 0;
	cout << endl;
	for(int i = 0; i < num_of_processes; i++)
	{
		total_num_of_faults = total_num_of_faults + faults[i];
		// cout << "total faults: " << total_num_of_faults << ", " << "faults[i]: " << faults[i] << endl;
		total_residency_time = total_residency_time + time_owned[i];
		total_evicts = total_evicts + evicts[i];
		
		if(evicts[i] == 0)
		{
			cout << "Process " << i + 1 << " had " << faults[i] << " faults.\n";
			cout << "\tWith no evictions, the average residence is undefined.\n";
		}
		if(evicts[i] != 0)
		{
			cout << "Process " << i + 1 << " had " << faults[i] << " faults and " << (double) time_owned[i]/evicts[i] << " average residency.\n";
		}
	}
	cout << endl;
	if(total_evicts == 0)
	{
		cout << "The total number of faults is " << total_num_of_faults << "." << endl;
		cout << "\tWith no evictions, the overall average residence is undefined." << endl;
	}
	if(total_evicts != 0)
	{
		cout << "The total number of faults is " << total_num_of_faults << " and the overall average residency is " << total_residency_time/total_evicts << "." << endl;
	}
	
	// close file
	random_numbers.close();
	return 0;
}