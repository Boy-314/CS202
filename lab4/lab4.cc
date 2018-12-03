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
	
	
	return 0;
}