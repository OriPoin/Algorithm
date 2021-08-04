#include <iostream>
#include <algorithm>
#include <fstream>
#include "bpt.h"

bpt::bpt(int NodeOrder, int LeafOrder)
{
	this->NodeOrder = NodeOrder;
	this->LeafOrder = LeafOrder;
	root = new Node;
	leafHead = root;
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
	list<string> *newKeys = new list<string>();
	list<string> *oldKeys = new list<string>();
	Task *task = new Task{false, insertOpt, key, value, newKeys, oldKeys};
	Node *LeafNode = LocateLeaf(root, key);
	task->state = operate(LeafNode, task);
	backtrace(LeafNode, task);
	if (task->state)
	{
		return true;
	}
	else
	{
		cout << "Conflicting!" << endl;
		cout << "Original data: " << task->val << " New data: " << value << endl;
		return false;
	}
}
bool bpt::select(string key)
{
	list<string> *newKeys = new list<string>();
	list<string> *oldKeys = new list<string>();
	Task *task = new Task{false, selectOpt, key, "NULL", newKeys, oldKeys};
	Node *LeafNode = LocateLeaf(root, key);
	task->state = operate(LeafNode, task);
	backtrace(LeafNode, task);
	if (task->state)
	{
		cout << "Index: " << key << endl;
		cout << "Value: " << task->val << endl;
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
	list<string> *newKeys = new list<string>();
	list<string> *oldKeys = new list<string>();
	Task *task = new Task{false, modifyOpt, key, value, newKeys, oldKeys};
	Node *LeafNode = LocateLeaf(root, key);
	task->state = operate(LeafNode, task);
	backtrace(LeafNode, task);
	if (task->state)
	{
		cout << "Index: " << key << endl;
		cout << "Value: " << task->val << " => " << value << endl;
		return true;
	}
	else
	{
		cout << "No such key" << endl;
		return false;
	}
}

bool bpt::del(string key)
{
	list<string> *newKeys = new list<string>();
	list<string> *oldKeys = new list<string>();
	Task *task = new Task{false, deletOpt, key, "NULL", newKeys, oldKeys};
	Node *LeafNode = LocateLeaf(root, key);
	task->state = operate(LeafNode, task);
	backtrace(LeafNode, task);
	if (task->state)
	{
		cout << "Deleted record" << endl;
		cout << "Index: " << key << endl;
		cout << "Value: " << task->val << endl;
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
	if (node->nodeSize == 0)
	{
		return -1;
	}
	int left = 0;
	int right = node->nodeSize - 1;
	list<string>::iterator key_it;
	while (left <= right)
	{
		int middle = (left + right) / 2;
		key_it = node->keyList.begin();
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

void bpt::backtrace(Node *LeafNode, Task *task)
{
	Node *nodep = LeafNode;
	if (!task->newKeys->empty())
	{
		list<string>::iterator old_it = task->oldKeys->begin();
		list<string>::iterator new_it = task->newKeys->begin();
		list<string>::iterator key_it = nodep->keyList.begin();
		int count = 0;
		for (old_it; (old_it != task->oldKeys->end() && !task->oldKeys->empty()); old_it++)
		{
			string str;
			int pos = SearchKey(nodep, *old_it);
			if (pos >= 0)
			{
				advance(key_it, pos);
				if (*key_it == *old_it)
				{
					advance(new_it, count);
					int keyCount = 0;
					for (list<string>::iterator it = nodep->keyList.begin(); it != nodep->keyList.end(); ++it)
					{
						keyCount++;
					}
					if (nodep->nodeSize == keyCount)
					{
						*key_it = *new_it;
					}
					else
					{
						nodep->keyList.remove(*old_it);
					}
					task->oldKeys->remove(*old_it);
					task->newKeys->remove(*new_it);
				}
			}
			count++;
		}
	}
	if (nodep->parentPtr == NULL && ((!nodep->childNodePtrs.empty() && nodep->nodeSize < NodeOrder) || (nodep->childNodePtrs.empty() && nodep->nodeSize <= LeafOrder)))
	{
		return;
	}
	if ((!nodep->childNodePtrs.empty() && nodep->nodeSize > NodeOrder - 1) || (nodep->childNodePtrs.empty() && nodep->nodeSize > LeafOrder))
	{
		nodep = split(nodep);
	}
	else if ((!nodep->childNodePtrs.empty() && nodep->nodeSize < NodeOrder / 2 - 1) || (nodep->childNodePtrs.empty() && nodep->nodeSize < (LeafOrder + 1) / 2))
	{
		nodep = merge(nodep);
	}
	else if (!task->newKeys->empty())
	{
		nodep = nodep->parentPtr;
	}
	else
	{
		return;
	}
	backtrace(nodep, task);
}

bool bpt::operate(Node *LeafNode, Task *task)
{
	int pos = SearchKey(LeafNode, task->key);
	if (pos < 0 && task->opt == insertOpt)
	{
		LeafNode->keyList.push_front(task->key);
		LeafNode->valueList.push_front(task->val);
		LeafNode->nodeSize++;
		return true;
	}
	list<string>::iterator key_it;
	list<string>::iterator value_it;
	key_it = LeafNode->keyList.begin();
	advance(key_it, pos);
	if (task->opt == insertOpt && *key_it < task->key)
	{
		if (pos >= LeafNode->nodeSize - 1)
		{
			LeafNode->keyList.push_back(task->key);
			LeafNode->valueList.push_back(task->val);
			LeafNode->nodeSize++;
			return true;
		}
		else
		{
			key_it++;
			LeafNode->keyList.insert(key_it, task->key);
			value_it = LeafNode->valueList.begin();
			advance(value_it, pos + 1);
			LeafNode->valueList.insert(value_it, task->val);
			LeafNode->nodeSize++;
			return true;
		}
	}
	else if (*key_it == task->key)
	{
		value_it = LeafNode->valueList.begin();
		advance(value_it, pos);
		if (task->opt == insertOpt)
		{
			task->val = *value_it;
			return false;
		}
		else if (task->opt == selectOpt)
		{
			task->val = *value_it;
			return true;
		}
		else if (task->opt == modifyOpt)
		{
			string str = *value_it;
			*value_it = task->val;
			task->val = str;
			return true;
		}
		else if (task->opt == deletOpt)
		{
			LeafNode->keyList.remove(task->key);
			task->val = *value_it;
			LeafNode->valueList.remove(*value_it);
			LeafNode->nodeSize--;
			if (pos == 0 && LeafNode->leftNode == NULL)
			{
				return true;
			}
			else if (pos < 0)
			{
				return false;
			}
			else if (pos == 0)
			{
				task->newKeys->push_back(LeafNode->keyList.front());
				task->oldKeys->push_back(task->key);
			}
			return true;
		}
	}
	return false;
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
		if (count == topnode->nodeSize)
		{
			topnode->childNodePtrs.push_back(newnode);
		}
		else
		{
			node_it++;
			topnode->childNodePtrs.insert(node_it, newnode);
		}
	}
	if (fullnode->childNodePtrs.empty())
	{
		key_it = fullnode->keyList.begin();
		advance(key_it, (LeafOrder + 1) / 2);
		fullnode->keyList.splice(newnode->keyList.begin(), fullnode->keyList, key_it, fullnode->keyList.end());
		fullnode->nodeSize = (LeafOrder + 1) / 2;
		newnode->nodeSize = LeafOrder + 1 - (LeafOrder + 1) / 2;
		list<string>::iterator value_it = fullnode->valueList.begin();
		advance(value_it, (LeafOrder + 1) / 2);
		fullnode->valueList.splice(newnode->valueList.begin(), fullnode->valueList, value_it, fullnode->valueList.end());
		key_it = topnode->keyList.begin();
		advance(key_it, count);
		if (key_it == topnode->keyList.end())
		{
			topnode->keyList.push_back(newnode->keyList.front());
			topnode->nodeSize++;
		}
		else
		{
			// key_it++;
			topnode->keyList.insert(key_it, newnode->keyList.front());
			topnode->nodeSize++;
		}
	}
	else
	{
		key_it = fullnode->keyList.begin();
		advance(key_it, (NodeOrder - 1) / 2);
		fullnode->keyList.splice(newnode->keyList.begin(), fullnode->keyList, key_it, fullnode->keyList.end());
		fullnode->nodeSize = (NodeOrder - 1) / 2;
		newnode->nodeSize = NodeOrder - NodeOrder / 2;
		node_it = fullnode->childNodePtrs.begin();
		Node *nodePtr;
		advance(node_it, 1 + (NodeOrder - 1) / 2);
		fullnode->childNodePtrs.splice(newnode->childNodePtrs.begin(), fullnode->childNodePtrs, node_it, fullnode->childNodePtrs.end());
		node_it = newnode->childNodePtrs.begin();
		for (node_it; node_it != newnode->childNodePtrs.end(); node_it++)
		{
			nodePtr = *node_it;
			nodePtr->parentPtr = newnode;
		}
		key_it = topnode->keyList.begin();
		advance(key_it, count);
		for (list<Node *>::iterator it = newnode->childNodePtrs.begin(); it != newnode->childNodePtrs.end(); ++it)
		{
			Node *nodep = *it;
			nodep->parentPtr = newnode;
		}
		if (key_it == topnode->keyList.end())
		{
			topnode->keyList.push_back(newnode->keyList.front());
			topnode->nodeSize++;
			newnode->keyList.pop_front();
			newnode->nodeSize--;
		}
		else
		{
			// key_it++;
			topnode->keyList.insert(key_it, newnode->keyList.front());
			topnode->nodeSize++;
			newnode->keyList.pop_front();
			newnode->nodeSize--;
		}
	}
	if (fullnode->rightNode != NULL)
	{
		newnode->rightNode = fullnode->rightNode;
		newnode->rightNode->leftNode = newnode;
		newnode->leftNode = fullnode;
		fullnode->rightNode = newnode;
	}
	else
	{
		fullnode->rightNode = newnode;
		newnode->leftNode = fullnode;
	}
	return topnode;
}

Node *bpt::merge(Node *scantnode)
{
	int sizep = 0;
	int sizen = 0;
	int count = 0;
	list<Node *>::iterator node_it;
	list<string>::iterator key_it;
	if (scantnode->leftNode != NULL && scantnode->leftNode->parentPtr == scantnode->parentPtr)
	{
		sizep = scantnode->leftNode->nodeSize;
	}
	if (scantnode->rightNode != NULL && scantnode->rightNode->parentPtr == scantnode->parentPtr)
	{
		sizen = scantnode->rightNode->nodeSize;
	}
	if (scantnode->childNodePtrs.empty())
	{
		count = (LeafOrder + 1) / 2;
	}
	else
	{
		count = NodeOrder / 2;
	}
	if (sizep > count || sizen > count)
	{
		if (sizep > sizen)
		{
			scantnode->keyList.push_front(scantnode->leftNode->keyList.back());
			scantnode->nodeSize++;
			scantnode->leftNode->keyList.pop_back();
			scantnode->leftNode->nodeSize--;
			int pos = 0;
			node_it = scantnode->parentPtr->childNodePtrs.begin();
			while (*node_it != scantnode)
			{
				node_it++;
				pos++;
			}
			key_it = scantnode->parentPtr->keyList.begin();
			advance(key_it, pos - 1);
			if (scantnode->childNodePtrs.empty())
			{
				scantnode->valueList.push_front(scantnode->leftNode->valueList.back());
				scantnode->leftNode->valueList.pop_back();
				*key_it = scantnode->keyList.front();
			}
			else
			{
				scantnode->childNodePtrs.push_front(scantnode->leftNode->childNodePtrs.back());
				scantnode->leftNode->childNodePtrs.pop_back();
				string str = scantnode->keyList.front();
				scantnode->keyList.front() = *key_it;
				*key_it = str;
			}
		}
		else
		{
			scantnode->keyList.push_back(scantnode->rightNode->keyList.front());
			scantnode->nodeSize++;
			scantnode->rightNode->keyList.pop_front();
			scantnode->rightNode->nodeSize--;
			int pos = 0;
			node_it = scantnode->parentPtr->childNodePtrs.begin();
			while (*node_it != scantnode)
			{
				node_it++;
				pos++;
			}
			key_it = scantnode->parentPtr->keyList.begin();
			advance(key_it, pos);
			if (scantnode->childNodePtrs.empty())
			{
				scantnode->valueList.push_back(scantnode->rightNode->valueList.front());
				scantnode->rightNode->valueList.pop_front();
				*key_it = scantnode->rightNode->keyList.front();
			}
			else
			{
				scantnode->childNodePtrs.push_front(scantnode->leftNode->childNodePtrs.back());
				scantnode->leftNode->childNodePtrs.pop_back();
				string str = scantnode->keyList.back();
				scantnode->keyList.back() = *key_it;
				*key_it = str;
			}
		}
	}
	else
	{
		Node *TopNode = scantnode->parentPtr;
		Node *LeftNode;
		Node *RightNode;
		if ((sizep != 0 && sizep <= sizen) || sizen == 0)
		{
			LeftNode = scantnode->leftNode;
			RightNode = scantnode;
		}
		else if ((sizen != 0 && sizen <= sizep) || sizep == 0)
		{
			RightNode = scantnode->rightNode;
			LeftNode = scantnode;
		}
		node_it = TopNode->childNodePtrs.begin();
		count = 0;
		while (*node_it != RightNode)
		{
			node_it++;
			count++;
		}
		key_it = TopNode->keyList.begin();
		advance(key_it, count - 1);
		if (LeftNode->childNodePtrs.empty())
		{
			if (*key_it == RightNode->keyList.front())
			{
				TopNode->keyList.remove(RightNode->keyList.front());
			}
			LeftNode->valueList.splice(LeftNode->valueList.end(), RightNode->valueList);
			LeftNode->keyList.splice(LeftNode->keyList.end(), RightNode->keyList);
			LeftNode->nodeSize += RightNode->nodeSize;
		}
		else
		{
			LeftNode->keyList.splice(LeftNode->keyList.end(), TopNode->keyList, key_it);
			LeftNode->keyList.splice(LeftNode->keyList.end(), RightNode->keyList);
			Node *nodePtr;
			list<Node *>::iterator childNode_it = RightNode->childNodePtrs.begin();
			for (childNode_it; childNode_it != RightNode->childNodePtrs.end(); childNode_it++)
			{
				nodePtr = *childNode_it;
				nodePtr->parentPtr = LeftNode;
			}
			LeftNode->childNodePtrs.splice(LeftNode->childNodePtrs.end(), RightNode->childNodePtrs);
			LeftNode->nodeSize += RightNode->nodeSize + 1;
		}
		LeftNode->rightNode = RightNode->rightNode;
		if (RightNode->rightNode != NULL)
		{
			RightNode->rightNode->leftNode = LeftNode;
		}
		TopNode->nodeSize--;
		TopNode->childNodePtrs.remove(*node_it);
		free(RightNode);
		if (TopNode->parentPtr == NULL && TopNode->nodeSize == 0)
		{
			root = LeftNode;
			root->parentPtr = NULL;
			free(TopNode);
			return root;
		}
		return TopNode;
	}
	return scantnode->parentPtr;
}