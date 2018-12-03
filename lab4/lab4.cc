#include <algorithm>
#include <cstdlib>
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

int main(int argc, char * argv[])
{
	// M, the machine size in words.
	// P, the page size in words.
	// S, the size of each process, i.e., the references are to virtual addresses 0..S-1.
	// J, the ‘‘job mix’’, which determines A, B, and C, as described below.
	// N, the number of references for each process.
	// R, the replacement algorithm, LIFO (NOT FIFO), RANDOM, or LRU.
	int M = atoi(argv[1]);
	int P = atoi(argv[2]);
	int S = atoi(argv[3]);
	int J = atoi(argv[4]);
	int N = atoi(argv[5]);
	string R = argv[6];
	
	// output the command line arguments
	cout << "The machine size is " << M << "." << endl;
	cout << "The page size is " << P << "." << endl;
	cout << "The process size is " << S << "." << endl;
	cout << "The job mix number is " << J << "." << endl;
	cout << "The number of references per process is " << N << "." << endl;
	cout << "The replacement algorithm is " << R << "." << endl;
	
	// handle job mix argument
	vector<vector<double>> probabilities;		// vector of tasks and their probabilities
	double num_of_processes;
	
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
	
	return 0;
}