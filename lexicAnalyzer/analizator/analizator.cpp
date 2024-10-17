#include "automat.h"
#include <fstream>
#include <iostream>
#include <map>
#include <string>

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
            string s[3];
            int j = 0;
            for (int i = 0; i < line.size(); i++) {
            }
        }
        // cerr << line << endl;
    }
    return 0;
}
