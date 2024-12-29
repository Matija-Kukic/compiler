#ifndef STRUCTS
#define STRUCTS

#include <iostream>
#include <map>
#include <memory>
#include <vector>
using namespace std;
enum type {
    Int = 10,
    Char = 20,
    Void = 30,
    kint = 40,
    kchar = 50,
    arrint = 60,
    arrchar = 70,
    arrkint = 80,
    arrkchar = 90,
    Func = 100,
    Error = 110
};

struct Type {
    enum type type;
    enum type retyp;
    vector<enum type> fparam;
    int numElem;
    bool lexp;
    Type() : type(Error), numElem(-1), lexp(false) {}

    explicit Type(enum type type, int ne, bool lexp)
        : type(type), numElem(ne), lexp(lexp) {}

    explicit Type(enum type type, bool lexp)
        : type(type), numElem(-1), lexp(lexp) {}
};
struct treeNode {
    shared_ptr<treeNode> parent;
    vector<shared_ptr<treeNode>> children;
    map<string, Type> table;
    ~treeNode() { cerr << "treeNODE destroyed" << endl; }
};

#endif // !GENSTRUCTS
