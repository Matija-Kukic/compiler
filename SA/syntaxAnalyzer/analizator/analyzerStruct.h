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

#endif // !ANALYZERSTRUCT
