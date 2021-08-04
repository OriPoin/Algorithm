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
    bool cacaheFlag = 0;
    Node *leftNode = NULL;
    Node *rightNode = NULL;
    uint32_t nodeOffset = 0;
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
    int NodeOrder;
    int LeafOrder;
    Node *root;
    Node *leafHead;
    string dbName;

    Node *split(Node *fullnode);
    Node *merge(Node *scantnode);
    int SearchKey(Node *node, string key);

public:
    bpt(int NodeOrder, int LeafOrder);
    bool init(char *fname);
    void setorder(int order);
    bool ins(string key, string value);
    bool select(string key);
    bool update(string key, string value);
    bool del(string key);
    bool rselect(string minkey, string maxkey);
    Node *LocateLeaf(Node *root, string key);
    bool operate(Node *LeafNode, Task *task);
    void backtrace(Node *LeafNode, Task *task);
    void deserialize(string fname);
    void serialize(string fname);
};