#ifndef GENSTRUCTS
#define GENSTRUCTS

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
using namespace std;

vector<string> splitSpaces(string ent);
pair<string, string> splitSemi(vector<string> &v);
struct Gen {
    set<string> unSign, fnSign, syn,
        empt; // Unfinished signs, finished signs, synced signs, empy signs
    multimap<string, string>
        dict; // grammar of transitions for a given unfinished sing
    map<string, set<string>> stSign, Starts;
    void insertTr(string key, string val);
    string startState;
    multimap<pair<pair<string, string>, string>, pair<string, string>> tr;
    map<string, map<string, int>> visited;
};
struct lrItem {
    string prod;
    string signs;
};
struct DFA {
    vector<int> states;
    map<int, set<pair<string, string>>> oldTr;
    int startState = 0;
    map<pair<int, string>, int> tr;
};
#endif // !GENSTRUCTS
