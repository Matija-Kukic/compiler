#include "automat.h"
#include <iostream>
#include <map>
#include <string>
#include <utility>
Automaton::Automaton() : lex_unit(), actions(), tr(), start("qs"), acc("qa") {};

void Automaton::add_units(string s1, string s2) { lex_unit[s1] = s2; }

void Automaton::add_action(string name, pair<string, string> action) {
    actions.insert({name, action});
}
void Automaton::add_transition(string s1, char z, string s2) {
    pair<string, char> s = make_pair(s1, z);
    tr[s] = s2;
}
void Automaton::print_tr() {
    for (const auto &entry : tr) {
        const auto key = &entry.first;
        const auto value = entry.second;
        cerr << "(" << key->first << ", " << key->second << ") -> " << value
             << endl;
    }
}
