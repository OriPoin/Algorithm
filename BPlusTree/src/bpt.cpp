#include <iostream>
#include <algorithm>
#include <fstream>
#include "bpt.h"

bpt::bpt(int order)
{
	this->order = order;
	root = new Node;
}

bool bpt::init(char *fname)
{
	string data1;
	string data2;
	ifstream infile(fname);
	if (!infile)
	{
		return false;
	}
	else
	{
		while (infile >> data1)
		{
			infile >> data2;
			// cout << data1 << " " << data2 << endl;
			ins(data1, data2);
		}
	}
	infile.close();
	return true;
}

bool bpt::ins(string key, string value)
{
	State state = State{false, "NULL"};
	state = operate(insertion, key, value);
	if (state.flag)
	{
		return true;
	}
	else
	{
		cout << "Conflicting!" << endl;
		cout << "Original data: " << state.value << " New data: " << value << endl;
		return false;
	}
}

bool bpt::select(string key)
{
	State state = State{false, "NULL"};
	state = operate(selection, key, "NULL");
	if (state.flag)
	{
		cout << "Index: " << key << endl;
		cout << "Value: " << state.value << endl;
		return true;
	}
	else
	{
		cout << "No data!" << endl;
		return false;
	}
}

bool bpt::update(string key, string value)
{
	State state = State{false, "NULL"};
	state = operate(modification, key, value);
	if (state.flag)
	{
		cout << "Index: " << key << endl;
		cout << "Value: " << state.value << " => " << value << endl;
		return true;
	}
	else
	{
		cout << "No such key" << endl;
		return false;
	}
}

//TODO: 使用二分搜索
State bpt::operate(opt option, string key, string value)
{
	Node *nodep = root;
	list<string>::iterator key_it;
	list<string>::iterator value_it;
	list<Node *>::iterator node_it;
	int count;
	// if root is full
	if (option == insertion)
	{
		if (nodep->size >= order - 1)
		{
			nodep = split(nodep);
		}
	}

	while (!nodep->childNodePtrs.empty())
	{
		count = 0;
		key_it = nodep->key.begin();
		while (*key_it < key && count < nodep->size - 1)
		{
			key_it++;
			count++;
		}
		if (*key_it <= key)
		{
			count++;
		}
		node_it = nodep->childNodePtrs.begin();
		advance(node_it, count);
		nodep = *node_it;
		//if full?
		if (option == insertion)
		{
			if (nodep->size >= order - 1)
			{
				nodep = split(nodep);
			}
		}
	}
	//first node
	if (nodep->key.empty())
	{
		if (option == insertion)
		{
			nodep->key.push_back(key);
			nodep->values.push_back(value);
			nodep->size++;
			return State{true, "NULL"};
		}
		else
		{
			return State{false, "NULL"};
		}
	}
	count = 0;
	key_it = nodep->key.begin();
	while (*key_it < key && (*key_it != nodep->key.back()))
	{
		key_it++;
		count++;
	}
	if (*key_it == key)
	{
		value_it = nodep->values.begin();
		string str;
		advance(value_it, count);
		str = *value_it;
		if (option == insertion)
		{
			return State{false, str};
		}
		else if (option == selection)
		{
			return State{true, str};
		}
		else if (option == modification)
		{
			*value_it = value;
			return State{true, str};
		}
		else
		{
			return State{true, "NULL"};
		}
	}
	else if (*key_it < key)
	{
		if (option == insertion)
		{
			nodep->key.push_back(key);
			nodep->values.push_back(value);
			nodep->size++;
			return State{true, "NULL"};
		}
		else
		{
			return State{false, "NULL"};
		}
	}
	else
	{
		if (option == insertion)
		{
			nodep->key.insert(key_it, key);
			value_it = nodep->values.begin();
			advance(value_it, count);
			nodep->values.insert(value_it, value);
			nodep->size++;
			return State{true, "NULL"};
		}
		else
		{
			return State{false, "NULL"};
		}
	}
}

