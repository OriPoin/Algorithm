#include <iostream>
#include <algorithm>
#include <fstream>
#include "bpt.h"

bpt::bpt(uint32_t NodeOrder, uint32_t LeafOrder, string dbName)
{
	setNodeOrder(NodeOrder);
	setLeafOrder(LeafOrder);
	setDBname(dbName);
	root = new Node;
	leafHead = root;
	memMode = true;
	this->nodeOffset = 0;
	this->dataOffset = 0;
}

void bpt::setDBname(string name)
{
	this->dbName = name;
}
void bpt::setNodeOrder(uint32_t order)
{
	this->NodeOrder = order;
}
void bpt::setLeafOrder(uint32_t order)
{
	this->LeafOrder = order;
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

bool bpt::load(char *fname)
{
	ifstream dbfile;
	dbfile.open(fname);
	string data = "NULL";
	uint32_t nodeCount = 0;
	uint32_t lineCount = 0;
	Node *nodePtr = root;
	Node *headParentPtr = NULL;
	Node *leftChildPtr = NULL;
	Node *rightChildPtr = NULL;
	string lineHeadstr;
	while (dbfile >> data)
	{
		if (data == "nodeorder")
		{
			dbfile >> data;
			setNodeOrder(stoi(data));
			nodeOffset = 1;
		}
		else if (data == "leaforder")
		{
			dbfile >> data;
			setLeafOrder(stoi(data));
			nodeOffset = 2;
		}
		else if (data == "node")
		{
			dbfile >> data;
			nodeOffset++;
			while (data != "data")
			{
				dbfile >> data;
				nodeCount = 0;
				leftChildPtr = new Node;
				leftChildPtr->parentPtr = nodePtr;
				nodePtr->childNodePtrs.emplace_back(leftChildPtr);
				if (nodePtr->leftNode != NULL)
				{
					nodePtr->leftNode->childNodePtrs.back()->rightNode = leftChildPtr;
					leftChildPtr->leftNode = nodePtr->leftNode->childNodePtrs.back();
				}
				while (data != "key" && data != "data")
				{
					nodeCount++;
					nodePtr->keyList.emplace_back(data);
					dbfile >> data;
					rightChildPtr = new Node;
					leftChildPtr->rightNode = rightChildPtr;
					rightChildPtr->leftNode = leftChildPtr;
					rightChildPtr->parentPtr = nodePtr;
					nodePtr->childNodePtrs.emplace_back(rightChildPtr);
					leftChildPtr = rightChildPtr;
				}
				nodePtr->nodeSize = nodeCount;
				if (nodePtr->rightNode != NULL)
				{
					nodePtr = nodePtr->rightNode;
				}
				else
				{
					Node *headnode = nodePtr->childNodePtrs.front();
					while (headnode->leftNode != NULL)
					{
						headnode = headnode->leftNode;
					}
					nodePtr = headnode;
				}
				nodeOffset++;
			}
		}
		else if (data == "key")
		{
			leafHead = nodePtr;
			nodeOffset++;
			while (1)
			{
				nodeOffset++;
				nodeCount = 0;
				while (data != "segment")
				{
					dbfile >> data;
					nodeCount++;
					nodePtr->keyList.emplace_back(data);
					dbfile >> data;
					dbfile >> data;
					if (memMode)
					{
						nodePtr->valueList.emplace_back(data);
					}
					else
					{
						nodePtr->indexList.emplace_back(nodeOffset);
					}
					dbfile >> data;
				}
				dbfile >> data;
				nodePtr->nodeSize = nodeCount;
				if (nodePtr->rightNode != NULL)
				{
					nodePtr = nodePtr->rightNode;
				}
				else
				{
					return true;
				}
			}
		}
	}
	dbfile.close();
	return true;
}

void bpt::serialize(string fname)
{
	// this->memMode = false;
	// cacheData(dbName);
	string fstr = fname;
	fstr.append(".tmp");
	initDB(fstr);
	Trim(fstr);
	rename(fstr.c_str(), dbName.c_str());
}

void bpt::setmode(string mode)
{
	if (!this->memMode && mode == "mem")
	{
		serialize(dbName);
		cacheData(dbName);
		this->memMode = true;
	}
	else if (this->memMode && mode == "fs")
	{
		initDB(dbName);
		if (root != leafHead || root->nodeSize != 0)
		{
			Trim(dbName);
		}
		this->memMode = false;
	}
}

void bpt::initDB(string fname)
{
	fstream dbfile;
	dbfile.open(fname, fstream::in | fstream::out | fstream::trunc);
	dbfile << "head"
		   << "\n"
		   << "nodeorder " << this->NodeOrder << "\n"
		   << "leaforder " << this->LeafOrder << "\n"
		   << "node"
		   << "\n";
	nodeOffset = 4;
	Node *nodePtr;
	Node *childHeadPtr;
	list<string>::iterator key_it;
	list<string>::iterator value_it;
	list<Node *> node_it;
	list<Node *> child_it;
	nodePtr = root;
	childHeadPtr = root->childNodePtrs.front();
	while (childHeadPtr != NULL)
	{
		dbfile << "key ";
		for (key_it = nodePtr->keyList.begin(); key_it != nodePtr->keyList.end(); key_it++)
		{
			dbfile << *key_it << " ";
		}
		dbfile << "\n";
		nodeOffset++;
		if (nodePtr->rightNode == NULL)
		{
			nodePtr = childHeadPtr;
			if (nodePtr == leafHead)
			{
				childHeadPtr = NULL;
			}
			else
			{
				childHeadPtr = nodePtr->childNodePtrs.front();
			}
		}
		else
		{
			nodePtr = nodePtr->rightNode;
		}
		dbfile.sync();
	}
	dbfile << "data"
		   << "\n";
	nodeOffset++;
	dataOffset = nodeOffset;
	dbfile.close();
}

void bpt::Trim(string fname)
{
	fstream dbfile;
	dbfile.open(fname, fstream::in | fstream::out);
	list<string>::iterator key_it;
	list<string>::iterator value_it;
	list<uint32_t>::iterator index_it;
	Node *nodePtr;
	nodePtr = leafHead;
	string dataBeginStr = "NULL";
	while (dataBeginStr != "data")
	{
		dbfile >> dataBeginStr;
	}
	dbfile << "\n";
	nodePtr = leafHead;
	while (1)
	{
		if (nodePtr == NULL)
		{
			break;
		}
		key_it = nodePtr->keyList.begin();
		value_it = nodePtr->valueList.begin();
		index_it = nodePtr->indexList.begin();
		for (uint32_t i = 0; i < nodePtr->nodeSize; i++)
		{
			dbfile << "key " << *key_it << " ";
			if (memMode)
			{
				dbfile << "value " << *value_it << "\n";
				nodePtr->indexList.emplace_back(dataOffset);
			}
			else
			{
				dbfile << "value " << fReadData(*key_it, *index_it) << "\n";
				*index_it = dataOffset;
				index_it++;
			}
			key_it++;
			value_it++;
			dataOffset++;
		}
		nodePtr->valueList.clear();
		dbfile << "segment\n";
		dbfile.sync();
		dataOffset++;
		nodePtr = nodePtr->rightNode;
	}
	dbfile.close();
}

void bpt::cacheData(string fname)
{
	list<string>::iterator key_it;
	list<uint32_t>::iterator index_it;
	Node *nodePtr = leafHead;
	while (1)
	{
		if (nodePtr == NULL)
		{
			break;
		}
		key_it = nodePtr->keyList.begin();
		index_it = nodePtr->indexList.begin();
		for (uint32_t i = 0; i < nodePtr->nodeSize; i++)
		{
			nodePtr->valueList.emplace_back(fReadData(*key_it, *index_it));
			key_it++;
			index_it++;
		}
		nodePtr->indexList.clear();
		nodePtr = nodePtr->rightNode;
	}
}

string bpt::fReadData(string key, uint32_t lineNum)
{
	fstream dbfile;
	dbfile.open(dbName, fstream::in);
	for (int currLineNumber = 0; currLineNumber < lineNum; ++currLineNumber)
	{
		if (dbfile.ignore(numeric_limits<streamsize>::max(), dbfile.widen('\n')))
		{
			// skipping the line
		}
	}
	string data;
	dbfile >> data;
	if (data == "key")
	{
		dbfile >> data;
		if (data == key)
		{
			dbfile >> data;
			dbfile >> data;
			dbfile.close();
			return data;
		}
	}
	dbfile.close();
	return "False";
}

uint32_t bpt::fWriteData(string key, string value)
{
	fstream dbfile;
	dbfile.open(dbName, fstream::in | fstream::out);
	for (int currLineNumber = 0; currLineNumber < dataOffset; ++currLineNumber)
	{
		if (dbfile.ignore(numeric_limits<streamsize>::max(), dbfile.widen('\n')))
		{
			// skipping the line
		}
	}
	dbfile << "key " << key << " "
		   << "value " << value << "\n";
	dataOffset++;
	dbfile.close();
	return dataOffset - 1;
}

bool bpt::ins(string key, string value)
{
	list<string> *newKeys = new list<string>();
	list<string> *oldKeys = new list<string>();
	Task *task = new Task{false, insertOpt, key, value, newKeys, oldKeys};
	Node *LeafNode = LocateLeaf(root, key);
	task->state = operate(LeafNode, task);
	backtrace(LeafNode, task);
	delete newKeys;
	delete oldKeys;
	if (task->state)
	{
		delete task;
		return true;
	}
	else
	{
		cout << "Conflicting!" << endl;
		cout << "Original data: " << task->val << " New data: " << value << endl;
		delete task;
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
	delete newKeys;
	delete oldKeys;
	if (task->state)
	{
		cout << "Index: " << key << endl;
		cout << "Value: " << task->val << endl;
		delete task;
		return true;
	}
	else
	{
		cout << "No such record!" << endl;
		delete task;
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
	delete newKeys;
	delete oldKeys;
	if (task->state)
	{
		cout << "Index: " << key << endl;
		cout << "Value: " << task->val << " => " << value << endl;
		delete task;
		return true;
	}
	else
	{
		cout << "No such record!" << endl;
		delete task;
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
	delete newKeys;
	delete oldKeys;
	if (task->state)
	{
		cout << "Deleted record" << endl;
		cout << "Index: " << key << endl;
		cout << "Value: " << task->val << endl;
		delete task;
		return true;
	}
	else
	{
		cout << "No such record!" << endl;
		delete task;
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
		advance(key_it, middle);
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
	// First Node or first leaf
	if (pos < 0 && task->opt == insertOpt)
	{
		LeafNode->keyList.emplace_front(task->key);
		if (memMode)
		{
			LeafNode->valueList.emplace_front(task->val);
		}
		else
		{
			LeafNode->indexList.emplace_front(fWriteData(task->key, task->val));
		}
		LeafNode->nodeSize++;
		return true;
	}
	list<string>::iterator key_it;
	list<string>::iterator value_it;
	list<uint32_t>::iterator index_it;
	key_it = LeafNode->keyList.begin();
	advance(key_it, pos);
	if (task->opt == insertOpt && *key_it < task->key)
	{
		if (pos >= LeafNode->nodeSize - 1)
		{
			LeafNode->keyList.emplace_back(task->key);
			if (memMode)
			{
				LeafNode->valueList.emplace_back(task->val);
			}
			else
			{
				index_it = LeafNode->indexList.begin();
				advance(index_it, pos + 1);
				LeafNode->indexList.emplace(index_it, fWriteData(task->key, task->val));
			}
			LeafNode->nodeSize++;
			return true;
		}
		else
		{
			key_it++;
			LeafNode->keyList.emplace(key_it, task->key);
			if (memMode)
			{
				value_it = LeafNode->valueList.begin();
				advance(value_it, pos + 1);
				LeafNode->valueList.emplace(value_it, task->val);
			}
			else
			{
				index_it = LeafNode->indexList.begin();
				advance(index_it, pos + 1);
				LeafNode->indexList.emplace(index_it, fWriteData(task->key, task->val));
			}
			LeafNode->nodeSize++;
			return true;
		}
	}
	else if (*key_it == task->key)
	{
		if (memMode)
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
					task->newKeys->emplace_back(LeafNode->keyList.front());
					task->oldKeys->emplace_back(task->key);
				}
				return true;
			}
		}
		else
		{
			index_it = LeafNode->indexList.begin();
			advance(index_it, pos);
			if (task->opt == insertOpt)
			{
				task->val = fReadData(task->key, *index_it);
				return false;
			}
			else if (task->opt == selectOpt)
			{
				task->val = fReadData(task->key, *index_it);
				return true;
			}
			else if (task->opt == modifyOpt)
			{
				string str = fReadData(task->key, *index_it);
				*index_it = fWriteData(task->key, task->val);
				task->val = str;
				return true;
			}
			else if (task->opt == deletOpt)
			{
				LeafNode->keyList.remove(task->key);
				task->val = fReadData(task->key, *index_it);
				LeafNode->indexList.remove(*index_it);
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
					task->newKeys->emplace_back(LeafNode->keyList.front());
					task->oldKeys->emplace_back(task->key);
				}
				return true;
			}
		}
	}
	return false;
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
		topnode->childNodePtrs.emplace_back(fullnode);
		topnode->childNodePtrs.emplace_back(newnode);
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
			topnode->childNodePtrs.emplace_back(newnode);
		}
		else
		{
			node_it++;
			topnode->childNodePtrs.emplace(node_it, newnode);
		}
	}
	if (fullnode->childNodePtrs.empty())
	{
		key_it = fullnode->keyList.begin();
		advance(key_it, (LeafOrder + 1) / 2);
		fullnode->keyList.splice(newnode->keyList.begin(), fullnode->keyList, key_it, fullnode->keyList.end());
		fullnode->nodeSize = (LeafOrder + 1) / 2;
		newnode->nodeSize = LeafOrder + 1 - (LeafOrder + 1) / 2;
		if (memMode)
		{
			list<string>::iterator value_it = fullnode->valueList.begin();
			advance(value_it, (LeafOrder + 1) / 2);
			fullnode->valueList.splice(newnode->valueList.begin(), fullnode->valueList, value_it, fullnode->valueList.end());
		}
		else
		{
			list<uint32_t>::iterator index_it = fullnode->indexList.begin();
			advance(index_it, (LeafOrder + 1) / 2);
			fullnode->indexList.splice(newnode->indexList.begin(), fullnode->indexList, index_it, fullnode->indexList.end());
		}
		key_it = topnode->keyList.begin();
		advance(key_it, count);
		if (key_it == topnode->keyList.end())
		{
			topnode->keyList.emplace_back(newnode->keyList.front());
			topnode->nodeSize++;
		}
		else
		{
			topnode->keyList.emplace(key_it, newnode->keyList.front());
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
			topnode->keyList.emplace_back(newnode->keyList.front());
			topnode->nodeSize++;
			newnode->keyList.pop_front();
			newnode->nodeSize--;
		}
		else
		{
			topnode->keyList.emplace(key_it, newnode->keyList.front());
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
			scantnode->keyList.emplace_front(scantnode->leftNode->keyList.back());
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
				if (memMode)
				{
					scantnode->valueList.splice(scantnode->valueList.begin(), scantnode->leftNode->valueList, scantnode->leftNode->valueList.end());
				}
				else
				{
					scantnode->indexList.splice(scantnode->indexList.begin(), scantnode->leftNode->indexList, scantnode->leftNode->indexList.end());
				}
				*key_it = scantnode->keyList.front();
			}
			else
			{
				scantnode->childNodePtrs.emplace_front(scantnode->leftNode->childNodePtrs.back());
				scantnode->leftNode->childNodePtrs.pop_back();
				string str = scantnode->keyList.front();
				scantnode->keyList.front() = *key_it;
				*key_it = str;
			}
		}
		else
		{
			scantnode->keyList.emplace_back(scantnode->rightNode->keyList.front());
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
				if (memMode)
				{
					scantnode->valueList.splice(scantnode->valueList.end(), scantnode->rightNode->valueList, scantnode->rightNode->valueList.begin());
				}
				else
				{
					scantnode->indexList.splice(scantnode->indexList.end(), scantnode->rightNode->indexList, scantnode->rightNode->indexList.begin());
				}
				*key_it = scantnode->rightNode->keyList.front();
			}
			else
			{
				scantnode->childNodePtrs.emplace_front(scantnode->leftNode->childNodePtrs.back());
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
			if (memMode)
			{
				LeftNode->valueList.splice(LeftNode->valueList.end(), RightNode->valueList);
			}
			else
			{
				LeftNode->indexList.splice(LeftNode->indexList.end(), RightNode->indexList);
			}
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
		delete RightNode;
		if (TopNode->parentPtr == NULL && TopNode->nodeSize == 0)
		{
			root = LeftNode;
			root->parentPtr = NULL;
			delete TopNode;
			return root;
		}
		return TopNode;
	}
	return scantnode->parentPtr;
}