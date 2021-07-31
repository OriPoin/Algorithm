#include <iostream>
#include <string>
#include <list>

using namespace std;

struct Node
{
    /*----root---begin----*/
    Node *parentPtr = NULL;
    /*----root---begin----*/
    list<string> key;
    uint32_t size = 0;
    list<Node *> childNodePtrs;
    /*----leaf---begin----*/
    uint32_t nodeoffset = 0;
    bool cacahed = 0;
    Node *previous = NULL;
    Node *next = NULL;
    list<uint32_t> index;
    list<string> values;
    /*----leaf----end-----*/
};

enum opt
{
    insertion,
    selection,
    modification,
    delection,
};

struct State{
    bool flag;
    string value;
};

class bpt
{
private:
    int order;
    Node *root;
    string dbname;

    bool iscant(Node *node);
    Node *split(Node *fulnode);
    int addrecord(string key, string index, string value);
    Node *merge(Node *node);

public:
    bpt(int order);
    bool init(char *fname);
    void setorder(int order);
    bool ins(string key, string value);
    bool select(string key);
    void update(string key, string value);
    void del(string key);
    bool rselect(string minkey,string maxkey);
    State operate(opt option, string key, string value);
    void deserialize(string fname);
    void serialize(string fname);
};