void bpt::serialize(string fname)
{
	Node *current_node = this->root;
}

Node *bpt::split(Node *fullnode)
{
	int count = 0;
	Node *topnode;
	list<string>::iterator key_it;
	list<Node *>::iterator node_it;
	// New node, splice keys
	Node *newnode = new Node;
	// topnode's addr and childPtrs
	if (fullnode->parentPtr == NULL)
	{
		topnode = new Node;
		fullnode->parentPtr = topnode;
		newnode->parentPtr = topnode;
		topnode->childNodePtrs.push_back(fullnode);
		topnode->childNodePtrs.push_back(newnode);
		root = topnode;
	}
	else
	{
		topnode = fullnode->parentPtr;
		newnode->parentPtr = topnode;
		node_it = topnode->childNodePtrs.begin();
		while (*node_it != fullnode)
		{
			node_it++;
			count++;
		}
		if (count == topnode->size)
		{
			topnode->childNodePtrs.push_back(newnode);
		}
		else
		{
			node_it++;
			topnode->childNodePtrs.insert(node_it, newnode);
		}
	}
	// leaf node
	if (fullnode->childNodePtrs.empty())
	{
		key_it = fullnode->key.begin();
		advance(key_it, order / 2);
		fullnode->key.splice(newnode->key.begin(), fullnode->key, key_it, fullnode->key.end());
		list<string>::iterator value_it = fullnode->values.begin();
		advance(value_it, order / 2);
		fullnode->values.splice(newnode->values.begin(), fullnode->values, value_it, fullnode->values.end());
		fullnode->size = order / 2;
		newnode->size = order - 1 - order / 2;
		if (fullnode->next != NULL)
		{
			newnode->next = fullnode->next;
			newnode->next->previous = newnode;
			newnode->previous = fullnode;
			fullnode->next = newnode;
		}
		else
		{
			fullnode->next = newnode;
			newnode->previous = fullnode;
		}
		// childPtrs of topnode
		key_it = topnode->key.begin();
		advance(key_it, count);
		if (key_it == topnode->key.end())
		{
			topnode->key.push_back(newnode->key.front());
			topnode->size++;
		}
		else
		{
			// key_it++;
			topnode->key.insert(key_it, newnode->key.front());
			topnode->size++;
		}
	}
	else // internal node
	{
		key_it = fullnode->key.begin();
		advance(key_it, order / 2);
		fullnode->key.splice(newnode->key.begin(), fullnode->key, key_it, fullnode->key.end());
		node_it = fullnode->childNodePtrs.begin();
		advance(node_it, order / 2 + 1);
		fullnode->childNodePtrs.splice(newnode->childNodePtrs.begin(), fullnode->childNodePtrs, node_it, fullnode->childNodePtrs.end());
		for (list<Node *>::iterator it = newnode->childNodePtrs.begin(); it != newnode->childNodePtrs.end(); ++it)
		{
			Node *nodep = *it;
			nodep->parentPtr = newnode;
		}
		fullnode->size = order / 2;
		newnode->size = order - 1 - order / 2;
		if (fullnode->next != NULL)
		{
			newnode->next = fullnode->next;
			newnode->next->previous = newnode;
			newnode->previous = fullnode;
			fullnode->next = newnode;
		}
		else
		{
			fullnode->next = newnode;
			newnode->previous = fullnode;
		}
		key_it = topnode->key.begin();
		advance(key_it, count);
		if (key_it == topnode->key.end())
		{
			topnode->key.push_back(newnode->key.front());
			topnode->size++;
			newnode->key.pop_front();
			newnode->size--;
		}
		else
		{
			// key_it++;
			topnode->key.insert(key_it, newnode->key.front());
			topnode->size++;
			newnode->key.pop_front();
			newnode->size--;
		}
	}
	return topnode;
}