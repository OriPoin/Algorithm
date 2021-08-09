#include <iostream>
#include <string>
#include <list>

using namespace std;

struct Node
{
    /*----root---begin----*/
    Node *parentPtr = NULL;
    /*----root---begin----*/
    list<string> keyList;
    uint32_t nodeSize = 0;
    list<Node *> childNodePtrs;
    /*----leaf---begin----*/
    Node *leftNode = NULL;
    Node *rightNode = NULL;
    list<string> valueList;
    list<uint32_t> indexList;
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
    string key;
    string val;
    list<string> *newKeys;
    list<string> *oldKeys;
};

class bpt
{
private:
    bool memMode;
    uint32_t NodeOrder;
    uint32_t LeafOrder;
    Node *root;
    Node *leafHead;
    string dbName;
    uint32_t nodeOffset;
    uint32_t dataOffset;

    int SearchKey(Node *node, string key);
    bool operate(Node *LeafNode, Task *task);
    Node *split(Node *fullnode);
    Node *merge(Node *scantnode);
    Node *LocateLeaf(Node *root, string key);
    void backtrace(Node *LeafNode, Task *task);
    void initDB(string fname);
    string fReadData(string key, uint32_t lineNum);
    uint32_t fWriteData(string key, string value);

public:
    bpt(uint32_t NodeOrder, uint32_t LeafOrder, string dbName);
    void setDBname(string name);
    void setmode(string name);
    void setNodeOrder(uint32_t order);
    void setLeafOrder(uint32_t ordee);
    bool init(char *fname);
    bool load(char *fname);
    bool ins(string key, string value);
    bool select(string key);
    bool update(string key, string value);
    bool del(string key);
    bool rselect(string minkey, string maxkey);
    void serialize(string fname);
    void Trim(string fname);
    void cacheData(string fname);
};