#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "def.h"
#include "../src/bpt.h"

using namespace std;

static void usage()
{
	// fprintf();
}

int main(int argc, char **argv)
{
	static char prompt[] = "bpt> ";
	char *line;
	bpt tree = bpt(NODE_ORDER, LEAF_ORDER);
	int opt;
	while ((opt = getopt(argc, argv, "i:o:b:")) != -1)
	{
		switch (opt)
		{
		case 'i':
			tree.init(argv[2]);
			return 0;
			break;
		default:
			break;
		}
	}
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
			tree.ins(cmd[1], cmd[2]);
			free(line);
		}
		else if (cmd.size() == 2 && !strcmp((char *)cmd[0].data(), "init"))
		{
			tree.init((char *)cmd[1].data());
			free(line);
		}
		else if (cmd.size() == 2 && !strcmp((char *)cmd[0].data(), "delete"))
		{
			tree.del((char *)cmd[1].data());
			free(line);
		}
		else if (cmd.size() == 2 && !strcmp((char *)cmd[0].data(), "select"))
		{
			tree.select(cmd[1]);
			free(line);
		}
		else if (cmd.size() == 3 && !strcmp((char *)cmd[0].data(), "update"))
		{
			tree.update(cmd[1], cmd[2]);
			free(line);
		}
		else
		{
			cout << "Invalid" << endl;
		}
	}
}