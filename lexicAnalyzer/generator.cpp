#include <fstream>
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
    map<string, string> actions;
    map<string, string> lex_unit;
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

    string ret_tr() {
        string ret = "";
        for (const auto &entry : tr) {
            const auto &key = entry.first;
            string value = entry.second;
            string z = "";
            if (key.second == '\t') {
                z += "|t";
            } else if (key.second == '\n') {
                z += "|n";
            } else if (key.second == ' ') {
                z += "|_";
            } else if (key.second == '\0') {
                z += "00";
            } else {
                z += key.second;
            }
            ret += key.first + " " + z + " " + value + "\n";
        }
        return ret;
    }
    void add_letter() {
        int N = letter.size() - 1;
        if (letter[N] == 'z') {
            letter[N] = '~';
            letter += "`";
            N++;
        }
        letter[N] += 1;
    }
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
            A.add_transition(left_state, p.first, '\0');
            A.add_transition(p.second, right_state, '\0');
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
                } else if (exp[i] == '_') {
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
                            '\0'); // ovo se koristi za epislon prijelaz
                    } else {
                        A.add_transition(a, b, exp[i]);
                    }
                } else {
                    int j = i + 1;
                    int cnt = 0;
                    while (
                        !(exp[j] == ')' && cnt == 0 && is_operator(exp, j))) {
                        if (exp[j] == '(' && is_operator(exp, j))
                            cnt++;
                        if (exp[j] == ')' && is_operator(exp, j))
                            --cnt;
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
                A.add_transition(a, x, '\0');
                A.add_transition(y, b, '\0');
                A.add_transition(a, b, '\0');
                A.add_transition(y, x, '\0');
                i = i + 1;
            }
            A.add_transition(last_state, a, '\0');
            last_state = b;
        }
        A.add_transition(last_state, right_state, '\0');
    }
    return make_pair(left_state, right_state);
}

int main() {
    map<string, string> regex;
    vector<string> input;
    map<string, Automaton>
        aA; // all automants, i use one for each state of analyzer
    string exp, lines = ""; // expresin/izraz
    int N = 0;
    while (getline(cin, lines))
        input.push_back(lines);
    for (int j = 0; input[j][0] == '{'; j++) {
        string line = input[j];
        string regname = "", reg = "", hreg = "";
        int z = 0, z2 = 0;
        char c1 = '{', c2 = '}';
        for (int i = 0; i < line.size(); i++) {
            if (line[i] == ' ') {
                z++;
                continue;
            }
            if (z == 0) {
                regname += line[i];
            } else {
                if (line[i] == c1 && is_operator(line, i)) {
                    z2++;
                }
                if (line[i] == c2 && is_operator(line, i)) {
                    hreg += c2;
                    reg += "(" + regex[hreg] + ")";
                    hreg = "";
                    --z2;
                    continue;
                }
                if (!z2)
                    reg += line[i];
                else
                    hreg += line[i];
            }
        }
        regex[regname] = reg;
        N++;
    }
    string states = input[N].substr(3, input[N].size() - 2);
    string help = "";
    for (int i = 0; i < states.size(); i++) {
        if (states[i] != ' ') {
            help += states[i];
        } else {
            aA[help] = Automaton();
            help = "";
        }
    }
    Automaton a = Automaton();
    aA[help] = a;
    N += 2;

    int z = 0, fs = 1;
    for (int i = N; i < input.size(); i++) {

        string current_state = "", exp = "", new_state = "", lex_unit = "";
        vector<string> actions;
        for (int j = 0; j < input[i].size(); j++) {
            if (input[i][j] == '<' && fs) {
                z++;
                continue;
            }
            if (input[i][j] == '>' && fs) {
                fs = 0;
                z = 0;
                continue;
            }
            if (z == 1) {
                current_state += input[i][j];
            } else {
                if (input[i][j] == '{' && is_operator(input[i], j)) {
                    int c = j + 1;
                    while (input[i][c] != '}') {
                        c++;
                    }
                    string helpe = input[i].substr(j, c - j + 1);
                    exp += "(" + regex[helpe] + ")";
                    j = c;
                } else {
                    exp += input[i][j];
                }
            }
        }

        fs = 1;
        i += 2;
        if (input[i][0] != '-') {
            lex_unit = input[i];
        }
        i++;
        while (input[i][0] != '}') {
            actions.push_back(input[i]);
            i++;
        }
        pair<int, int> p = convert_to_states(exp, aA[current_state]);
        string left = aA[current_state].letter + to_string(p.first);
        string right = aA[current_state].letter + to_string(p.second);
        aA[current_state].add_transition2("qs", left, '\0');
        aA[current_state].add_transition2(right, "qa", '\0');
        string lett = aA[current_state].letter;
        if (!actions.empty()) {
            for (int c = 0; c < actions.size(); c++) {
                aA[current_state].actions[lett] += actions[c] + " ";
            }
        }
        if (lex_unit != "") {
            aA[current_state].lex_unit[aA[current_state].letter] = lex_unit;
        }
        aA[current_state].res_last();
        aA[current_state].add_letter();
    }

    ofstream file("analizator/automat.txt");

    for (const auto &e : aA) {
        const auto &key = e.first;
        file << "State\n";
        file << key + "\n";
        file << "Transitions\n";
        // aA[key].print_tr();
        file << aA[key].ret_tr();
        file << "Units\n";
        for (const auto &e2 : aA[key].lex_unit) {
            const auto &k2 = e2.first;
            file << k2 << " " << aA[key].lex_unit[k2] << "\n";
        }
        if (aA[key].lex_unit.empty()) {
            file << "---\n";
        }
        file << "Actions\n";
        for (const auto &e3 : aA[key].actions) {
            const auto &key = e3.first;
            const auto value = e3.second;
            file << key << " " << value << "\n";
        }
    }
    file.close(); // Always close the file
    Automaton A;
    string exp3 = "";
    exp3 += "'(" + regex["{sveOsimJednostrukogNavodnikaNovogRedaITaba}"] + ")'";
    pair<int, int> p = convert_to_states(exp3, A);
    return 0;
}
