#include "automat.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <ostream>
#include <set>
#include <stack>
#include <string>
#include <utility>
using namespace std;

void epsilon_transitions(set<string> &R, set<string> &Q, stack<string> &s,
                         Automaton &A, set<string> &acc) {
    while (!s.empty()) {
        string st = s.top();
        auto range = A.tr.equal_range(make_pair(st, '\0'));
        s.pop();
        for (auto it = range.first; it != range.second; ++it) {
            string bb = it->second;
            if (Q.count(bb) == 0) {
                Q.insert(bb);
                s.push(bb);
            }
            if (bb == "qa") {
                string ss = "";
                int i = 0;
                while (!(st[i] >= '0' && st[i] <= '9')) {
                    ss += st[i];
                    i++;
                }
                acc.insert(ss);
            }
        }
    }
    swap(R, Q);
}

int main() {
    ifstream file("automat.txt");
    string line;
    map<string, Automaton>
        m; // main map with automaton for each state of analyzer
    char c = ' ';
    string state = "";
    string first_state = "";
    getline(file, first_state);
    while (getline(file, line)) {
        if (line == "State") {
            c = 's';
            continue;
        }
        if (line == "Transitions") {
            c = 't';
            continue;
        }
        if (line == "Units") {
            c = 'u';
            continue;
        }
        if (line == "Actions") {
            c = 'a';
            continue;
        }
        if (c == 's') {
            state = line;
            continue;
        } else if (c == 't') {
            string s[3] = {"", "", ""};
            int j = 0;
            for (int i = 0; i < line.size(); i++) {
                if (line[i] == ' ') {
                    j++;
                    continue;
                }
                s[j] += line[i];
            }
            char z = s[1][0];
            if (s[1].size() > 1) {
                if (s[1] == "|t")
                    z = '\t';
                if (s[1] == "|n")
                    z = '\n';
                if (s[1] == "|_")
                    z = ' ';
                if (s[1] == "00")
                    z = '\0';
            }
            m[state].add_transition(s[0], z, s[2]);
        } else if (c == 'u') {
            if (line == "---")
                continue;
            string s[2] = {"", ""};
            int j = 0;
            for (int i = 0; i < line.size(); i++) {
                if (line[i] == ' ') {
                    j++;
                    continue;
                }
                s[j] += line[i];
            }
            m[state].add_units(s[0], s[1]);
        } else if (c == 'a') {

            int t = 0;
            int i;
            string letter = "";
            string name = "", parametar = "";
            for (i = 0; line[i] != ' '; i++) {
                letter += line[i];
            }
            i++;
            if (i >= line.size())
                continue;
            for (; i < line.size(); i++) {
                if (line[i] == ' ') {
                    if (name == "NOVI_REDAK") {
                        m[state].add_action(letter, make_pair(name, parametar));
                        name = "";
                        parametar = "";
                        continue;
                    }
                    t++;
                    continue;
                }
                if (t == 0) {
                    name += line[i];
                } else if (t == 1) {
                    parametar += line[i];
                } else {
                    m[state].add_action(letter, make_pair(name, parametar));
                    name = line[i];
                    parametar = "";
                    t = 0;
                }
            }

            if (name != "")
                m[state].add_action(letter, make_pair(name, parametar));
        }
    }

    file.close();
    string content((istreambuf_iterator<char>(cin)),
                   istreambuf_iterator<char>());
    int Start = 0, End = 0, Last = 0;
    string Exp = "";
    int row_cnt = 0;
    int no_line = 1, nl = 0;
    string las = first_state;
    set<string> R, Q, acc;
    stack<string> s;
    Q.insert("qs");
    s.push("qs");
    epsilon_transitions(R, Q, s, m[las], acc);
    R.erase("qs");
    char curr = '\0';
    content += "\u200B";
    cerr << las << endl;
    while (End < content.size()) {
        if (!R.empty() && R.count("qa") == 0) {
            curr = content[End];
            End += 1;
            for (const auto &e : R) {
                string st = e;
                auto range = m[las].tr.equal_range({e, curr});
                for (auto it = range.first; it != range.second; ++it) {
                    Q.insert(it->second);
                    s.push(it->second);
                }
            }
            R.clear();
            epsilon_transitions(R, Q, s, m[las], acc);
            Q.clear();
        } else if (!R.empty() && R.count("qa") != 0) {

            auto it = acc.begin();
            Exp = *it;
            acc.clear();
            Last = End - 1;
            curr = content[End];
            End = End + 1;
            Q.clear();
            for (const auto &e : R) {
                string st = e;
                auto range = m[las].tr.equal_range({e, curr});
                for (auto it = range.first; it != range.second; ++it) {
                    Q.insert(it->second);
                    s.push(it->second);
                }
            }
            R.clear();
            epsilon_transitions(R, Q, s, m[las], acc);
            Q.clear();
        } else if (R.empty()) {

            if (Exp == "") {
                auto range = m[las].actions.equal_range(Exp);
                for (auto it = range.first; it != range.second; ++it) {
                    auto const &key = it->first;
                    string act = it->second.first;
                    string param = it->second.second;
                    if (act == "NOVI_REDAK") {
                        no_line++;
                    }
                }
                cerr << curr << endl;
                End = Start + 1;
                Last = Start;
                Start = Start + 1;
                R.clear();
                Q.clear();
                Q.insert("qs");
                s.push("qs");
                epsilon_transitions(R, Q, s, m[las], acc);
                Q.clear();

            } else {
                int z = 0;
                string new_lex = m[las].lex_unit[Exp];
                auto range = m[las].actions.equal_range(Exp);
                for (auto it = range.first; it != range.second; ++it) {
                    auto const &key = it->first;
                    string act = it->second.first;
                    string param = it->second.second;
                    if (act == "UDJI_U_STANJE") {
                        las = param;
                    }
                    if (act == "VRATI_SE") {
                        int p = stoi(param);
                        Last = Start + p - 1;
                    }
                    if (act == "NOVI_REDAK") {
                        nl++;
                    }
                }
                if (!new_lex.empty()) {
                    cout << new_lex << " " << no_line << " "
                         << content.substr(Start, Last - Start + 1) << endl;
                }
                no_line += nl;
                nl = 0;
                Exp = "";
                Start = Last + 1;
                End = Last + 1;
                R.clear();
                Q.clear();
                Q.insert("qs");
                s.push("qs");
                epsilon_transitions(R, Q, s, m[las], acc);
                Q.clear();
                curr = '\0';
            }
        }
    }

    // A.print_tr();
    return 0;
}
