#ifndef ANALYZERSTRUCT
#define ANALYZERSTRUCT
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace std;

vector<string> splitSpaces(string ent);
pair<string, string> splitSemi(vector<string> &v);
struct tables {
    map<int, map<string, string>> action;
    map<int, map<string, string>> new_state;
    set<string> unSign, fnSign, syn;
    map<int, pair<string, string>> prodMap;
};
// struct for a lexic unit
struct LU {
    int row;
    string content;
    string sign;
    LU(string sign, int row, string content);
};
struct Tree {
    map<int, vector<int>> m;
    map<int, string> sm;
    int root_id;
};
struct LRI {
    int lrs; // LR state
    int id;
    string s;
    LRI(int lrs, int id, string s);
};
#endif // !ANALYZERSTRUCT
