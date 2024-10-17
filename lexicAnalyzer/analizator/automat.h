#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <map>
#include <string>
#include <utility>
using namespace std;

struct Automaton {
    map<string, string> lex_unit;
    multimap<string, pair<string, string>> actions;
    map<pair<string, char>, string> tr;
    string start, acc;

    Automaton();
    void add_transition(string s1, char z, string s2);
    void add_action(string name, pair<string, string> action);
    void add_units(string s1, string s2);
    void print_tr();
};

#endif // AUTOMATON_H
