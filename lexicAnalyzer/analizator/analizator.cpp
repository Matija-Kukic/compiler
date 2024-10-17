#include "automat.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <utility>
using namespace std;

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
            cerr << i << " " << letter << endl;
            i++;
            if (i >= line.size())
                continue;
            for (; i < line.size(); i++) {
                if (line[i] == ' ') {
                    if (name == "NOVI_RED") {
                        m[state].add_action(letter, make_pair(name, parametar));
                        name = "";
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
                    name = "";
                    parametar = "";
                    t = 0;
                }
            }

            if (name != "")
                m[state].add_action(letter, make_pair(name, parametar));
        }
        // cerr << line << endl;
    }

    for (const auto &e : m) {
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
    }

    file.close();
    string content((istreambuf_iterator<char>(cin)),
                   istreambuf_iterator<char>());
    cout << content;
    return 0;
}
