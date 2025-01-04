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
    List = 120,
    Error = 130
};

struct Type {
    enum type type = Error;
    enum type retyp = Error;
    vector<enum type> fparam = vector<enum type>();
    vector<string> nparam = vector<string>();
    int numElem = -1;
    bool lexp = false;
    bool fundef = false;
    Type() : type(Error), numElem(-1), lexp(false) {}

    explicit Type(enum type type, int ne, bool lexp)
        : type(type), numElem(ne), lexp(lexp) {}

    explicit Type(enum type type, bool lexp)
        : type(type), numElem(-1), lexp(lexp) {}

    explicit Type(enum type type, enum type retyp, bool fundef)
        : type(type), numElem(-1), lexp(0), retyp(retyp), fparam(), nparam(),
          fundef(fundef) {}
    explicit Type(enum type type, const vector<enum type> &fparam,
                  const vector<string> &nparam, enum type retyp, bool fundef)
        : type(type), numElem(-1), lexp(0), fparam(fparam), retyp(retyp),
          nparam(nparam), fundef(fundef) {}
};
struct Lparam {
    vector<enum type> fparam;
    vector<string> nparam;
    Lparam() : fparam(), nparam() {}
};
struct Inistruct { // for inicijalizator return type
    enum type type;
    vector<enum type> types;
    int numElem;
    explicit Inistruct(enum type type, const vector<enum type> &types,
                       int numElem)
        : type(type), types(types), numElem(numElem) {}
};
struct treeNode { // tree for scopes
    shared_ptr<treeNode> parent;
    vector<shared_ptr<treeNode>> children;
    map<string, Type> table;
    string IDN;
    ~treeNode() { cerr << "treeNODE destroyed" << endl; }
};
struct Tree { // tree for inputs
    string node;
    vector<shared_ptr<Tree>> children;
    shared_ptr<Tree> parent;
    Tree(string node) : node(node) {}
    Tree() : node("") {}
    ~Tree() { cerr << "input tree node destroyed" << endl; }
};

#endif // !GENSTRUCTS
