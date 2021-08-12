#ifndef __BPT_H
#define __BPT_H

#include <iostream>
#include <string>
#include <list>

struct Node
{
    /*----root---begin----*/
    Node *parentPtr = NULL;
    /*----root---begin----*/
    std::list<std::string> keyList;
    uint32_t nodeSize = 0;
    std::list<Node *> childNodePtrs;
    /*----leaf---begin----*/
    Node *leftNode = NULL;
    Node *rightNode = NULL;
    std::list<std::string> valueList;
    std::list<uint32_t> indexList;
    /*----leaf----end-----*/
};

enum Option
{
    insertOpt,
    selectOpt,
    modifyOpt,
    deletOpt,
};
struct Task
{
    bool state;
    Option opt;
    std::string key;
    std::string val;
    std::list<std::string> *newKeys;
    std::list<std::string> *oldKeys;
};

class bpt
{
private:
    bool memMode;
    uint32_t NodeOrder;
    uint32_t LeafOrder;
    Node *root;
    Node *leafHead;
    std::string dbName;
    uint32_t nodeOffset;
    uint32_t dataOffset;

    int SearchKey(Node *node, std::string key);
    bool operate(Node *LeafNode, Task *task);
    Node *split(Node *fullnode);
    Node *merge(Node *scantnode);
    Node *LocateLeaf(Node *root, std::string key);
    void backtrace(Node *LeafNode, Task *task);
    void initDB(std::string fname);
    std::string fReadData(std::string key, uint32_t lineNum);
    uint32_t fWriteData(std::string key, std::string value);

public:
    bpt(uint32_t NodeOrder, uint32_t LeafOrder, std::string dbName);
    ~bpt();
    void setDBname(std::string name);
    void setmode(std::string name);
    void setNodeOrder(uint32_t order);
    void setLeafOrder(uint32_t ordee);
    bool init(char *fname);
    bool load(char *fname);
    bool ins(std::string key, std::string value);
    bool select(std::string key);
    bool update(std::string key, std::string value);
    bool del(std::string key);
    void serialize(std::string fname);
    void Trim(std::string fname);
    void cacheData(std::string fname);
};
#endif