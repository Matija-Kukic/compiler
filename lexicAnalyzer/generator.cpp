#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

struct transition {
    // s1 - pocetno stanje, s2 - stanje u koje prijelazimo
    // z - znak prijelaza
    int s1;
    int s2;
    char z;
    transition(int s1, int s2, int z) : s1(s1), s2(s2), z(z) {}
};

class Automaton {
    vector<transition> tr; // prijelazi
    vector<int> states;
    int start, acc;

  public:
    Automaton() : tr(), states(), start(0), acc(0) {}
    int add_state() {
        if (!states.empty()) {
            states.push_back(states.size());
            return states.size() - 1;
        } else {
            states.push_back(0);
            return 0;
        }
    }
    void print_states() {
        cout << "-------States-------" << endl;
        cout << "Start: " << start << " Acc: " << acc << endl;
        for (int i = 0; i < states.size(); i++) {
            cout << states[i] << " ";
        }
        cout << "--------------------" << endl;
        return;
    }
    void add_transition(int s1, int s2, char z) {
        transition t(s1, s2, z);
        tr.push_back(t);
    }
    void print_tr() {
        cout << "----Transitions------" << endl;
        for (int i = 0; i < tr.size(); i++) {
            cout << tr[i].s1 << " " << tr[i].s2 << " " << tr[i].z << endl;
        }
        cout << "---------------------" << endl;
    }
    void add_start(int s) { start = s; }
    void add_acc(int s) { acc = s; }
};

bool is_operator(string exp, int i) {
    int br = 0;
    while (i - 1 >= 0 && exp[i - 1] == '\\') {
        br++;
        --i;
    }
    return br % 2 == 0;
}

pair<int, int> convert_to_states(string exp, Automaton &A) {
    vector<string> v;
    int n_braces = 0;
    int lc = 0; // zadnji zapisani znak |
    for (int i = 0; i < exp.size(); i++) {
        if (exp[i] == '(' && is_operator(exp, i))
            n_braces++;
        else if (exp[i] == ')' && is_operator(exp, i))
            --n_braces;
        else if (exp[i] == '|' && n_braces == 0 && is_operator(exp, i)) {
            string exp2 = exp.substr(lc, i - lc);
            lc = i + 1;
            cout << "TEST: " << exp2 << endl;
            v.push_back(exp2);
        }
    }
    if (!v.empty())
        v.push_back(exp.substr(lc, exp.size() - lc));
    for (int i = 0; i < v.size(); i++) {
        cout << v[i] << " ";
    }
    cout << endl;
    // return make_pair(0, 0);
    int left_state = A.add_state();
    int right_state = A.add_state();
    if (!v.empty()) {
        for (int i = 0; i < v.size(); i++) {
            pair<int, int> p = convert_to_states(v[i], A);
            A.add_transition(left_state, p.first, '$');
            A.add_transition(p.second, right_state, '$');
        }
    } else {
        bool prefix = false;
        int a, b;
        int last_state = left_state;
        for (int i = 0; i < exp.size(); i++) {
            if (prefix == true) {
                prefix = false;
                char tz; // prijelazni znak
                if (exp[i] == 't') {
                    tz = '\t';
                } else if (exp[i] == 'n') {
                    tz = '\n';
                } else if (exp[i] == ' ') {
                    tz = ' ';
                } else {
                    tz = exp[i];
                }
                a = A.add_state();
                b = A.add_state();
                A.add_transition(a, b, tz);
            } else {
                if (exp[i] == '\\') {
                    prefix = true;
                    continue;
                }
                if (exp[i] != '(') {
                    a = A.add_state();
                    b = A.add_state();
                    if (exp[i] == '$') {
                        A.add_transition(
                            a, b,
                            '$'); // ovo se koristi za epislon prijelaz
                    } else {
                        A.add_transition(a, b, exp[i]);
                    }
                } else {
                    int j = i;
                    while (exp[j] != ')') {
                        // cout << "bla " << j << " " << exp[j] << endl;
                        j++;
                    }
                    string exp2 = exp.substr(i + 1, j - i - 1);
                    pair<int, int> p = convert_to_states(exp2, A);
                    cout << exp2 << " " << j << endl;
                    a = p.first;
                    b = p.second;
                    i = j;
                }
            }
            if (i + 1 < exp.size() && exp[i + 1] == '*') {
                int x = a;
                int y = b;
                a = A.add_state();
                b = A.add_state();
                A.add_transition(a, x, '$');
                A.add_transition(y, b, '$');
                A.add_transition(a, b, '$');
                A.add_transition(y, x, '$');
                i = i + 1;
            }
            A.add_transition(last_state, a, '$');
            last_state = b;
        }
        A.add_transition(last_state, right_state, '$');
    }
    return make_pair(left_state, right_state);
}

int main() {
    string exp; // expresin/izraz
    getline(cin, exp);
    cout << exp << endl;
    Automaton A;
    pair<int, int> p = convert_to_states(exp, A);
    A.add_start(p.first);
    A.add_acc(p.second);
    A.print_states();
    A.print_tr();
    return 0;
}
