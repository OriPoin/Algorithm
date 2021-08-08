#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <unistd.h>

using namespace std;

int main(int argc, char **argv)
{
	printf("gendb\n");
	bool order = 1;
	bool reverse = 0;
	int size = 0;
	int opt;
	string dbname;
	while ((opt = getopt(argc, argv, "o:r:R:")) != -1)
	{
		switch (opt)
		{
		case 'r':
			reverse = 1;
			size = atoi(argv[2]);
			break;
		case 'o':
			size = atoi(argv[2]);
			break;
		case 'R':
			order = 0;
			size = atoi(argv[2]);
		default:
			break;
		}
	}
	if (order == 1)
	{
		dbname = std::to_string(size) + "InOrder.db";
		std::ofstream dbfile;
		dbfile.open(dbname);
		for (int i = 0; i < size; i++)
		{
			stringstream ss;
			if (reverse)
			{
				ss << setw(log10(size) + 1) << setfill('0') << size - 1 - i;
			}
			else
			{
				ss << setw(log10(size) + 1) << setfill('0') << i;
			}
			string str;
			ss >> str;
			dbfile << str << " " << str << "\n";
		}
		dbfile.close();
	}
	else
	{
		dbname = std::to_string(size) + "Random.db";
		std::ofstream dbfile;
		dbfile.open(dbname);
		vector<int> numbres;
		for (int i = 0; i < size; i++)
		{
			numbres.push_back(i);
		}
		random_shuffle(numbres.begin(), numbres.end());
		for (std::vector<int>::iterator it = numbres.begin(); it != numbres.end(); ++it)
		{
			int num = *it;
			stringstream ss;
			ss << setw(log10(size) + 1) << setfill('0') << num;
			string str;
			ss >> str;
			dbfile << str << " ";
			// for (uint32_t i = 0; i < 8192; i++)
			for (uint32_t i = 0; i < 1 ; i++)
			{
				// dbfile << "12345678";
				dbfile << str;
			}
			dbfile << "\n";
		}
		dbfile.close();
	}
}
