#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

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

int main(int agrc, char ** argv)
{
	return 0;
}