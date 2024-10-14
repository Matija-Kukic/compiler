#include <iostream>
#include <string>
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

  public:
    Automaton() : tr(), states() {}
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
        for (int i = 0; i < states.size(); i++) {
            cout << states[i] << " ";
        }
        cout << endl;
        return;
    }
    void add_transition(int s1, int s2, char z) {
        transition t(s1, s2, z);
        tr.push_back(t);
    }
    void print_tr() {
        for (int i = 0; i < tr.size(); i++) {
            cout << tr[i].s1 << " " << tr[i].s2 << " " << tr[i].z << endl;
        }
    }
};

bool is_operator(string exp, int i) {
    int br = 0;
    while (i - 1 >= 0 && exp[i - 1] == '\\') {
        br++;
        --i;
    }
    return br % 2;
}

pair<int, int> convert_to_states(string exp, Automaton &A) {
    bool prefix = false;
    int a, b;
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
                    A.add_transition(a, b,
                                     '$'); // ovo se koristi za epislon prijelaz
                }
                A.add_transition(a, b, exp[i]);
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
    }
    return make_pair(a, b);
}

int main() {
    string exp; // expresin/izraz
    getline(cin, exp);
    cout << exp << endl;
    Automaton A;
    convert_to_states(exp, A);
    A.print_states();
    A.print_tr();
    return 0;
}
