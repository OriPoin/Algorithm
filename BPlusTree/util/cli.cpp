#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>

#include "def.h"
#include "../src/bpt.h"

using namespace std;

static void usage()
{
	cout << "init ${DBNAME} #test.db | ${DB_SIZE}InOrder.db | ${DB_SIZE}Random.db" << endl;
	cout << "select ${KEY}" << endl;
	cout << "update ${KEY} ${NEW_VALUE}" << endl;
	cout << "delete ${KEY}" << endl;
	cout << "save ${dbname}" << endl;
	cout << "load ${dbname}" << endl;
	cout << "setmode fs/mem" << endl;
}

int main(int argc, char **argv)
{
	static char prompt[] = "bpt> ";
	char *line;
	bpt tree = bpt(NODE_ORDER, LEAF_ORDER, DBNAME);
	while (1)
	{
		line = readline(prompt); // readline allocates space for returned string
		vector<string> cmd;
		string result;
		stringstream input(line);
		while (input >> result)
		{
			cmd.push_back(result);
		}
		if (cmd.size() == 1 && !strcmp((char *)cmd[0].data(), "exit"))
		{
			cout << "bye" << endl;
			free(line);
			exit(0);
		}
		else if (cmd.size() == 3 && !strcmp((char *)cmd[0].data(), "insert"))
		{
			auto t1 = std::chrono::system_clock::now();
			tree.ins(cmd[1], cmd[2]);
			auto t2 = std::chrono::system_clock::now();
			auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			std::cout << " elapsed time: " << dt << "ns" << std::endl;
			std::cout << " used memory: " << getPSMemValue() << "kB" << std::endl;
			free(line);
		}
		else if (cmd.size() == 2 && !strcmp((char *)cmd[0].data(), "init"))
		{
			auto t1 = std::chrono::system_clock::now();
			tree.init((char *)cmd[1].data());
			auto t2 = std::chrono::system_clock::now();
			auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			std::cout << " elapsed time: " << dt << "ns" << std::endl;
			std::cout << " used memory: " << getPSMemValue() << "kB" << std::endl;
			free(line);
		}
		else if (cmd.size() == 2 && !strcmp((char *)cmd[0].data(), "delete"))
		{
			auto t1 = std::chrono::system_clock::now();
			tree.del((char *)cmd[1].data());
			auto t2 = std::chrono::system_clock::now();
			auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			std::cout << " elapsed time: " << dt << "ns" << std::endl;
			std::cout << " used memory: " << getPSMemValue() << "kB" << std::endl;
			free(line);
		}
		else if (cmd.size() == 2 && !strcmp((char *)cmd[0].data(), "select"))
		{
			auto t1 = std::chrono::system_clock::now();
			tree.select(cmd[1]);
			auto t2 = std::chrono::system_clock::now();
			auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			std::cout << " elapsed time: " << dt << "ns" << std::endl;
			std::cout << " used memory: " << getPSMemValue() << "kB" << std::endl;
			free(line);
		}
		else if (cmd.size() == 3 && !strcmp((char *)cmd[0].data(), "update"))
		{
			auto t1 = std::chrono::system_clock::now();
			tree.update(cmd[1], cmd[2]);
			auto t2 = std::chrono::system_clock::now();
			auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			std::cout << " elapsed time: " << dt << "ns" << std::endl;
			std::cout << " used memory: " << getPSMemValue() << "kB" << std::endl;
			free(line);
		}
		else if (cmd.size() == 2 && !strcmp((char *)cmd[0].data(), "load"))
		{
			auto t1 = std::chrono::system_clock::now();
			tree.load((char *)cmd[1].data());
			tree.serialize(DBNAME);
			tree.cacheData(DBNAME);
			auto t2 = std::chrono::system_clock::now();
			auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			std::cout << " elapsed time: " << dt << "ns" << std::endl;
			std::cout << " used memory: " << getPSMemValue() << "kB" << std::endl;
			free(line);
		}
		else if (cmd.size() == 2 && !strcmp((char *)cmd[0].data(), "setmode"))
		{
			tree.setmode((char *)cmd[1].data());
			free(line);
		}
		else if (cmd.size() == 1 && !strcmp((char *)cmd[0].data(), "save"))
		{
			auto t1 = std::chrono::system_clock::now();
			tree.serialize(DBNAME);
			auto t2 = std::chrono::system_clock::now();
			auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			std::cout << " elapsed time: " << dt << "ns" << std::endl;
			std::cout << " used memory: " << getPSMemValue() << "kB" << std::endl;
			free(line);
		}
		else if (cmd.size() == 2 && !strcmp((char *)cmd[0].data(), "save"))
		{
			auto t1 = std::chrono::system_clock::now();
			tree.serialize((char *)cmd[1].data());
			auto t2 = std::chrono::system_clock::now();
			auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			std::cout << " elapsed time: " << dt << "ns" << std::endl;
			std::cout << " used memory: " << getPSMemValue() << "kB" << std::endl;
			free(line);
		}
		else if (cmd.size() == 2 && !strcmp((char *)cmd[0].data(), "help"))
		{
			usage();
			free(line);
		}
		else
		{
			cout << "Invalid" << endl;
			cout << "type help to check command" << endl;
		}
	}
}

int parseLine(char *line)
{
	// This assumes that a digit will be found and the line ends in " Kb".
	int i = strlen(line);
	const char *p = line;
	while (*p < '0' || *p > '9')
		p++;
	line[i - 3] = '\0';
	i = atoi(p);
	return i;
}

int getPSMemValue()
{ //Note: this value is in KB!
	FILE *file = fopen("/proc/self/status", "r");
	int result = -1;
	char line[128];

	while (fgets(line, 128, file) != NULL)
	{
		if (strncmp(line, "VmSize:", 7) == 0)
		{
			result = parseLine(line);
			break;
		}
	}
	fclose(file);
	return result;
}