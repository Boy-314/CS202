#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

// TODO: check sample input 3, 6

bool is_number(const string& s)
{
	return !s.empty() && find_if(s.begin(), s.end(), [](char c)
	{
		return !isdigit(c);
	}) == s.end();
}

int main()
{
	// variables to hold input
	int numOfModules;
	int ND;
	int NU;
	int NT;
	
	// get total number of modules
	cin >> numOfModules;
	
	// base address and its counter
	vector<int> bAddress(numOfModules,0);
	int bACounter = 0;
	
	// text entries and its counter
	vector<int> textEntry(1000,999999);
	int tCounter = 0;
	
	// adrees type and its counter
	vector<int> aType(1000,0);
	int aCounter = 0;
	
	// intialize symbols array and its counter
	pair<string,int> empty("XXXXXXX",0);
	vector<pair<string,int>> symbols(1000,empty);
	int sCounter = 0;
	
	// use lists and its counter
	vector<string> useLists(1000,"XXXXXXX");
	int lCounter = 0;
	
	// error handling
	int err3 = 0;
	bool error2 = false;
	bool error5 = false;
	
	// Symbol Table Output
	cout << "Symbol Table:" << endl;
	
	// PASS ONE
	// iterate through each module
	for(int m = 0; m < numOfModules; m++)
	{
		// get total number of definitions in current module
		cin >> ND;

		// update base address
		bAddress[m] += bACounter;
		
		// iterate through definitions in current module
		for(int d = 0; d < ND; d++)
		{
			// store definitions in symbols array
			cin >> symbols[d + sCounter].first >> symbols[d + sCounter].second;
			symbols[d + sCounter].second += bAddress[m];
			
			// check if symbol is already defined
			int a = 1;
			while(symbols[a - 1].first != "XXXXXXX")
			{
				if(symbols[a].first == "XXXXXXX")
				{
					break;
				}
				if(symbols[d + sCounter].first == symbols[a - 1].first)
				{
					symbols[a - 1].second = symbols[d + sCounter].second;
					symbols[d + sCounter].first = "XXXXXXX";
					symbols[d + sCounter].second = 0;
					cout << "Error: This variable is multiply defined; last value used.\n";
					break;
				}
				a++;
			}
		}
		sCounter += ND;
		
		// get number of use lists in current module
		cin >> NU;
		
		// iterate through NU "pairs"
		for(int u = 0; u < NU; u++)
		{
			string temp;
			cin >> temp;
			while(temp != "-1")
			{
				if(is_number(temp))
				{
					int err5 = 0;
					error5 = false;
					while(useLists[err5] != "XXXXXXX")
					{
						if(to_string(atoi(temp.c_str()) + bAddress[m]) == useLists[err5])
						{
							error5 = true;
							cout << "Error: Multiple variables used in instruction; all but last ignored: Memory map index " << useLists[err5] << endl;
							break;
						}
						err5++;
					}
					useLists[lCounter] = to_string(atoi(temp.c_str()) + bAddress[m]);
				}
				else
				{
					useLists[lCounter] = temp;
				}
				lCounter++;
				cin >> temp;
			}
		}
		
		// get number of text entries
		cin >> NT;
		
		for(int t = 0; t < NT; t++)
		{
			cin >> textEntry[t + tCounter];
			
			// get last digit from user input
			aType[t + tCounter] = textEntry[t + tCounter] % 10;
			
			// get first 4 digits from user input
			textEntry[t + tCounter] = (textEntry[t + tCounter] - aType[t + tCounter])/10;
		}
		tCounter += NT;
		
		// update base address counter
		bACounter += NT;
	}
	
	// PASS TWO
	// symbol table output
	
	// error 2: loop through useLists and compare symbols to symbols vector
	int x = 0;
	int y = 0;
	if(symbols[y].first == "XXXXXXX" && symbols[y].second == 0)
	{
		int z = 0;
		while(useLists[z] != "XXXXXXX")
		{
			if(!is_number(useLists[z]))
			{
				symbols[y].first = useLists[z];
				symbols[y].second = 111;
				cout << "Error: " << useLists[z] << " is not defined; 111 used." << endl;
			}
			z++;
		}
	}
	while(useLists[x] != "XXXXXXX")
	{
		while(symbols[y].first != "XXXXXXX")
		{
			if(useLists[x] == symbols[y].first)
			{
				error2 = false;
				break;
			}
			else
			{
				if(!is_number(useLists[x]))
				{
					error2 = true;
				}
			}
			y++;
		}
		if(error2)
		{
			cout << "Error: " << useLists[x] << " is not defined; 111 used." << endl;
		}
		x++;
	}

	
	int temp = 0;
	int	counter = 0;
	while(textEntry[counter] != 999999)
	{
		counter++;
	}
	counter--;
	int totalAddress = counter;
	while(symbols[temp].second != 0)
	{
		if(totalAddress < symbols[temp].second && symbols[temp].second != 111)
		{
			cout << symbols[temp].first << "=" << totalAddress << " Error: Definition exceeds module size; last word in module used." << endl;	
			symbols[temp].second = totalAddress;
		}
		else
		{
			if(symbols[temp].second != 111)
			{
				cout << symbols[temp].first << "=" << symbols[temp].second << endl;		
			}				
		}
		temp++;
	}
		
	// convert useLists to match textEntry
	vector<int> fixedULists(100000,0);
	int fixCounter = 0;
	string previous;
	while(useLists[fixCounter] != "XXXXXXX")
	{
		// if current index contains a symbol
		if(!is_number(useLists[fixCounter]))
		{
			previous = useLists[fixCounter];
			fixCounter++;
			continue;
		}
		
		// if current index is a number
		if(is_number(useLists[fixCounter]))
		{
			// find the symbol in the symbol table
			int symbolFinder = 0;
			while(symbols[symbolFinder].first != previous)
			{
				symbolFinder++;
			}
			fixedULists[atoi(useLists[fixCounter].c_str())] = symbols[symbolFinder].second;
		}
		fixCounter++;
	}
		
	// memory map output
	cout << endl << "Memory Map" << endl;
	counter = 0;
	while(textEntry[counter] != 999999)
	{
		// immediate address
		if(aType[counter] == 1)
		{
			// do nothing
		}
		
		// absolute address
		if(aType[counter] == 2)
		{
			if(textEntry[counter] % 1000 >= 300)
			{
				textEntry[counter] = (textEntry[counter]/1000)*1000 + 299;
				cout << "Error: Absolute address exceeds machine size; largest legal value used below." << endl;
			}
		}
		
		// relative address
		int temp = 1;
		if(aType[counter] == 3)
		{
			while(counter < bAddress[temp - 1] || counter > bAddress[temp])
			{

				temp++;
			}
			if(counter == totalAddress)
			{
				textEntry[counter] += bAddress[temp];
			}
			else
			{
				textEntry[counter] += bAddress[temp - 1];		
			}
		}
		
		// external address, in use list
		if(aType[counter] == 4 && fixedULists[counter] != 0)
		{
			textEntry[counter] = (textEntry[counter]/1000)*1000 + fixedULists[counter];
		}
		
		// external address, not in use list
		if(aType[counter] == 4 && fixedULists[counter] == 0)
		{
			int temp = 1;
			while(bAddress[temp] != 0)
			{
				temp++;
			}
			int incrementBy = bAddress[temp - 2];
			textEntry[counter] += incrementBy;
		}
		
		cout << counter << ":\t" << textEntry[counter];
		cout << endl;
		counter++;
	}
	
	while(symbols[err3].second != 0)
	{
		bool error3 = false;
		int err3Counter = 0;
		while(useLists[err3Counter] != "XXXXXXX")
		{
			if(symbols[err3].first == useLists[err3Counter])
			{
				error3 = true;
			}
			err3Counter++;
		}
		if(error3 == false)
		{
			cout << "Warning: " << symbols[err3].first << " was defined but never used.\n";
		}
		err3++;
	}
	
	return 0;
}