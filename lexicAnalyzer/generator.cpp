#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
using namespace std;

class Automaton {
    multimap<pair<string, char>, string> tr; // prijelazi
    int last;

  public:
    string letter;
    string start, acc;
    Automaton() : tr(), last(0), letter("a"), start("qs"), acc("qa") {}
    int add_state() {
        int t = last;
        last++;
        return t;
    }
    void add_transition(int s1, int s2, char z) {
        string st1 = letter + to_string(s1);
        string st2 = letter + to_string(s2);
        tr.insert({{st1, z}, st2});
    }
    void add_transition2(string s1, string s2, char z) {
        tr.insert({{s1, z}, s2});
    }
    void print_tr() {
        cout << "----Transitions------" << endl;
        for (const auto &entry : tr) {
            const auto &key = entry.first;
            string value = entry.second;
            cout << "(" << key.first << ", " << key.second << ") : " << value
                 << endl;
        }
        cout << "---------------------" << endl;
    }
    void add_letter() { letter[letter.size() - 1] += 1; }
    void res_last() { last = 0; }
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
            v.push_back(exp2);
        }
    }
    if (!v.empty())
        v.push_back(exp.substr(lc, exp.size() - lc));
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
                        j++;
                    }
                    string exp2 = exp.substr(i + 1, j - i - 1);
                    pair<int, int> p = convert_to_states(exp2, A);
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
    map<string, string> regex;
    vector<string> input;
    string exp, line; // expresin/izraz
    while (getline(cin, line)) {
        input.push_back(line);
    }
    exp = input[0];
    cout << "INPUT" << endl;
    for (int i = 0; i < input.size(); i++) {
        cout << input[i] << endl;
    }
    Automaton A;
    for (int i = 0; i < input.size(); i++) {
        pair<int, int> p = convert_to_states(input[i], A);
        pair<string, string> p2 = make_pair(A.letter + to_string(p.first),
                                            A.letter + to_string(p.second));
        cout << p2.first << " " << p2.second << " " << A.start << " " << A.acc
             << endl;
        A.add_transition2(A.start, p2.first, '$');
        A.add_transition2(p2.second, A.acc, '$');
        A.add_letter();
        A.res_last();
    }
    A.print_tr();
    return 0;
}
