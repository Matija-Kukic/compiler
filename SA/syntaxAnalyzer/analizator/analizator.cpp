#include "analyzerStruct.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>
using namespace std;

int main() {
    string line;
    int I = 0;
    tables tab;
    ifstream file("table.txt");
    char state = ' ';
    while (getline(file, line)) {
        if (I < 3) {
            vector<string> v = splitSpaces(line);
            if (v[0] == "%T") {
                for (int i = 1; i < v.size(); i++) {
                    tab.fnSign.insert(v[i]);
                }
            }
            if (v[0] == "%V") {
                for (int i = 1; i < v.size(); i++) {
                    tab.unSign.insert(v[i]);
                }
            }
            if (v[0] == "%Syn") {
                for (int i = 1; i < v.size(); i++) {
                    tab.syn.insert(v[i]);
                }
            }
            I++;
            continue;
        }
        if (line == "Productions") {
            state = 'p';
            continue;
        }
        if (line == "Actions") {
            state = 'a';
            continue;
        }
        if (line == "NEW STATES") {
            state = 'n';
            continue;
        }
        if (state == 'p') {
            vector<string> v = splitSpaces(line);
            string uf = v[0];
            string prod = "";
            for (int i = 1; i < v.size() - 1; i++) {
                prod += v[i];
                if (i < v.size() - 2)
                    prod += " ";
            }
            int st = stoi(v[v.size() - 1]);
            tab.prodMap[st] = {uf, prod};
        }
        if (state == 'a') {
            vector<string> v = splitSpaces(line);
            int cs = stoi(v[0]);
            string trsg = v[1];
            string act = v[2];
            tab.action[cs][trsg] = act;
        }
        if (state == 'n') {
            vector<string> v = splitSpaces(line);
            int cs = stoi(v[0]);
            string tr = v[1];
            string ns = v[2];
            tab.new_state[cs][tr] = ns;
        }
    }
    file.close();
    // cout << "Unfinished signs: ";
    // for (const auto &item : tab.unSign) {
    //     cout << item << " ; ";
    // }
    // cout << endl << "Finished signs: ";
    // for (const auto &item : tab.fnSign) {
    //     cout << item << " ; ";
    // }
    // cout << endl << "Sync signs: ";
    // for (const auto &item : tab.syn) {
    //     cout << item << " ; ";
    // }
    // cout << endl;
    cout << "Productions" << endl;
    for (const auto &it : tab.prodMap) {
        cout << it.first << ", " << it.second.first << " : " << it.second.second
             << endl;
    }
    cout << "ACTION TABLE" << endl;
    cout << "Actions" << endl;
    for (const auto &it : tab.action) {
        for (const auto &it2 : it.second) {
            cout << it.first << " " << it2.first << " " << it2.second << endl;
        }
    }
    cout << "NEW STATES" << endl;
    for (const auto &it : tab.new_state) {
        for (const auto &it2 : it.second) {
            cout << it.first << " " << it2.first << " " << it2.second << endl;
        }
    }
    // LU lu("bla", 1, "bla");
    // cout << "BLA" << endl;
    // cout << lu.sign << " " << lu.row << " " << lu.content << endl;
    vector<LU> input;
    Tree tree;
    int last_id = 0;
    map<int, int> lum;
    while (getline(cin, line)) {
        vector<string> v = splitSpaces(line);
        string sign = v[0];
        int row = stoi(v[1]);
        string content = "";
        int rz = 0, I = 0;
        for (int i = 0; i < line.size(); i++) {
            if (line[i] == ' ')
                rz++;
            if (rz == 2) {
                I = i + 1;
                break;
            }
        }
        content = line.substr(I);
        LU lu(sign, row, content);
        tree.sm[last_id] = sign;
        last_id++;
        input.push_back(lu);
    }
    string endCh = "\u22A5";
    input.push_back(LU(endCh, -1, "BLA"));
    tree.sm[last_id] = endCh;
    last_id++;
    // for (int i = 0; i < input.size(); i++) {
    //     cout << input[i].sign << " " << input[i].row << " " <<
    //     input[i].content
    //          << " " << endl;
    // }
    // cout << last_id << endl;
    int crs = 0; // index of current readed input
    stack<LRI> stack;
    string qf = "\u2207";
    LRI l1(0, last_id, qf);
    tree.sm[last_id] = qf;
    last_id++;
    for (const auto &e : tree.sm) {
        cout << e.first << " " << e.second << endl;
    }
    stack.push(l1);
    string red_color = "\033[31m";
    string green_color = "\033[32m";
    string reset = "\033[0m";
    while (1) {
        if (crs >= input.size())
            crs = input.size() - 1;
        string csign = input[crs].sign;
        int crow = input[crs].row;
        int tlrs = stack.top().lrs;
        int tid = stack.top().id;
        string ts = stack.top().s;
        bool Act = (tab.action.find(tlrs) != tab.action.end() &&
                    tab.action[tlrs].find(csign) != tab.action[tlrs].end());
        if (Act) {
            string act = tab.action[tlrs][csign];
            cout << act << endl;
            if (act[0] == 'p') {
                int nlrs = stoi(act.substr(1));
                string ns = csign;
                int nid = crs;
                crs++;
                stack.push(LRI(nlrs, nid, ns));
            } else if (act[0] == 'r') {
                int rn = stoi(act.substr(1));
                string uf = tab.prodMap[rn].first,
                       prod = tab.prodMap[rn].second;
                if (prod != "$") {
                    int olrs = stack.top().lrs;
                    vector<string> v = splitSpaces(prod);
                    for (int i = v.size() - 1; i >= 0; --i) {
                        string ss = v[i];
                        string sts = stack.top().s;
                        if (sts == ss) {
                            stack.pop();
                        } else {
                            cerr << red_color << "GRESKA" << reset << endl;
                            exit(0);
                        }
                    }
                }
                int olrs = stack.top().lrs;
                int nlrs;
                bool Act2 =
                    (tab.new_state.find(olrs) != tab.new_state.end() &&
                     tab.new_state[olrs].find(uf) != tab.new_state[olrs].end());
                if (Act2) {
                    nlrs = stoi(tab.new_state[olrs][uf].substr(1));
                } else {
                    cerr << red_color << "GRESKA2" << reset << endl;
                    break;
                }
                int nid = last_id;
                last_id++;
                tree.sm[nid] = uf;
                stack.push(LRI(nlrs, nid, uf));
            } else if (act == "Acc") {
                cout << green_color << "Prihvacen" << reset << endl;
                break;
            }
        } else {
            cerr << red_color << "STACK " << tid << " " << tlrs << " " << ts
                 << endl
                 << "READ " << crs << " " << csign << reset << endl;
            break;
        }
    }
    while (!stack.empty()) {
        cout << stack.top().s << " " << stack.top().id << " " << stack.top().lrs
             << endl;
        stack.pop();
    }
    return 0;
}
