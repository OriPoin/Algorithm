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
			ins(data1, data2);
		}
	}
	infile.close();
	return true;
}

bool bpt::ins(string key, string value)
{
	State state = State{false, "NULL"};
	Node *LeafNode = LocateLeaf(root, key);
	state = operate(LeafNode, insertion, key, value);
	backtrace(LeafNode, state.value);
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
	Node *LeafNode = LocateLeaf(root, key);
	state = operate(LeafNode, selection, key, "NULL");
	backtrace(LeafNode, state.value);
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
	Node *LeafNode = LocateLeaf(root, key);
	state = operate(LeafNode, modification, key, value);
	backtrace(LeafNode, state.value);
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

Node *bpt::LocateLeaf(Node *node, string key)
{
	if (node->childNodePtrs.empty())
	{
		return node;
	}
	else
	{
		int pos = SearchKey(node, key);
		pos++;
		list<Node *>::iterator node_it = node->childNodePtrs.begin();
		if (pos != 0)
		{
			advance(node_it, pos);
		}
		return LocateLeaf(*node_it, key);
	}
}

int bpt::SearchKey(Node *node, string key)
{
	if (node->size == 0)
	{
		return -1;
	}
	int left = 0;
	int right = node->size - 1;
	list<string>::iterator key_it;
	while (left <= right)
	{
		int middle = (left + right) / 2;
		key_it = node->key.begin();
		if (middle != 0)
		{
			advance(key_it, middle);
		}
		if (*key_it > key)
		{
			right = middle - 1;
		}
		else
		{
			left = middle + 1;
		}
	}
	return right;
}

void bpt::backtrace(Node *LeafNode, string value)
{
	Node *nodep = LeafNode;
	if (nodep->parentPtr == NULL && nodep->size < order)
	{
		return;
	}
	else
	{
		if (nodep->size >= order)
		{
			nodep = split(nodep);
		}
		else if (nodep->size < order / 2)
		{
			nodep = merge(nodep);
		}
		else
		{
			nodep = nodep->parentPtr;
		}
		backtrace(nodep, value);
	}
}

State bpt::operate(Node *LeafNode, opt option, string key, string value)
{
	int pos = SearchKey(LeafNode, key);
	if (pos < 0 && option == insertion)
	{
		LeafNode->key.push_front(key);
		LeafNode->values.push_front(value);
		LeafNode->size++;
		return State{true, "NULL"};
	}
	list<string>::iterator key_it;
	list<string>::iterator value_it;
	key_it = LeafNode->key.begin();
	if (pos != 0)
	{
		advance(key_it, pos);
	}
	if (option == insertion && *key_it < key)
	{
		if (pos >= LeafNode->size - 1)
		{
			LeafNode->key.push_back(key);
			LeafNode->values.push_back(value);
			LeafNode->size++;
			return State{true, "NULL"};
		}
		else
		{
			key_it++;
			LeafNode->key.insert(key_it, key);
			value_it = LeafNode->values.begin();
			advance(value_it, pos + 1);
			LeafNode->values.insert(value_it, value);
			LeafNode->size++;
			return State{true, "NULL"};
		}
	}
	else if (*key_it == key)
	{
		value_it = LeafNode->values.begin();
		advance(value_it, pos);
		if (option == insertion)
		{
			return State{false, *value_it};
		}
		else if (option == selection)
		{
			return State{true, *value_it};
		}
		else if (option == modification)
		{
			string str = *value_it;
			*value_it = value;
			return State{true, str};
		}
	}
	return State{false, "NULL"};
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
	key_it = fullnode->key.begin();
	advance(key_it, order / 2);
	fullnode->key.splice(newnode->key.begin(), fullnode->key, key_it, fullnode->key.end());
	fullnode->size = order / 2;
	newnode->size = order - order / 2;
	key_it = topnode->key.begin();
	advance(key_it, count);
	if (fullnode->childNodePtrs.empty())
	{
		list<string>::iterator value_it = fullnode->values.begin();
		advance(value_it, order / 2);
		fullnode->values.splice(newnode->values.begin(), fullnode->values, value_it, fullnode->values.end());
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
	else
	{
		node_it = fullnode->childNodePtrs.begin();
		advance(node_it, 1 + order / 2);
		fullnode->childNodePtrs.splice(newnode->childNodePtrs.begin(), fullnode->childNodePtrs, node_it, fullnode->childNodePtrs.end());
		for (list<Node *>::iterator it = newnode->childNodePtrs.begin(); it != newnode->childNodePtrs.end(); ++it)
		{
			Node *nodep = *it;
			nodep->parentPtr = newnode;
		}
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
	return topnode;
}

Node *bpt::merge(Node *scantnode)
{
	return scantnode;
}