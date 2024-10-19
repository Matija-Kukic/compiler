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
        auto range = A.tr.equal_range(make_pair(st, '$'));
        s.pop();
        for (auto it = range.first; it != range.second; ++it) {
            string bb = it->second;
            // cerr << st << " " << bb << endl;
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
                // cerr << "BLAAA " << ss << endl;
            }
        }
    }
    swap(R, Q);
}

int main() {
    /*Automaton a;
    a.add_transition("bla", 'z', "bla2");
    cerr << a.tr[make_pair("bla", 'z')];
    */
    ifstream file("automat.txt");
    string line;
    map<string, Automaton>
        m; // main map with automaton for each state of analyzer
    char c = ' ';
    string state = "";
    string first_state = "";
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
            cerr << state << endl;
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
        // cerr << line << endl;
    }

    /*for (const auto &e : m) {
        const auto &bla = e.first;
        cerr << "STATE: " << bla << endl;
        // m[bla].print_tr();
        for (const auto &e2 : m[bla].lex_unit) {
            cerr << e2.first << " " << e2.second << endl;
        }
        for (const auto &e3 : m[bla].actions) {
            const auto &bla2 = e3.first;
            cerr << bla2 << " " << e3.second.first << " " << e3.second.second
                 << endl;
        }
    }*/

    file.close();
    string content((istreambuf_iterator<char>(cin)),
                   istreambuf_iterator<char>());
    int Start = -1, End = 0, Last = -1;
    string Exp = "";
    int row_cnt = 0;
    string las = "S_pocetno";
    set<string> R, Q, acc;
    stack<string> s;
    Q.insert("qs");
    s.push("qs");
    epsilon_transitions(R, Q, s, m[las], acc);
    R.erase("qs");
    char curr = '\0';
    for (const auto &ent : R) {
        cerr << ent << " ";
    }
    ofstream f2("izlaz.txt");
    content += "/0";
    while (End < content.size()) {
        if (!R.empty() && R.count("qa") == 0) {
            cout << "STANJA(0): " << Start << " " << End << " " << Last << " "
                 << curr << " " << Exp << " " << las << " " << R.size() << endl;
            curr = content[End];
            End += 1;
            // cerr << "TEST0: " << End << " " << curr << " " << R.size() <<
            // endl;
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
            cout << "R: " << endl;
            for (const auto &e : R) {
                cout << e << " ";
            }
            cout << endl;

            // cerr << "TEST: " << Q.size() << " " << s.size() << " " <<
            // R.size()
            //     << endl;
        } else if (!R.empty() && R.count("qa") != 0) {
            cout << "STANJA(1): " << Start << " " << End << " " << Last << " "
                 << curr << " " << Exp << " " << las << " " << R.size() << endl;

            auto it = acc.begin();
            Exp = *it;
            for (auto it = acc.begin(); it != acc.end(); ++it) {
                string s = *it;
                if (m[las].lex_unit.count(s) == 0) {
                    auto range = m[las].actions.equal_range(s);
                    for (auto it = range.first; it != range.second; ++it) {
                        auto const &key = it->first;
                        string act = it->second.first;
                        string param = it->second.second;
                        if (act == "UDJI_U_STANJE") {
                            cerr << "USAO" << endl;
                            las = param;
                        }
                    }
                }
            }
            acc.clear();
            Last = End - 1;
            curr = content[End];
            End = End + 1;
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
            cout << "R: " << endl;
            for (const auto &e : R) {
                cout << e << " ";
            }
            cout << endl;

        } else if (R.empty()) {
            cout << "STANJA(2): " << Start << " " << End << " " << Last << " "
                 << curr << " " << Exp << " " << las << " " << R.size() << " "
                 << endl;

            if (Exp == "") {
                // cerr << "START: " << content[Start] << endl;
                End = Start + 1;
                Start = Start + 1;
                Last = Start + 1;
                R.clear();
                Q.clear();
                Q.insert("qs");
                s.push("qs");
                epsilon_transitions(R, Q, s, m[las], acc);

            } else {
                int z = 0;
                string new_lex = m[las].lex_unit[Exp];
                auto range = m[las].actions.equal_range(Exp);
                for (auto it = range.first; it != range.second; ++it) {
                    auto const &key = it->first;
                    string act = it->second.first;
                    string param = it->second.second;
                    cout << "TESTiranje " << key << " " << act << " " << param
                         << endl;
                    if (act == "UDJI_U_STANJE") {
                        cerr << "USAO" << endl;
                        las = param;
                    }
                    if (act == "VRATI_SE") {
                        int p = stoi(param);
                        Last = Start + p - 1;
                    }
                }
                if (!new_lex.empty()) {
                    f2 << new_lex << endl;
                    cerr << new_lex << endl;
                }
                Exp = "";
                Start = Last + 1;
                End = Last + 1;
                R.clear();
                Q.clear();
                Q.insert("qs");
                s.push("qs");
                epsilon_transitions(R, Q, s, m[las], acc);
                curr = '\0';
            }
        }
    }

    // A.print_tr();
    return 0;
    f2.close();
}
