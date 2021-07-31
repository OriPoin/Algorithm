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

class bpt
{
private:
    int order;
    Node *root;
    string dbname;

    bool iscant(Node *node);
    Node* split(Node *fulnode);
    int addrecord(string key, string index, string value);
    Node *merge(Node *node);

public:
    bpt(int order);
    bool init(char *fname);
    void del(string key);
    void deserialize(string fname);
    bool ins(string key, string value);
    bool ins1(string key, string value);
    void select(string key);
    void serialize(string fname);
    void setorder(int order);
    void update(string key, string value);
};