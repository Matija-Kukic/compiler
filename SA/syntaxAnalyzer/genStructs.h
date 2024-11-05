#ifndef GENSTRUCTS
#define GENSTRUCTS
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace std;

vector<string> splitSpaces(string ent);
pair<string, string> splitSemi(vector<string> &v);
unsigned long long hashFunc(set<int> &s);
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
    map<pair<string, string>, int> seqMap;
    map<pair<string, string>, int> prodMap;
    int seq = 0, seq2 = 0;
};
struct DFA {
    vector<int> states;
    map<int, set<pair<string, string>>> oldTr;
    map<unsigned long long int, int> hashMap;
    int startState = 0;
    map<pair<int, string>, int> tr;
};
struct tables {
    map<int, map<string, string>> action;
    map<int, map<string, string>> new_state;
};
#endif // !GENSTRUCTS
