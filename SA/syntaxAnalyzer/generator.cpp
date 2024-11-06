#include "genStructs.h"
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>
using namespace std;

map<string, map<string, int>> Start;
set<string> all;
int main() {
    // string s;
    // getline(cin, s);
    // vector<string> v = splitSpaces(s);
    // for (int i = 0; i < v.size(); i++) {
    //     cout << v[i] << " : ";
    // }
    ofstream file("analizator/table.txt");
    string line;
    vector<string> lines;
    while (getline(cin, line))
        lines.push_back(line);
    Gen g;
    for (int q = 0; q < 3; q++) {
        vector<string> v = splitSpaces(lines[q]);
        if (v[0] == "%V") {
            file << lines[q] << endl;
            for (int i = 1; i < v.size(); i++) {
                if (i == 1)
                    g.startState = v[i];
                g.unSign.insert(v[i]);
                all.insert(v[i]);
            }
        } else if (v[0] == "%T") {
            file << lines[q] << endl;
            for (int i = 1; i < v.size(); i++) {
                g.fnSign.insert(v[i]);
                all.insert(v[i]);
            }
        } else if (v[0] == "%Syn") {
            file << lines[q] << endl;
            for (int i = 1; i < v.size(); i++) {
                g.syn.insert(v[i]);
            }
        }
    }
    // cout << "Debug of dicts:" << endl;
    // cout << "Unfinished signs: ";
    // for (const auto &item : g.unSign) {
    //     cout << item << " ; ";
    // }
    // cout << endl << "Finished signs: ";
    // for (const auto &item : g.fnSign) {
    //     cout << item << " ; ";
    // }
    // cout << endl << "Sync signs: ";
    // for (const auto &item : g.syn) {
    //     cout << item << " ; ";
    // }
    // cout << endl;
    //  refleksivnost starta
    for (const auto &item : g.unSign) {
        Start[item][item] = 2;
    }
    for (const auto &item : g.fnSign) {
        Start[item][item] = 2;
    }
    int Nw = 1; // is new entry added to empty signs
    for (int q = 3; q < lines.size(); q++) {
        string line = lines[q];
        if (g.unSign.count(line) != 0) {
            int a = q + 1;
            while (g.unSign.count(lines[a]) == 0 && a < lines.size()) {
                string exp = lines[a].substr(1);
                g.insertTr(line, exp);
                if (exp == "$") {
                    g.empt.insert(line);
                }
                g.prodMap[{line, exp}] = g.seq2;
                g.seq2++;
                a++;
            }
            q = a - 1;
        }
    }
    // cout << "DICT:" << endl;
    // for (const auto &pair : g.dict) {
    //     cout << pair.first << ": " << pair.second << endl;
    // }
    while (Nw == 1) {
        Nw = 0;
        for (const auto &pair : g.dict) {
            vector<string> v = splitSpaces(pair.second);
            int cnt = 0;
            for (int i = 0; i < v.size(); i++) {
                if (g.empt.count(v[i]) != 0)
                    cnt++;
            }
            if (cnt == v.size()) {
                if (g.empt.count(pair.first) != 0)
                    Nw = 1;
                g.empt.insert(pair.first);
            }
        }
    }
    // cout << "Empty signs: ";
    // for (const auto &item : g.empt) {
    //     cout << item << " ; ";
    // }
    for (const auto &pair : g.dict) {
        string row = pair.first;
        vector v = splitSpaces(pair.second);
        if (v[0] == "$")
            continue;
        for (int i = 0; i < v.size(); i++) {
            string column = v[i];
            Start[row][column] = 1;
            if (g.empt.count(column) == 0)
                break;
        }
    }
    for (int qq = 0; qq < 2; qq++) {
        for (const auto &row : Start) {
            for (const auto &col : row.second) {
                if (col.second == 1 || col.second == 2) {
                    for (const auto &col2 :
                         Start[col.first]) { // col2 is map<string,int>
                        if (col2.second == 1 || col2.second == 2) {
                            Start[row.first][col2.first] = 2;
                        }
                    }
                }
            }
        }
    }
    // cout << endl << "Zapocinje mat:" << endl;
    // for (const auto &row : Start) {
    //     for (const auto &col : row.second) {
    //         std::cout << row.first << ", " << col.first << ": " << col.second
    //                   << std::endl;
    //     }
    // }
    for (const auto &un : g.unSign) {
        for (const auto &fn : g.fnSign) {
            if (Start[un][fn])
                g.stSign[un].insert(fn);
        }
    }
    // cout << "Zapocinje znak:" << endl;
    // for (const auto &pair : g.stSign) {
    //     cout << pair.first << ": ";
    //     for (const auto &item : pair.second) {
    //         cout << item << " ";
    //     }
    //     cout << endl;
    // }
    for (const auto &pair : g.dict) {
        string prod = pair.second;
        vector<string> v = splitSpaces(prod);
        for (int j = 0; j < v.size(); j++) {
            string exp = "";
            for (int c = j; c < v.size(); c++) {
                exp += v[c];
                if (c != v.size() - 1)
                    exp += " ";
            }
            for (int i = j; i < v.size(); i++) {
                if (v[i][0] == '<') {
                    for (const auto &item : g.stSign[v[i]]) {
                        g.Starts[exp].insert(item);
                    }
                    if (g.empt.count(v[i]) == 0)
                        break;
                } else {
                    g.Starts[exp].insert(v[i]);
                    break;
                }
            }
        }
    }
    // cout << "Zapocinje start:" << endl;
    // for (const auto &pair : g.Starts) {
    //     cout << pair.first << ": ";
    //     for (const auto &item : pair.second) {
    //         cout << item << " ";
    //     }
    //     cout << endl;
    // }
    string ss = "<S`>";
    string middleDot = "\u00B7", endCh = "\u22A5";
    // cout << g.startState << endl;
    pair<pair<string, string>, string> first = {
        {ss + " : " + middleDot + " " + g.startState, endCh}, "$"};
    // auto range = g.dict.equal_range(g.startState);
    //  pair<string, string> p = splitSemi(splitSpaces("bla bla2 : bla3 bla4"));
    //  cout << p.first << " XXX " << p.second;
    queue<pair<string, string>> q;
    set<pair<pair<string, string>, string>> check;
    q.push({first.first.first, first.first.second});
    g.seqMap[{first.first.first, first.first.second}] = g.seq;
    g.seq += 1;
    while (!q.empty()) {
        string prod = q.front().first;
        string next = q.front().second;
        q.pop();
        // trying to add epsilon transitions to the current production
        vector<string> v = splitSpaces(prod);
        pair<string, string> pr = splitSemi(v);
        string uf = pr.first, exp = pr.second;
        vector<string> expv = splitSpaces(exp);
        int dotI = 0;
        while (expv[dotI] != middleDot)
            dotI++;
        if (dotI == expv.size() - 1)
            continue;
        if (dotI < expv.size() - 1) {
            int fw = dotI + 1;
            if (expv[fw][0] == '<') {
                int fw2 = fw + 1;
                string ufs = expv[fw];
                string suf = ""; // sufix of a prod
                int ecnt = 0;    // empty count
                int dd = 0;
                for (int i = fw2; i < expv.size(); i++) {
                    suf += expv[i];
                    if (i < expv.size() - 1)
                        suf += " ";
                    if (expv[i][0] == '<') {
                        if (g.empt.count(expv[i]) > 0)
                            ecnt++;
                    } else {
                        dd++;
                    }
                }
                // cout << suf << endl;
                string signs = "";
                if (suf == "") {
                    signs = next;
                } else {
                    int cnt = 0;
                    set<string> sig;
                    for (const auto &it : g.Starts[suf]) {
                        signs += it + " ";
                        sig.insert(it);
                    }
                    vector<string> v2 = splitSpaces(suf);
                    for (int c = 0; c < v2.size(); c++) {
                        if (v2[c][0] == '<') {
                            if (g.empt.count(v2[c]) > 0)
                                cnt++;
                        }
                    }
                    if (ecnt == cnt && cnt != 0 && !dd) {
                        vector<string> help = splitSpaces(next);
                        for (int kk = 0; kk < help.size(); kk++)
                            sig.insert(help[kk]);
                        signs = "";
                        for (const auto &e : sig) {
                            signs += e + " ";
                        }
                    }
                }
                auto range = g.dict.equal_range(ufs);

                pair<pair<string, string>, string> pp = {{prod, next}, "$"};
                for (auto it = range.first; it != range.second; ++it) {
                    string nprod =
                        it->first + " : " + middleDot + " " + it->second;
                    auto rn2 = g.tr.equal_range({{prod, next}, "$"});
                    pair<string, string> ppp = {nprod, signs};
                    int z = 0;
                    for (auto it2 = rn2.first; it2 != rn2.second; ++it2) {
                        if (it2->second == ppp) {
                            z++;
                        }
                    }
                    if (!z) {
                        g.tr.insert({pp, {nprod, signs}});
                        q.push({nprod, signs});
                        g.seqMap[{nprod, signs}] = g.seq;
                        g.seq += 1;
                    }
                }
            }
        }
        // here is the code to move the dot in an item
        string s = "";
        if (exp == middleDot + " $")
            continue;
        for (int i = 0; i < expv.size(); i++) {
            if (expv[i] == middleDot && i != expv.size() - 1) {
                s += expv[i + 1] + " " + middleDot;
                i++;
            } else {
                s += expv[i];
            }
            if (i != expv.size() - 1)
                s += " ";
        }
        string ns = expv[dotI + 1], ne = uf + " : " + s;
        auto rn = g.tr.equal_range({{prod, next}, ns});
        int z2 = 0;
        pair<string, string> pp2 = {ne, next};
        for (auto it = rn.first; it != rn.second; ++it) {
            if (it->second == pp2) {
                z2++;
            }
        }
        if (!z2) {
            g.tr.insert({{{prod, next}, ns}, {ne, next}});
            q.push({uf + " : " + s, next});
            g.seqMap[{ne, next}] = g.seq;
            g.seq += 1;
        }
    }
    // cout << "TEST: " << endl;
    // for (const auto &entry : g.tr) {
    //    cout << "(" << entry.first.first.first << ", "
    //         << entry.first.first.second << "), " << entry.first.second
    //         << ") -> (" << entry.second.first << ", " << entry.second.second
    //         << ")" << endl;
    //}
    // cout << "SEQ MAP" << endl;
    // for (const auto e : g.seqMap) {
    //    cout << e.first.first << " [" << e.first.second << "] -> " << e.second
    //         << endl;
    //}
    set<pair<string, string>> states; // nst are new states
    DFA dfa;
    dfa.states.push_back(0);
    states.insert({first.first.first, first.first.second});
    queue<set<pair<string, string>>> q2;
    queue<int> qi;
    qi.push(0);

    set<int> nstates;
    for (const auto &it : states) {
        q.push(it);
        nstates.insert(g.seqMap[it]);
    }
    while (!q.empty()) {
        pair<string, string> pr = q.front();
        auto rn = g.tr.equal_range({pr, "$"});
        for (auto it = rn.first; it != rn.second; ++it) {
            if (states.count(it->second) == 0) {
                q.push(it->second);
                states.insert(it->second);
                nstates.insert(g.seqMap[it->second]);
            }
        }
        q.pop();
    }
    for (const auto &it : states) {
        dfa.oldTr[0].insert(it);
    }
    q2.push(states);

    unsigned long long d = hashFunc(nstates);
    dfa.hashMap[d] = 0;
    // cout << "TEST " << d << endl;
    while (!q2.empty()) {
        // cout << "TESTING " << endl;
        set<pair<string, string>> curr = q2.front();
        q2.pop();
        int cs = qi.front();
        qi.pop();
        for (const auto &it : all) {
            set<pair<string, string>> nst;
            set<int> inst;
            for (const auto &i : curr) {
                auto rn = g.tr.equal_range({i, it});
                for (auto it2 = rn.first; it2 != rn.second; ++it2) {
                    nst.insert({it2->second});
                    inst.insert(g.seqMap[it2->second]);
                }
            }
            for (const auto &it2 : nst) {
                q.push(it2);
            }
            while (!q.empty()) {
                pair<string, string> pr = q.front();
                auto rn = g.tr.equal_range({pr, "$"});
                for (auto it2 = rn.first; it2 != rn.second; ++it2) {
                    if (nst.count(it2->second) == 0) {
                        q.push(it2->second);
                        nst.insert(it2->second);
                        inst.insert(g.seqMap[it2->second]);
                    }
                }
                q.pop();
            }
            unsigned long long dd = hashFunc(inst);
            int old_in = -1;
            // for (const auto &en : dfa.oldTr) {
            //     set<pair<string, string>> hs = en.second;
            //     if (hs == nst) {
            //         old_in = en.first;
            //     }
            // }
            auto item = dfa.hashMap.find(dd);
            if (item != dfa.hashMap.end()) {
                old_in = dfa.hashMap[dd];
            }
            // cout << dd << " " << old_in << endl;
            if (old_in == -1 && !nst.empty()) {
                int ns = dfa.states.back() + 1;
                dfa.tr[{cs, it}] = ns;
                dfa.states.push_back(ns);
                qi.push(ns);
                q2.push(nst);
                unsigned long long nd = hashFunc(inst);
                dfa.hashMap[nd] = ns;
                for (const auto &it3 : nst) {
                    dfa.oldTr[ns].insert(it3);
                }
                //  cout << cs << " " << it << " " << ns << endl;
            }
            if (old_in != -1 && !nst.empty()) {
                dfa.tr[{cs, it}] = old_in;
            }
        }
    }
    // cout << "DKA STATES" << endl;
    // for (const auto &en : dfa.oldTr) {
    //     cout << en.first << " : " << endl;
    //     for (const auto &e2 : en.second) {
    //         cout << e2.first << " [" << e2.second << " ] " << endl;
    //     }
    // }
    // cout << "transitions " << endl;
    // for (const auto &it : dfa.tr) {
    //     cout << it.first.first << " " << it.first.second << " , " <<
    //     it.second
    //          << endl;
    // }
    // cout << "NKA stats:" << endl;
    // cout << "Broj stanja: " << g.seqMap.size()
    //     << " Broj prijelaza: " << g.tr.size() << endl;
    // cout << "DKA stats:" << endl;
    // cout << "Broj stanja: " << dfa.hashMap.size()
    //     << " Broj prijelaza: " << dfa.tr.size() << endl;
    //  Action table
    cout << "BLA" << endl;
    tables tab;
    for (const auto &e : dfa.oldTr) {
        int cs = e.first;
        for (const auto &e2 : e.second) {
            string prod = e2.first;
            string contx = e2.second;
            vector<string> v = splitSpaces(prod);
            pair<string, string> p = splitSemi(v);
            string uf = p.first, tr = p.second;
            vector<string> v2 = splitSpaces(tr);
            int dotI = 0;
            while (v2[dotI] != middleDot)
                dotI++;
            if (dotI != v2.size() - 1) {
                // for function "move"
                if (v2[dotI + 1][0] != '<') {
                    string sign = v2[dotI + 1];
                    if (dfa.tr.find({cs, sign}) != dfa.tr.end()) {
                        tab.action[cs][sign] =
                            "p" + to_string(dfa.tr[{cs, sign}]);
                    }
                }
            } else {
                vector<string> con = splitSpaces(contx);
                if (v2[dotI - 1] == g.startState && con[0] == endCh &&
                    uf == ss) {
                    tab.action[cs][endCh] = "Acc"; // accepted state in parser
                    continue;
                }
                for (int i = 0; i < con.size(); i++) {
                    string letter = con[i];
                    string prod;
                    for (int j = 0; j < v2.size(); j++) {
                        if (v2[j] == middleDot)
                            continue;
                        if (v2[j] != middleDot) {
                            prod += v2[j];
                        }
                        if (j < v2.size() - 2) {
                            prod += " ";
                        }
                    }
                    if (tab.action.find(cs) != tab.action.end() &&
                        tab.action[cs].find(letter) != tab.action[cs].end()) {
                        cout << prod << endl;
                        if (tab.action[cs][letter][0] == 'p')
                            continue;
                        else {
                            int nr = g.prodMap[{uf, prod}];
                            string sor = tab.action[cs][letter];
                            sor = sor.substr(1);
                            int Or = stoi(sor);
                            cout << "SOR TEST" << sor << endl;
                            if (nr < Or) {
                                tab.action[cs][letter] = "r" + to_string(nr);
                            }
                        }
                    } else {
                        int nr = g.prodMap[{uf, prod}];
                        cout << cs << " " << nr << " " << prod << ";" << endl;
                        tab.action[cs][letter] = "r" + to_string(nr);
                    }
                }
            }
        }
    }
    file << "Productions" << endl;
    for (const auto &it : g.prodMap) {
        // cout << it.first.first << ", " << it.first.second << "; " <<
        // it.second
        //      << endl;
        file << it.first.first << " " << it.first.second << " " << it.second
             << endl;
    }
    //    cout << "ACTION TABLE" << endl;
    file << "Actions" << endl;
    for (const auto &it : tab.action) {
        for (const auto &it2 : it.second) {
            // cout << it.first << ", " << it2.first << " ; " << it2.second
            //      << endl;
            file << it.first << " " << it2.first << " " << it2.second << endl;
        }
    }
    for (const auto &it : dfa.tr) {
        int cs = it.first.first;
        string sign = it.first.second;
        if (sign[0] == '<') {
            if (dfa.tr.find({cs, sign}) != dfa.tr.end()) {
                int ns = dfa.tr[{cs, sign}];
                tab.new_state[cs][sign] = "S" + to_string(ns);
            }
        }
    }
    // cout << "NEW STATE TABLE" << endl;
    file << "NEW STATES" << endl;
    for (const auto &it : tab.new_state) {
        for (const auto &it2 : it.second) {
            // cout << it.first << ", " << it2.first << " ; " << it2.second
            //      << endl;
            file << it.first << " " << it2.first << " " << it2.second << endl;
        }
    }
    file.close();
    return 0;
}
