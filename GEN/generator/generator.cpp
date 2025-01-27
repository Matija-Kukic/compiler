#include "structs.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
using namespace std;

ofstream file("a.frisc");
shared_ptr<treeNode> root;
shared_ptr<treeNode> currScope;
shared_ptr<Tree> troot;

// generator structures
string toCall = "";
string curr_global = "";
int last_global = 0;
int gind = -1;
unordered_map<string, int> globals;
map<int, int> global_ints;
unordered_map<int, string> consts;
int last_const = 0;
int pref = 0;
int ifs = 0, ops = 0, fors = 0;
int currIn = 0;
int assign = 0;
bool in_idx = false;
string av = "";
vector<string> input;
vector<string> splitSpaces(string ent) {
    vector<string> v;
    string p = "";
    for (int i = 0; i < ent.size(); i++) {
        if (ent[i] != ' ') {
            p += ent[i];
        } else {
            if (p != "")
                v.push_back(p);
            p = "";
        }
    }
    if (p != "") {
        v.push_back(p);
    }
    return v;
}

shared_ptr<Tree> build(vector<string> &input) {
    shared_ptr<Tree> root = nullptr;
    stack<pair<shared_ptr<Tree>, int>> stack;

    for (const auto &line : input) {
        int depth = 0;
        while (depth < line.size() && line[depth] == ' ')
            depth++;

        string node = line.substr(depth);

        shared_ptr<Tree> newNode = make_shared<Tree>(node);

        while (!stack.empty() && stack.top().second >= depth) {
            stack.pop();
        }
        if (stack.empty()) {
            root = newNode;
        } else {
            stack.top().first->children.push_back(newNode);
            newNode->parent = stack.top().first;
        }
        stack.push({newNode, depth});
    }
    return root;
}
void destroyTreeNode(shared_ptr<treeNode> &node) {
    if (node) {
        // Recursively destroy children first
        for (auto &child : node->children) {
            destroyTreeNode(child);
        }
        // Reset parent and children to avoid circular references
        node->children.clear();
        node->parent.reset(); // Reset parent reference
        node.reset();         // Reset the current node
    }
}

// Function to recursively destroy the Tree hierarchy
void destroyTree(shared_ptr<Tree> &node) {
    if (node) {
        // Recursively destroy children first
        for (auto &child : node->children) {
            destroyTree(child);
        }
        // Reset parent and children to avoid circular references
        node->children.clear();
        node->parent.reset(); // Reset parent reference
        node.reset();         // Reset the current node
    }
}
int convertEscapeSequenceToInt(string str) {
    if (str == "\\n") {
        return '\n';
    } else if (str == "\\t") {
        return '\t';
    } else if (str == "\\'") {
        return '\'';
    } else if (str == "\\\"") {
        return '\"';
    } else if (str == "\\\\") {
        return '\\';
    } else {
        // If it's j
        return str[0];
    }
}
void printTree(const shared_ptr<Tree> &node, int depth) {
    if (!node)
        return;
    cout << string(depth, ' ') << node->node << endl;
    for (const auto &child : node->children) {
        printTree(child, depth + 1);
    }
}
bool findNode(const shared_ptr<Tree> &node, string s) {
    bool z = 0;
    const vector<shared_ptr<Tree>> &u = node->children;
    if (u.size() > 0) {
        for (int i = 0; i < u.size(); i++) {
            if (findNode(u[i], s)) {
                z = 1;
            }
        }
    }
    vector<string> ulaz = splitSpaces(node->node);
    if (ulaz[0] == s) {
        return true;
    }
    return z;
}
string findIDN(const shared_ptr<Tree> &node, string s) {
    string z = "";
    const vector<shared_ptr<Tree>> &u = node->children;
    if (u.size() > 0) {
        for (int i = 0; i < u.size(); i++) {
            string ss = findIDN(u[i], s);
            if (ss != "")
                z = ss;
        }
    }
    vector<string> ulaz = splitSpaces(node->node);
    if (ulaz[0] == s) {
        return ulaz[2];
    }
    return z;
}
bool checkImp(type t1, type t2) {
    if (t1 == kint) {
        if (t2 == Int)
            return true;
    } else if (t1 == kchar) {
        if (t2 == Int || t2 == Char || t2 == kint)
            return true;
    } else if (t1 == Char) {
        if (t2 == Int || t2 == kint) {
            return true;
        } else if (t2 == kchar) {
            return true;
        }
    } else if (t1 == Int) {
        if (t2 == kint)
            return true;
    } else if (t1 == arrint) {
        if (t2 == arrkint)
            return true;
    } else if (t1 == arrchar) {
        if (t2 == arrkchar)
            return true;
    }
    return t1 == t2;
}
bool checkChar(string znak) {
    // ovo treba dovrsiti
    if (znak.size() == 3)
        return true;
    if (znak.size() == 4) {
        string md = znak.substr(1, 2);
        if (md == "\\t" || md == "\\n" || md == "\\0" || md == "\\\'" ||
            md == "\\\"" || md == "\\\\") {
            return true;
        }
    }
    return false;
}
int checkString(string niz) {
    int z = 0;
    for (int i = 1; i < niz.size() - 1; i++) {
        if (niz[i] == '\\') {
            if (niz[i + 1] != 't' || niz[i + 1] != 'n' || niz[i + 1] != '0' ||
                niz[i + 1] != '\\' || niz[i + 1] != '\"' ||
                niz[i + 1] != '\'') {
                return -1;
            } else {
                i++;
                z++;
            }
        } else {
            z++;
        }
    }
    return z;
}
void prodErr(const shared_ptr<Tree> &node) {
    cout << node->node << " ::=";
    for (const auto &child : node->children) {
        vector<string> out = splitSpaces(child->node);
        cout << " ";
        if (out.size() == 1) {
            cout << out[0];
        } else if (out.size() > 1) {
            cout << out[0] << "(" << out[1] << "," << out[2] << ")";
        }
    }
    cout << endl;
    currScope.reset();
    destroyTree(troot);
    destroyTreeNode(root);
    exit(0);
}
Type IZRAZ(const shared_ptr<Tree> &node);
vector<type> LISTA_ARGUMENATA(const shared_ptr<Tree> &node);
Type CAST_IZRAZ(const shared_ptr<Tree> &node);
type IME_TIPA(const shared_ptr<Tree> &node);
type SPECIFIKATOR_TIPA(const shared_ptr<Tree> &node);
Type IZRAZ_PRIDRUZIVANJA(const shared_ptr<Tree> &node);

void errPrint(string message) {
    string redS = "\033[31m", redE = "\033[0m";
    cout << redS << message << redE << endl;
    currScope.reset();
    destroyTree(troot);
    destroyTreeNode(root);
    exit(0);
}

Type PRIMARNI_IZRAZ(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);
    if (v[0] == "IDN") {
        shared_ptr<treeNode> ptr = currScope;
        string name = v[2];
        bool declared = false;
        Type t = Type(Error, false);
        while (ptr != nullptr) {
            map<string, Type> &m = ptr->table;
            if (m.find(name) != m.end()) {
                declared = true;
                t = m[name];
                break;
            } else {
                ptr = ptr->parent;
            }
        }
        if (t.type == Int || t.type == kchar || t.type == Char ||
            t.type == kint || t.type == Func) {
            if (t.type == Func) {
                toCall = name;
            } else if (assign) {
                // cout << "TU SAM NIGA I SMANJUJEM " << endl;
                av = name;
                assign--;

            } else if (ptr != root && ptr != nullptr) {
                const unordered_map<string, int> &m = ptr->offsets;
                const unordered_map<string, int> &m2 = ptr->loc_off;
                // int llo = currScope->loc_last_off;
                // int diff = currScope->all_push - llo;
                if (m.find(name) != m.end()) {
                    // int val = 4 * (ptr->last_off - m.at(name) +
                    //                currScope->loc_last_off + diff);
                    int val = 4 * (currScope->all_push - m.at(name));
                    file << "   LOAD R0, (R7+" << 0 << hex << val << dec << ")"
                         << endl;
                    file << "   PUSH R0" << endl;
                    currScope->all_push++;
                } else if (m2.find(name) != m2.end()) {
                    int val = 4 * (currScope->all_push - m2.at(name));
                    file << "   LOAD R0, (R7+" << 0 << hex << val << dec << ")"
                         << endl;
                    file << "   PUSH R0" << endl;
                    currScope->all_push++;
                }
            } else if (globals.find(name) != globals.end()) {
                file << "   LOAD R0, (" << hex << globals[name] * 4 + 4096
                     << dec << ")" << endl;
                file << "   PUSH R0" << endl;
                currScope->all_push++;
            }
        } else {
            if (assign) {
                av = name;
            }
        }
        ptr.reset();
        if (declared) {
            return t;
        } else {
            prodErr(node);
        }
    } else if (v[0] == "BROJ") {
        try {
            long long vr = stoll(v[2], nullptr, 0);
        } catch (const out_of_range &e) {
            prodErr(node);
        }
        long long vr = stoll(v[2], nullptr, 0);
        bool minus = false;
        shared_ptr<Tree> ptr = node;
        while (ptr != nullptr && ptr->node != "<multiplikativni_izraz>")
            ptr = ptr->parent;
        if (findNode(ptr, "MINUS")) {
            if (vr > 2147483648)
                prodErr(node);
            minus = true;
        } else {
            if (vr > 2147483647) {
                prodErr(node);
            }
        }
        shared_ptr<Tree> ptr2 = node;
        if (minus)
            vr *= -1;
        while (ptr2 != nullptr && ptr2->node != "<definicija_funkcije>")
            ptr2 = ptr2->parent;

        if (ptr2 == nullptr) {
            if (gind == -1) {
                global_ints.insert({globals[curr_global], vr});
            } else {
                global_ints.insert({globals[curr_global] + gind, vr});
                gind++;
            }
        } else {
            if (consts.find(vr) == consts.end()) {
                string cname = "C_" + to_string(last_const);
                consts.insert({vr, cname});
                last_const += 1;
            }
            file << "   LOAD R0, (" << consts[vr] << ")" << endl;
            file << "   PUSH R0" << endl;
            currScope->all_push++;
        }

        return Type(Int, false);
    } else if (v[0] == "ZNAK") {
        string znak = v[2];
        if (!checkChar(znak))
            prodErr(node);
        int vr = convertEscapeSequenceToInt(znak.substr(1, znak.length() - 2));
        shared_ptr<Tree> ptr2 = node;
        while (ptr2 != nullptr && ptr2->node != "<definicija_funkcije>")
            ptr2 = ptr2->parent;
        if (ptr2 == nullptr) {
            if (gind == -1) {
                global_ints.insert({globals[curr_global], vr});
            } else {
                global_ints.insert({globals[curr_global] + gind, vr});
                gind++;
            }
        } else {
            if (consts.find(vr) == consts.end()) {
                string cname = "C_" + to_string(last_const);
                consts.insert({vr, cname});
                last_const += 1;
            }
            file << "   LOAD R0, (" << consts[vr] << ")" << endl;
            file << "   PUSH R0" << endl;
            currScope->all_push++;
        }
        return Type(Char, false);
    } else if (v[0] == "NIZ_ZNAKOVA") {
        int z = checkString(v[2]);
        if (z == -1)
            prodErr(node);
        return Type(arrkchar, z, false);
    } else if (v[0] == "L_ZAGRADA") {
        // kasnije dovrsi ova produkcija je
        // L_ZAGRADA <izraz> D_ZAGRADA
        curr++;
        ulaz = u[curr]->node;
        Type ret = Type(Error, false);
        if (ulaz == "<izraz>") {
            ret = IZRAZ(u[curr]);
        } else {
            errPrint("PRIMARNI_IZRAZ GRESKA KRIVI NIZ ZNAKOVA GRAMATIKE");
        }
        curr++;
        ulaz = u[curr]->node;
        vector<string> v2 = splitSpaces(ulaz);
        if (v2[0] != "D_ZAGRADA") {
            errPrint("PRIMARNI_IZRAZ GRESKA KRIVI NIZ ZNAKOVA GRAMATIKE");
        }
        return ret;
    } else {
        errPrint("PRIMARNI_IZRAZ GRESKA KRIVI NIZ ZNAKOVA GRAMATIKE");
    }
    return Type(Error, false);
}

Type POSTFIKS_IZRAZ(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<primarni_izraz>") {
        Type t = PRIMARNI_IZRAZ(u[curr]);
        return t;
    } else if (ulaz == "<postfiks_izraz>") {

        Type t = POSTFIKS_IZRAZ(u[curr]);
        curr++;
        ulaz = u[curr]->node;
        vector v = splitSpaces(ulaz);
        if (v[0] == "L_UGL_ZAGRADA") {
            shared_ptr<treeNode> ptr = currScope;
            string name = findIDN(u[0], "IDN");
            while (ptr != root) {
                const map<string, Type> &m = ptr->table;
                if (m.find(name) != m.end())
                    break;
                ptr = ptr->parent;
            }
            if (ptr == root) {
                in_idx = true;
            }
            if (t.type == arrint || t.type == arrchar || t.type == arrkint ||
                t.type == arrkchar) {
                curr++;
                ulaz = u[curr]->node;

                if (ulaz == "<izraz>") {
                    Type t2 = IZRAZ(u[curr]);
                    if (t2.type == Int || t2.type == Char || t2.type == kchar ||
                        t2.type == kint) {
                        curr++;
                        ulaz = u[curr]->node;
                        vector v2 = splitSpaces(ulaz);
                        if (v2[0] == "D_UGL_ZAGRADA") {
                            if (!assign) {
                                in_idx = false;
                                if (ptr == root) {
                                    file << "   POP R0" << endl;
                                    file << "   SHL R0, 2, R0" << endl;
                                    file << "   MOVE " << hex
                                         << 4096 + globals[name] * 4 << ", R1"

                                         << endl;
                                    file << "   ADD R1, R0, R1" << endl;
                                    file << "   LOAD R0,(R1)" << endl;
                                    file << "   PUSH R0" << endl;
                                    // currScope->all_push++;
                                } else {

                                    const unordered_map<string, int> &m =
                                        ptr->offsets;
                                    const unordered_map<string, int> &m2 =
                                        ptr->loc_off;

                                    if (m.find(name) != m.end()) {
                                        // currScope->all_push--;
                                        int val =
                                            4 * (ptr->all_push - m.at(name));
                                        file << "   POP R1" << endl;
                                        file << "   SHL R1, 2, R1" << endl;
                                        file << "   MOVE " << hex << val
                                             << ", R2"

                                             << endl;
                                        file << "   ADD R2, R1, R2" << endl;
                                        file << "   ADD R2, R7, R2" << endl;
                                        file << "   LOAD R0,(R2)" << endl;
                                        file << "   PUSH R0,(R2)" << endl;
                                        // currScope->all_push -= 2;
                                    } else if (m2.find(name) != m2.end()) {
                                        // currScope->all_push--;
                                        int val = (4 * (currScope->all_push -
                                                        m2.at(name)));
                                        file << "   POP R1" << endl;
                                        file << "   SHL R1, 2, R1" << endl;
                                        file << "   MOVE " << hex << val
                                             << ", R2"

                                             << endl;
                                        file << "   ADD R2, R1, R2" << endl;
                                        file << "   ADD R2, R7, R2" << endl;
                                        file << "   LOAD R0,(R2)" << endl;
                                        file << "   PUSH R0" << endl;
                                        // currScope->all_push -= 2;
                                    }
                                }
                            }
                            ptr.reset();
                            if (t.type == arrint) {
                                return Type(Int, true);
                            } else if (t.type == arrchar) {
                                return Type(Char, true);
                            } else if (t.type == arrkchar) {
                                return Type(kchar, false);
                            } else if (t.type == arrkint) {
                                return Type(kint, false);
                            } else {
                                prodErr(node);
                            }
                        } else {
                            errPrint(
                                "GRESKA U ULAZNIM ZNAKOVIMA POSTFIKS_IZRAZ 1");
                        }
                    } else {
                        prodErr(node);
                    }

                } else {
                    errPrint("GRESKA U ULAZNIM ZNAKOVIMA POSTFIKS_IZRAZ 2");
                }
            } else {
                prodErr(node);
            }

        } else if (v[0] == "L_ZAGRADA") {
            curr++;
            ulaz = u[curr]->node;
            vector v2 = splitSpaces(ulaz);
            if (v2[0] == "D_ZAGRADA") {
                if (t.type == Func) {
                    file << "   CALL f_" + toCall << endl;
                    if (t.retyp != Void) {
                        file << "   PUSH R6" << endl;
                    }
                    currScope->all_push++;
                    toCall = "";
                }
                if (t.type == Func && t.fparam.size() == 0) {
                    cerr << "DEBUG POSTFIKS_IZRAZ" << t.fparam.size();
                    return Type(t.retyp, false);
                } else {
                    prodErr(node);
                }
            } else if (v2[0] == "<lista_argumenata>") {
                vector<type> params = LISTA_ARGUMENATA(u[curr]);
                int z = 0;
                if (params.size() == t.fparam.size() && params.size() != 0 &&
                    t.type == Func) {
                    for (int i = 0; i < params.size(); i++) {
                        if (params[i] == t.fparam[i] ||
                            checkImp(params[i], t.fparam[i])) {
                            z++;
                        }
                        cerr << "PARAM CHECK" << params[i] << " " << t.fparam[i]
                             << endl;
                    }
                }
                if (t.type == Func) {
                    file << "   CALL f_" + toCall << endl;
                    int vr = 0;
                    for (int i = 0; i < t.fparam.size(); i++) {
                        if (t.fparam[i] == Int || t.fparam[i] == Char ||
                            t.fparam[i] == kint || t.fparam[i] == kchar)
                            vr++;
                    }
                    file << "   ADD R7, " << 0 << hex << vr * 4 << dec << ", R7"
                         << endl;

                    currScope->all_push -= t.fparam.size();
                    if (t.retyp != Void) {
                        file << "   PUSH R6" << endl;
                    }
                    currScope->all_push++;
                    toCall = "";
                }
                if (z == params.size()) {
                    return Type(t.retyp, false);
                } else {
                    cerr << z << " " << params.size() << " " << t.fparam.size()
                         << endl;
                    prodErr(node);
                }
            }
        } else if (v[0] == "OP_INC" || v[0] == "OP_DEC") {
            if (t.lexp == 1 && checkImp(t.type, Int)) {
                return Type(Int, false);
            } else {
                prodErr(node);
            }
        } else {
            errPrint("GRESKA U ULAZNIM ZNAKOVIMA POSTFIKS_IZRAZ 3");
        }
    } else {
        errPrint("KRIVI ULAZ ZNAKOVA GRAMATIKE POSTFIKS_IZRAZ 4");
    }
    return Type(Error, false);
}
vector<type> LISTA_ARGUMENATA(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<izraz_pridruzivanja>") {
        vector<type> v;
        v.push_back(IZRAZ_PRIDRUZIVANJA(u[curr]).type);
        return v;
    } else if (ulaz == "<lista_argumenata>") {
        vector<type> v = LISTA_ARGUMENATA(u[curr]);
        curr++;
        ulaz = u[curr]->node;
        vector<string> uu = splitSpaces(ulaz);
        if (uu[0] != "ZAREZ") {
            errPrint("KRIVI ULAZ ZNAKOVA GRAMATIKE 1 LISTA ARG");
        }
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<izraz_pridruzivanja>") {
            type t = IZRAZ_PRIDRUZIVANJA(u[curr]).type;
            v.push_back(t);
            return v;
        } else {
            errPrint("KRIVI ULAZ ZNAKOVA GRAMATIKE 2 LISTA ARG");
        }
    } else {
        errPrint("KRIVI ULAZ ZNAKOVA GRAMATIKE 3 LISTA ARG");
    }
    vector<type> v2;
    return v2;
}

Type UNARNI_IZRAZ(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);
    if (v[0] == "<postfiks_izraz>") {
        Type t = POSTFIKS_IZRAZ(u[curr]);
        return t;
    } else if (v[0] == "OP_INC" || v[0] == "OP_DEC") {
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<unarni_izraz>") {
            int a = -1 ? 1 : v[0] == "OP_INC";
            string name = findIDN(u[curr], "IDN");
            cout << "NAME TEST " << name << endl;
            shared_ptr<treeNode> ptr = currScope;
            while (ptr != root && ptr->table.find(name) == ptr->table.end())
                ptr = ptr->parent;
            const unordered_map<string, int> &m = ptr->offsets;
            const unordered_map<string, int> &m2 = ptr->loc_off;
            // int llo = currScope->loc_last_off;
            // int diff = currScope->all_push - llo;
            if (m.find(name) != m.end()) {
                // int val = 4 * (ptr->last_off - m.at(name) +
                //                currScope->loc_last_off + diff);
                int val = 4 * (currScope->all_push - m.at(name));
                file << "   LOAD R0, (R7+" << 0 << hex << val << dec << ")"
                     << endl;
                file << "   ADD R0, " << hex << a << dec << ", R0" << endl;
                file << "   STORE R0, (R7+" << 0 << hex << val << dec << ")"
                     << endl;
                // currScope->all_push++;
            } else if (m2.find(name) != m2.end()) {
                int val = 4 * (currScope->all_push - m2.at(name));
                file << "   LOAD R0, (R7+" << 0 << hex << val << dec << ")"
                     << endl;
                file << "   ADD R0, " << hex << a << dec << ", R0" << endl;
                file << "   STORE R0, (R7+" << 0 << hex << val << dec << ")"
                     << endl;
                // currScope->all_push++;
            }
            ptr.reset();
            Type t = UNARNI_IZRAZ(u[curr]);
            if (t.lexp == 1 && checkImp(t.type, Int)) {
                return t;
            } else {
                prodErr(node);
            }

        } else {
            errPrint("KRIVI ULAZ ZNAKOVA 1 UNARNI IZRAZ");
        }
    } else if (v[0] == "<unarni_operator>") {
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<cast_izraz>") {
            Type t = CAST_IZRAZ(u[curr]);
            if (checkImp(t.type, Int)) {
                return Type(Int, false);
            } else {
                prodErr(node);
            }
        } else {
            errPrint("KRIVI ULAZ ZNAKOVA 3 UNARNI IZRAZ");
        }
    } else {
        errPrint("KRIVI ULAZ ZNAKOVA 2 UNARNI IZRAZ");
    }
    return Type(Error, false);
}

Type CAST_IZRAZ(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);
    if (v[0] == "<unarni_izraz>") {
        return UNARNI_IZRAZ(u[curr]);
    } else if (v[0] == "L_ZAGRADA") {
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<ime_tipa>") {
            type t = IME_TIPA(u[curr]);
            if (!(t == Int || t == kint || t == Char || t == kchar)) {
                cerr << "PRVI ERR NODE U CAST IZRAZ" << endl;
                prodErr(node);
            }
            curr++;
            ulaz = u[curr]->node;
            vector<string> v2 = splitSpaces(ulaz);
            if (v2[0] != "D_ZAGRADA")
                errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA CAST_IZRAZ 3");
            curr++;
            ulaz = u[curr]->node;
            if (ulaz == "<cast_izraz>") {
                Type t2 = CAST_IZRAZ(u[curr]);
                if (checkImp(t2.type, t) || (t2.type == Int && t == Char) ||
                    (t2.type == Int && t == kchar) ||
                    (t2.type == kint && t == kchar) ||
                    (t2.type == kint && t == Char)) {
                    return Type(t, false);
                } else {
                    cerr << "DRUGI ERR NODE U CAST IZRAZ" << t << " " << t2.type
                         << endl;
                    prodErr(node);
                }
            } else {
                errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA CAST_IZRAZ 2");
            }
        } else {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA CAST_IZRAZ 4");
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA CAST_IZRAZ 1");
    }
    return Type(Error, false);
}

type IME_TIPA(const shared_ptr<Tree> &node) {
    cerr << "U IME TIPA" << endl;
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);
    if (v[0] == "<specifikator_tipa>") {
        type t = SPECIFIKATOR_TIPA(u[curr]);
        return t;
    } else if (v[0] == "KR_CONST") {
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<specifikator_tipa>") {
            type t = SPECIFIKATOR_TIPA(u[curr]);
            if (t == Void) {
                prodErr(node);
            }
            if (t == Int) {
                return kint;
            } else if (t == Char) {
                return kchar;
            } else {
                prodErr(node);
            }
        } else {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA IME_TIPA 1");
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA IME_TIPA 2");
    }
    return Error;
}
type SPECIFIKATOR_TIPA(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    cerr << "U SPECIFIKATOR TIPA " << u.size() << endl;
    int curr = 0;
    string ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);
    cerr << v[0] << endl;
    if (v[0] == "KR_VOID") {
        return Void;
    } else if (v[0] == "KR_CHAR") {
        return Char;
    } else if (v[0] == "KR_INT") {
        cerr << "BLA" << endl;
        return Int;
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA SPECIFIKATOR_TIPA 1");
    }
    return Error;
}
Type MULTIPLIKATIVNI_IZRAZ(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;

    if (ulaz == "<cast_izraz>") {
        return CAST_IZRAZ(u[curr]);
    } else if (ulaz == "<multiplikativni_izraz>") {
        Type t = MULTIPLIKATIVNI_IZRAZ(u[curr]);
        if (!checkImp(t.type, Int))
            prodErr(node);
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        if (!(v[0] == "OP_PUTA" || v[0] == "OP_DIJELI" || v[0] == "OP_MOD")) {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA MULTIPLIKATIVNI_IZRAZ 1");
        }
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<cast_izraz>") {
            Type t2 = CAST_IZRAZ(u[curr]);
            if (!checkImp(t2.type, Int))
                prodErr(node);
            return Type(Int, false);
        }
    }
    return Type(Error, false);
}
Type ADITIVNI_IZRAZ(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<multiplikativni_izraz>") {
        return MULTIPLIKATIVNI_IZRAZ(u[curr]);
    } else if (ulaz == "<aditivni_izraz>") {
        Type t = ADITIVNI_IZRAZ(u[curr]);
        if (!checkImp(t.type, Int)) {
            prodErr(node);
        }
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        string sign = v[0];
        if (!(v[0] == "PLUS" || v[0] == "MINUS")) {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA ADITIVNI_IZRAZ 1");
        }
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<multiplikativni_izraz>") {
            Type t2 = MULTIPLIKATIVNI_IZRAZ(u[curr]);
            if (!checkImp(t2.type, Int))
                prodErr(node);

            file << "   POP R1" << endl;
            file << "   POP R0" << endl;
            if (sign == "PLUS")
                file << "   ADD R0, R1, R0" << endl;
            else
                file << "   SUB R0, R1, R0" << endl;
            file << "   PUSH R0" << endl;
            currScope->all_push--;
            return Type(Int, false);
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA ADITIVNI_IZRAZ 2");
    }
    return Type(Error, false);
}

Type ODNOSNI_IZRAZ(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<aditivni_izraz>") {
        return ADITIVNI_IZRAZ(u[curr]);
    } else if (ulaz == "<odnosni_izraz>") {
        Type t = ODNOSNI_IZRAZ(u[curr]);
        if (!checkImp(t.type, Int))
            prodErr(node);
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        if (!(v[0] == "OP_LT" || v[0] == "OP_GT" || v[0] == "OP_LTE" ||
              v[0] == "OP_GTE")) {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA ODNOSNI_IZRAZ 1");
        }
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<aditivni_izraz>") {
            Type t2 = ADITIVNI_IZRAZ(u[curr]);
            if (!checkImp(t2.type, Int))
                prodErr(node);
            file << "   POP R1" << endl;
            file << "   POP R0" << endl;
            file << "   CMP R0, R1" << endl;
            // file << JP_opertator ;
            if (v[0] == "OP_LT") {
                file << "   JP_SLT opt_" << ops << endl;
                file << "   JP_SGE opf_" << ops << endl;
            } else if (v[0] == "OP_GT") {
                file << "   JP_SGT opt_" << ops << endl;
                file << "   JP_SLE opf_" << ops << endl;
            } else if (v[0] == "OP_LTE") {
                file << "   JP_SLE opt_" << ops << endl;
                file << "   JP_SGT opf_" << ops << endl;
            } else if (v[0] == "OP_GTE") {
                file << "   JP_SGE opt_" << ops << endl;
                file << "   JP_SLT opf_" << ops << endl;
            }
            file << "opt_" << ops << endl;
            file << "   MOVE 1, R1" << endl;
            file << "   JP open_" << ops << endl;
            file << "opf_" << ops << endl;
            file << "   MOVE 0, R1" << endl;
            file << "open_" << ops << endl;
            file << "   PUSH R1" << endl;
            currScope->all_push--;
            return Type(Int, false);
        } else {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA ODNOSNI_IZRAZ 2");
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA ODNOSNI_IZRAZ 3");
    }
    return Type(Error, false);
}

Type JEDNAKOSNI_IZRAZ(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<odnosni_izraz>") {
        return ODNOSNI_IZRAZ(u[curr]);
    } else if (ulaz == "<jednakosni_izraz>") {
        Type t = JEDNAKOSNI_IZRAZ(u[curr]);
        if (!checkImp(t.type, Int))
            prodErr(node);
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        if (!(v[0] == "OP_EQ" || v[0] == "OP_NEQ")) {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA JEDNAKOSNI_IZRAZ 1");
        }
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<odnosni_izraz>") {
            Type t2 = ODNOSNI_IZRAZ(u[curr]);
            if (!checkImp(t2.type, Int))
                prodErr(node);
            file << "   POP R1" << endl;
            file << "   POP R0" << endl;
            file << "   CMP R0, R1" << endl;
            // file << JP_opertator ;
            if (v[0] == "OP_EQ") {
                file << "   JP_EQ opt_" << ops << endl;
                file << "   JP_NE opf_" << ops << endl;
            } else {
                file << "   JP_NE opt_" << ops << endl;
                file << "   JP_EQ opf_" << ops << endl;
            }
            file << "opt_" << ops << endl;
            file << "   MOVE 1, R1" << endl;
            file << "   JP open_" << ops << endl;
            file << "opf_" << ops << endl;
            file << "   MOVE 0, R1" << endl;
            file << "open_" << ops << endl;
            file << "   PUSH R1" << endl;
            currScope->all_push--;

            return Type(Int, false);
        } else {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA JEDNAKOSNI_IZRAZ 2");
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA JEDNAKOSNI_IZRAZ 3");
    }
    return Type(Error, false);
}
Type BIN_I_IZRAZ(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<jednakosni_izraz>") {
        return JEDNAKOSNI_IZRAZ(u[curr]);
    } else if (ulaz == "<bin_i_izraz>") {
        Type t = BIN_I_IZRAZ(u[curr]);
        if (!checkImp(t.type, Int))
            prodErr(node);
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        if (!(v[0] == "OP_BIN_I")) {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA BIN_I_IZRAZ 1");
        }
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<jednakosni_izraz>") {
            Type t2 = JEDNAKOSNI_IZRAZ(u[curr]);
            if (!checkImp(t2.type, Int))
                prodErr(node);
            file << "   POP R1" << endl;
            file << "   POP R0" << endl;
            file << "   AND R0, R1, R0" << endl;
            file << "   PUSH R0" << endl;
            currScope->all_push--;
            return Type(Int, false);
        } else {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA  BIN_I_IZRAZ 2");
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA BIN_I_IZRAZ 3");
    }
    return Type(Error, false);
}
Type BIN_XILI_IZRAZ(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<bin_i_izraz>") {
        return BIN_I_IZRAZ(u[curr]);
    } else if (ulaz == "<bin_xili_izraz>") {
        Type t = BIN_XILI_IZRAZ(u[curr]);
        if (!checkImp(t.type, Int))
            prodErr(node);
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        if (!(v[0] == "OP_BIN_XILI")) {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA BIN_XILI_IZRAZ 1");
        }
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<bin_i_izraz>") {
            Type t2 = BIN_I_IZRAZ(u[curr]);
            if (!checkImp(t2.type, Int))
                prodErr(node);
            file << "   POP R1" << endl;
            file << "   POP R0" << endl;
            file << "   XOR R0, R1, R0" << endl;
            file << "   PUSH R0" << endl;
            currScope->all_push--;
            return Type(Int, false);
        } else {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA  BIN_XILI_IZRAZ 2");
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA BIN_XILI_IZRAZ 3");
    }
    return Type(Error, false);
}
Type BIN_ILI_IZRAZ(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<bin_xili_izraz>") {
        return BIN_XILI_IZRAZ(u[curr]);
    } else if (ulaz == "<bin_ili_izraz>") {
        Type t = BIN_ILI_IZRAZ(u[curr]);
        if (!checkImp(t.type, Int))
            prodErr(node);
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        if (!(v[0] == "OP_BIN_ILI")) {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA BIN_ILI_IZRAZ 1");
        }
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<bin_xili_izraz>") {
            Type t2 = BIN_XILI_IZRAZ(u[curr]);
            if (!checkImp(t2.type, Int))
                prodErr(node);
            file << "   POP R1" << endl;
            file << "   POP R0" << endl;
            file << "   OR R0, R1, R0" << endl;
            file << "   PUSH R0" << endl;
            currScope->all_push--;
            return Type(Int, false);
        } else {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA  BIN_ILI_IZRAZ 2");
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA BIN_ILI_IZRAZ 3");
    }
    return Type(Error, false);
}
Type LOG_I_IZRAZ(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<bin_ili_izraz>") {
        return BIN_ILI_IZRAZ(u[curr]);
    } else if (ulaz == "<log_i_izraz>") {
        Type t = LOG_I_IZRAZ(u[curr]);
        if (!checkImp(t.type, Int))
            prodErr(node);
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        if (!(v[0] == "OP_I")) {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA LOG_I_IZRAZ 1");
        }
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<bin_ili_izraz>") {
            Type t2 = BIN_ILI_IZRAZ(u[curr]);
            if (!checkImp(t2.type, Int))
                prodErr(node);
            return Type(Int, false);
        } else {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA  LOG_I_IZRAZ 2");
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA LOG_I_IZRAZ 3");
    }
    return Type(Error, false);
}
Type LOG_ILI_IZRAZ(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<log_i_izraz>") {
        return LOG_I_IZRAZ(u[curr]);
    } else if (ulaz == "<log_ili_izraz>") {
        Type t = LOG_ILI_IZRAZ(u[curr]);
        if (!checkImp(t.type, Int))
            prodErr(node);
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        if (!(v[0] == "OP_ILI")) {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA LOG_ILI_IZRAZ 1");
        }
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<log_i_izraz>") {
            Type t2 = LOG_I_IZRAZ(u[curr]);
            if (!checkImp(t2.type, Int))
                prodErr(node);
            return Type(Int, false);
        } else {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA  LOG_ILI_IZRAZ 2");
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA LOG_ILI_IZRAZ 3");
    }
    return Type(Error, false);
}
Type IZRAZ_PRIDRUZIVANJA(const shared_ptr<Tree> &node) {
    cerr << "U IZRAZ_PRIDRUZIVANJA" << endl;
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<log_ili_izraz>") {
        return LOG_ILI_IZRAZ(u[curr]);
    } else if (ulaz == "<postfiks_izraz>") {
        assign += 1;
        string last_av = av;
        Type t = POSTFIKS_IZRAZ(u[curr]);
        if (t.lexp != 1)
            prodErr(node);
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        if (!(v[0] == "OP_PRIDRUZI")) {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA IZRAZ_PRIDRUZIVANJA 1");
        }
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<izraz_pridruzivanja>") {
            Type t2 = IZRAZ_PRIDRUZIVANJA(u[curr]);
            if (!checkImp(t2.type, t.type))
                prodErr(node);
            /// KOD ZA ASSIGN
            shared_ptr<treeNode> ptr = currScope;
            while (ptr != root) {
                map<string, Type> &m = ptr->table;
                if (m.find(av) != m.end()) {
                    break;
                }
                ptr = ptr->parent;
            }
            if (ptr != root) {
                map<string, Type> &m3 = ptr->table;
                Type t = m3[av];
                const unordered_map<string, int> &m = ptr->offsets;
                const unordered_map<string, int> &m2 = ptr->loc_off;
                if (t.type == Int || t.type == kchar || t.type == Char ||
                    t.type == kint) {
                    // int llo = currScope->loc_last_off;
                    // int diff = (currScope->all_push - 1) - llo;
                    if (m.find(av) != m.end()) {
                        // int val = 4 * (ptr->last_off - m.at(av) +
                        //                currScope->loc_last_off + diff);
                        currScope->all_push--;
                        int val = 4 * (ptr->all_push - m.at(av));
                        file << "   POP R0" << endl;
                        file << "   STORE R0, (R7+" << 0 << hex << val << dec
                             << ")" << endl;
                        if (assign - 1 >= 0) {
                            file << "   PUSH R0" << endl;
                            currScope->all_push++;
                        }
                    } else if (m2.find(av) != m2.end()) {
                        currScope->all_push--;
                        int val = (4 * (currScope->all_push - m2.at(av)));
                        file << "   POP R0" << endl;
                        file << "   STORE R0, (R7+" << 0 << hex << val << dec
                             << ")" << endl;
                        if (assign - 1 >= 0) {
                            file << "   PUSH R0" << endl;
                            currScope->all_push++;
                        }
                    }
                } else {
                    if (m.find(av) != m.end()) {
                        // int val = 4 * (ptr->last_off - m.at(av) +
                        //                currScope->loc_last_off + diff);
                        currScope->all_push--;
                        int val = 4 * (ptr->all_push - m.at(av));
                        file << "   POP R0" << endl;
                        file << "   POP R1" << endl;
                        file << "   SHL R1, 2, R1" << endl;
                        file << "   MOVE " << hex << val << ", R2"

                             << endl;
                        file << "   ADD R2, R1, R2" << endl;
                        file << "   ADD R2, R7, R2" << endl;
                        file << "   STORE R0,(R2)" << endl;
                        currScope->all_push -= 2;

                    } else if (m2.find(av) != m2.end()) {
                        currScope->all_push--;
                        int val = (4 * (currScope->all_push - m2.at(av)));
                        file << "   POP R0" << endl;
                        file << "   POP R1" << endl;
                        file << "   SHL R1, 2, R1" << endl;
                        file << "   MOVE " << hex << val << ", R2"

                             << endl;
                        file << "   ADD R2, R1, R2" << endl;
                        file << "   ADD R2, R7, R2" << endl;
                        file << "   STORE R0,(R2)" << endl;
                        currScope->all_push -= 2;
                    }
                }
            } else {
                map<string, Type> &m = ptr->table;
                Type t = m[av];
                if (t.type == Int || t.type == kchar || t.type == Char ||
                    t.type == kint) {
                    // currScope->loc_off[av] = currScope->all_push + 1;
                    //  currScope->all_push++;
                    file << "   POP R0" << endl;
                    file << "   STORE R0, (" << hex << globals[av] * 4 + 4096
                         << dec << ")" << endl;
                    currScope->all_push--;
                } else {
                    file << "   POP R0" << endl;
                    file << "   POP R1" << endl;
                    file << "   SHL R1, 2, R1" << endl;
                    file << "   MOVE " << hex << 4096 + globals[av] * 4
                         << ", R2"

                         << endl;
                    file << "   ADD R2, R1, R2" << endl;
                    file << "   STORE R0,(R2)" << endl;
                    currScope->all_push -= 2;
                }
            }
            av = last_av;
            if (assign > 0) {
                assign--;
            }
            return Type(t.type, false);
        } else {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA  IZRAZ_PRIDRUZIVANJA 2");
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA IZRAZ_PRIDRUZIVANJA 3");
    }
    return Type(Error, false);
}
Type IZRAZ(const shared_ptr<Tree> &node) {
    cerr << "U IZRAZ" << endl;
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<izraz_pridruzivanja>") {
        return IZRAZ_PRIDRUZIVANJA(u[curr]);
    } else if (ulaz == "<izraz>") {
        Type t = IZRAZ(u[curr]);
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        if (v[0] != "ZAREZ") {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA IZRAZ 1");
        }
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<izraz_pridruzivanja>") {
            Type t2 = IZRAZ_PRIDRUZIVANJA(u[curr]);
            return Type(t2.type, false);
        } else {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA IZRAZ 2");
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA IZRAZ 3");
    }
    return Type(Error, false);
}

// void SLOZENA_NAREDBA(const shared_ptr<Tree> &node);
void LISTA_NAREDBI(const shared_ptr<Tree> &node);
void LISTA_DEKLARACIJA(const shared_ptr<Tree> &node);
void NAREDBA(const shared_ptr<Tree> &node);
type IZRAZ_NAREDBA(const shared_ptr<Tree> &node);
void NAREDBA_GRANANJA(const shared_ptr<Tree> &node);
void NAREDBA_PETLJE(const shared_ptr<Tree> &node);
void NAREDBA_SKOKA(const shared_ptr<Tree> &node);
void VANJSKA_DEKLARACIJA(const shared_ptr<Tree> &node);
void DEFINICIJA_FUNKCIJE(const shared_ptr<Tree> &node);
void DEKLARACIJA(const shared_ptr<Tree> &node);
Lparam LISTA_PARAMETARA(const shared_ptr<Tree> &node);

void SLOZENA_NAREDBA(const shared_ptr<Tree> &node, const vector<string> &names,
                     const vector<type> &types, string IDN) {
    cerr << "U SLOZENA_NAREDBA" << endl;
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);
    // novi djelogkrug
    shared_ptr<treeNode> newScope = make_shared<treeNode>();
    newScope->parent = currScope;
    newScope->IDN = IDN;
    newScope->loc_last_off = currScope->loc_last_off;
    if (IDN != "") {
        newScope->all_push = currScope->all_push + 1;
    } else {
        newScope->all_push = currScope->all_push;
    }
    currScope->children.push_back(newScope);
    currScope = newScope;
    if (!names.empty()) {
        for (int i = 0; i < names.size(); i++) {
            currScope->table[names[i]] = Type(types[i], true);
        }
        int sp = currScope->all_push - 1;
        for (int i = names.size() - 1; i >= 0; --i) {
            type t = types[i];
            if (t == Int || t == Char || t == kint || t == kchar) {
                currScope->offsets[names[i]] = sp;
                sp -= 1;
            } else {
                shared_ptr<treeNode> ptr = currScope;
                while (ptr != root) {
                    const unordered_map<string, int> &m = ptr->loc_off;
                    if (m.find(names[i]) == m.end()) {
                        break;
                    }
                }
                if (ptr != root)
                    currScope->loc_off[names[i]] = ptr->loc_off[names[i]];
            }
        }
    }
    if (v[0] == "L_VIT_ZAGRADA") {
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<lista_naredbi>") {
            LISTA_NAREDBI(u[curr]);
            curr++;
            ulaz = u[curr]->node;
            v = splitSpaces(ulaz);
            if (v[0] != "D_VIT_ZAGRADA") {
                errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA SLOZENA_NAREDBA 1");
            }
        } else if (ulaz == "<lista_deklaracija>") {
            LISTA_DEKLARACIJA(u[curr]);
            curr++;
            ulaz = u[curr]->node;
            if (ulaz != "<lista_naredbi>") {
                errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA SLOZENA_NAREDBA 2");
            }
            LISTA_NAREDBI(u[curr]);
            curr++;
            ulaz = u[curr]->node;
            v = splitSpaces(ulaz);
            if (v[0] != "D_VIT_ZAGRADA") {
                errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA SLOZENA_NAREDBA 3");
            }
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA SLOZENA_NAREDBA 4");
    }
    currScope = currScope->parent;
    if (currScope->table[IDN].retyp == Void) {
        file << "   RET" << endl;
    }
}
void LISTA_NAREDBI(const shared_ptr<Tree> &node) {
    cerr << "U LISTA NAREDBI" << endl;
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<naredba>") {
        NAREDBA(u[curr]);
    } else if (ulaz == "<lista_naredbi>") {
        LISTA_NAREDBI(u[curr]);
        curr++;
        ulaz = u[curr]->node;
        if (ulaz != "<naredba>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA LISTA_NAREDBI 1");
        NAREDBA(u[curr]);
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA LISTA_NAREDBI 1");
    }
}
void NAREDBA(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<slozena_naredba>") {
        SLOZENA_NAREDBA(u[curr], vector<string>(), vector<type>(), "");
    } else if (ulaz == "<izraz_naredba>") {
        IZRAZ_NAREDBA(u[curr]);
    } else if (ulaz == "<naredba_grananja>") {
        NAREDBA_GRANANJA(u[curr]);
    } else if (ulaz == "<naredba_petlje>") {
        NAREDBA_PETLJE(u[curr]);
    } else if (ulaz == "<naredba_skoka>") {
        NAREDBA_SKOKA(u[curr]);
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA 1");
    }
}

type IZRAZ_NAREDBA(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);
    if (v[0] == "TOCKAZAREZ") {
        return Int;
    } else if (v[0] == "<izraz>") {
        Type t = IZRAZ(u[curr]);
        curr++;
        ulaz = u[curr]->node;
        v = splitSpaces(ulaz);
        if (v[0] != "TOCKAZAREZ")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA IZRAZ_NAREDBA 1");
        return t.type;
    }
    return Error;
}

void NAREDBA_GRANANJA(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);
    if (v[0] != "KR_IF")
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_GRANANJA 1");
    curr++;
    ulaz = u[curr]->node;
    v = splitSpaces(ulaz);
    if (v[0] != "L_ZAGRADA")
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_GRANANJA 2");
    curr++;
    ulaz = u[curr]->node;
    if (ulaz != "<izraz>")
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_GRANANJA 3");
    Type t = IZRAZ(u[curr]);
    if (!checkImp(t.type, Int)) {
        prodErr(node);
    }
    curr++;
    ulaz = u[curr]->node;
    v = splitSpaces(ulaz);

    file << "   POP R0" << endl;
    file << "   CMP R0, 1" << endl;
    currScope->all_push--;
    if (v[0] != "D_ZAGRADA")
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_GRANANJA 4");
    if (u.size() == 5) {
        curr++;
        ulaz = u[curr]->node;
        if (ulaz != "<naredba>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_GRANANJA 5");
        file << "   JP_EQ if_" << ifs << endl;
        file << "   JP_NE enif_" << ifs << endl;
        file << "if_" << ifs << endl;
        NAREDBA(u[curr]);
    } else if (u.size() > 5) {
        curr++;
        ulaz = u[curr]->node;
        if (ulaz != "<naredba>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_GRANANJA 5");
        file << "   JP_EQ if_" << ifs << endl;
        file << "   JP_NE el_" << ifs << endl;
        file << "if_" << ifs << endl;
        NAREDBA(u[curr]);
        file << "   JP enif_" << ifs << endl;
        curr++;
        ulaz = u[curr]->node;
        v = splitSpaces(ulaz);
        if (v[0] != "KR_ELSE")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_GRANANJA 6");
        curr++;
        ulaz = u[curr]->node;
        if (ulaz != "<naredba>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_GRANANJA 5");
        file << "el_" << ifs << endl;
        NAREDBA(u[curr]);
    } else {
        errPrint("KRIVO STABLO NAREDBA_GRANANJA");
    }
    file << "enif_" << ifs << endl;
    ifs++;
}

void NAREDBA_PETLJE(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);

    if (v[0] == "KR_WHILE") {
        curr++;
        ulaz = u[curr]->node;
        v = splitSpaces(ulaz);
        if (v[0] != "L_ZAGRADA")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_PETLJE 1");
        curr++;
        ulaz = u[curr]->node;
        if (ulaz != "<izraz>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_PETLJE 2");
        Type t = IZRAZ(u[curr]);
        if (!checkImp(t.type, Int))
            prodErr(node);
        curr++;
        ulaz = u[curr]->node;
        v = splitSpaces(ulaz);
        if (v[0] != "D_ZAGRADA")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_PETLJE 3");
        curr++;
        ulaz = u[curr]->node;
        if (ulaz != "<naredba>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_PETLJE 4");
        NAREDBA(u[curr]);
    } else if (v[0] == "KR_FOR") {
        curr++;
        ulaz = u[curr]->node;
        v = splitSpaces(ulaz);
        if (v[0] != "L_ZAGRADA")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_PETLJE 5");
        curr++;
        ulaz = u[curr]->node;
        file << "for_" << fors << endl;
        if (ulaz != "<izraz_naredba>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_PETLJE 6");
        type t = IZRAZ_NAREDBA(u[curr]);
        curr++;
        ulaz = u[curr]->node;
        file << "foru_" << fors << endl;
        if (ulaz != "<izraz_naredba>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_PETLJE 7");
        type t2 = IZRAZ_NAREDBA(u[curr]);
        if (!checkImp(t2, Int))
            prodErr(node);
        file << "   POP R0" << endl;
        file << "   CMP R0, 1" << endl;
        currScope->all_push--;
        file << "   JP_EQ forn_" << fors << endl;
        file << "   JP_NE fore_" << fors << endl;
        file << "fori_" << fors << endl;
        if (u.size() == 7) {
            curr++;
            ulaz = u[curr]->node;
            if (ulaz != "<izraz>")
                errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_PETLJE 8");
            IZRAZ(u[curr]);
        }
        file << "   POP R5" << endl;
        currScope->all_push--;
        file << "   JP foru_" << fors << endl;
        curr++;
        ulaz = u[curr]->node;
        v = splitSpaces(ulaz);
        if (v[0] != "D_ZAGRADA")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_PETLJE 9");
        curr++;
        ulaz = u[curr]->node;
        file << "forn_" << fors << endl;
        if (ulaz != "<naredba>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_PETLJE 10");
        NAREDBA(u[curr]);
        file << "   JP fori_" << fors << endl;
        file << "fore_" << fors << endl;
        fors++;
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_PETLJE 11");
    }
}

void NAREDBA_SKOKA(const shared_ptr<Tree> &node) {
    cerr << "U NAREDBA_SKOKA" << endl;
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);
    if (v[0] == "KR_CONTINUE" || v[0] == "KR_BREAK") {
        shared_ptr<Tree> ptr = node;
        while (ptr != nullptr && ptr->node != "<naredba_petlje>")
            ptr = ptr->parent;
        if (ptr == nullptr) {
            ptr.reset();
            prodErr(node);
        }
        ptr.reset();
        curr++;
        ulaz = u[curr]->node;
        v = splitSpaces(ulaz);
        if (v[0] != "TOCKAZAREZ")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBBA_SKOKA 1");
    } else if (v[0] == "KR_RETURN") {
        shared_ptr<treeNode> ptr = currScope;
        while (ptr->IDN == "" && ptr != nullptr) {
            ptr = ptr->parent;
        }
        if (ptr == nullptr)
            prodErr(node);
        string name = ptr->IDN;
        ptr = ptr->parent;
        map<string, Type> &m = ptr->table;
        if (ptr == nullptr) {
            errPrint("NULLPTR");
        }
        Type t = m[name];
        ptr.reset();
        curr++;
        ulaz = u[curr]->node;
        v = splitSpaces(ulaz);
        if (v[0] == "TOCKAZAREZ") {
            if (!(t.retyp == Void && t.type == Func))
                prodErr(node);
        } else if (v[0] == "<izraz>") {
            Type t2 = IZRAZ(u[curr]);
            if (!checkImp(t2.type, t.retyp)) {
                cerr << t.retyp << " " << t2.type << endl;
                prodErr(node);
            }
        }
        int a = 0;
        shared_ptr<treeNode> ptr2 = currScope;
        while (ptr2 != root) {
            const unordered_map<string, int> &m = ptr2->loc_off;
            for (const auto &e : m) {
                Type t = ptr2->table[e.first];
                if (t.numElem == -1) {
                    a += 1;
                } else {
                    a += t.numElem;
                }
            }
            ptr2 = ptr2->parent;
        }
        file << "   POP R6" << endl;
        currScope->all_push--;
        file << "   ADD R7, " << hex << 0 << a * 4 << dec << ", R7" << endl;
        currScope->all_push -= a;
        file << "   RET" << endl;
        ptr2.reset();
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA NAREDBA_SKOKA 2");
    }
}

void PRIJEVODNA_JEDINICA(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    cerr << "PRIJEVODNA_JEDINICA" << endl;
    if (ulaz == "<vanjska_deklaracija>") {
        VANJSKA_DEKLARACIJA(u[curr]);
    } else if (ulaz == "<prijevodna_jedinica>") {
        PRIJEVODNA_JEDINICA(u[curr]);
        curr++;
        ulaz = u[curr]->node;
        if (ulaz != "<vanjska_deklaracija>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA PRIJEVODNA_JEDINICA 1");
        VANJSKA_DEKLARACIJA(u[curr]);
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA PRIJEVODNA_JEDINICA 2");
    }
}
void VANJSKA_DEKLARACIJA(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    cerr << "VANJSKA_DEKLARACIJA" << " " << ulaz << endl;
    if (ulaz == "<definicija_funkcije>") {
        cerr << "VANJSKA_2" << endl;
        DEFINICIJA_FUNKCIJE(u[curr]);
    } else if (ulaz == "<deklaracija>") {
        DEKLARACIJA(u[curr]);
    }
}

void DEFINICIJA_FUNKCIJE(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    cerr << "BLA" << endl;
    if (ulaz != "<ime_tipa>")
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA DEFINICIJA_FUNKCIJE 1");

    type t = IME_TIPA(u[curr]);
    if (t == kint || t == kchar) {
        prodErr(node);
    }

    curr++;
    ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);
    if (v[0] != "IDN")
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA DEFINICIJA_FUNKCIJE 2");
    bool defined = false;
    string name = v[2];
    file << "f_" << name << endl;

    shared_ptr<treeNode> ptr = currScope; // always root
    map<string, Type> &m = ptr->table;
    if (m.find(name) != m.end()) {
        if (m[name].fundef)
            defined = true;
    }
    ptr = ptr->parent;

    if (defined) {
        ptr.reset();
        prodErr(node);
    }
    curr++;
    ulaz = u[curr]->node;
    v = splitSpaces(ulaz);
    if (v[0] != "L_ZAGRADA")
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA DEFINICIJA_FUNKCIJE 3");
    curr++;
    ulaz = u[curr]->node;
    v = splitSpaces(ulaz);
    if (v[0] == "KR_VOID") {
        if (m.find(name) != m.end()) {
            Type &f = m[name];
            if (f.fundef) {
                ptr.reset();
                errPrint("FUNKCIJA JE VEC DEKLARIRANA IL JE KRIVI TIP NA IDN");
            }
            if (!(f.fparam.size() == 0 && f.retyp == t)) {
                ptr.reset();
                prodErr(node);
            }
        }
        m[name] = Type(Func, t, true);
        cerr << name << " " << m[name].type << " " << m[name].retyp << " "
             << m[name].fundef << endl;
        curr++;
        ulaz = u[curr]->node;
        v = splitSpaces(ulaz);
        if (v[0] != "D_ZAGRADA") {
            ptr.reset();
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA DEFINICIJA_FUNKCIJE 4");
        }
        ptr.reset();
        curr++;
        ulaz = u[curr]->node;
        if (ulaz != "<slozena_naredba>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA DEFINICIJA_FUNKCIJE 5");
        SLOZENA_NAREDBA(u[curr], vector<string>(), vector<type>(), name);
    } else if (v[0] == "<lista_parametara>") {
        int z = 0;
        Lparam l = LISTA_PARAMETARA(u[curr]);
        vector<type> &types = l.fparam;
        cerr << "IMAM LISTU PARAMETARA" << endl;
        if (m.find(name) != m.end()) {
            cerr << "BLA2" << endl;
            Type &f = m[name];
            cerr << "BLA" << endl;
            if (m[name].fundef) {
                ptr.reset();
                errPrint("FUNKCIJA JE VEC DEKLARIRANA IL JE KRIVI TIP NA IDN");
            }
            if (types.size() == f.fparam.size()) {
                for (int i = 0; i < types.size(); i++) {
                    if (types[i] == f.fparam[i])
                        z++;
                }
            }
            if (!(z == types.size() && types.size() != 0 && t == f.retyp)) {
                ptr.reset();
                prodErr(node);
            }
        }
        cerr << "ZAVRSIO PROVJERU DEKLARACIJE I DEFINICIJE" << endl;
        m[name] = Type(Func, l.fparam, l.nparam, t, true);
        cerr << "ZAVRSIO PROVJERU DEKLARACIJE I DEFINICIJE 2" << endl;
        ptr.reset();
        curr++;
        ulaz = u[curr]->node;
        v = splitSpaces(ulaz);
        if (v[0] != "D_ZAGRADA")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA DEFINICIJA_FUNKCIJE 4");
        curr++;
        ulaz = u[curr]->node;
        if (ulaz != "<slozena_naredba>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA DEFINICIJA_FUNKCIJE 5");
        SLOZENA_NAREDBA(u[curr], l.nparam, l.fparam, name);
    } else {
        ptr.reset();
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA DEFINICIJA_FUNKCIJE 4");
    }
}

pair<string, type> DEKLARACIJA_PARAMETRA(const shared_ptr<Tree> &node);

Lparam LISTA_PARAMETARA(const shared_ptr<Tree> &node) {
    cerr << "U LISTA_PARAMETARA" << endl;
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    Lparam ret = Lparam();
    if (ulaz == "<deklaracija_parametra>") {
        pair<string, type> p = DEKLARACIJA_PARAMETRA(u[curr]);
        ret.nparam.push_back(p.first);
        ret.fparam.push_back(p.second);
        cerr << "LISTA_PARAMETERA RET NAMJESTEN" << endl;
    } else if (ulaz == "<lista_parametara>") {
        ret = LISTA_PARAMETARA(u[curr]);
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        if (v[0] != "ZAREZ")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA LISTA_PARAMETARA 1");
        curr++;
        ulaz = u[curr]->node;
        if (ulaz != "<deklaracija_parametra>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA LISTA_PARAMETARA 2");
        pair<string, type> p = DEKLARACIJA_PARAMETRA(u[curr]);
        if (find(ret.nparam.begin(), ret.nparam.end(), p.first) !=
            ret.nparam.end()) {
            prodErr(node);
        }
        ret.nparam.push_back(p.first);
        ret.fparam.push_back(p.second);
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA LISTA_PARAMETARA 3");
    }
    cerr << "VRACAM LISTU PARAMETARA" << endl;
    return ret;
}
pair<string, type> DEKLARACIJA_PARAMETRA(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    cerr << "U DEKLARACIJA_PARAMETRA " << u.size() << endl;
    if (ulaz != "<ime_tipa>")
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA DEKLARACIJA_PARAMETRA 1");
    type t = IME_TIPA(u[curr]);
    if (t == Void)
        prodErr(node);
    curr++;
    ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);
    if (v[0] != "IDN")
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA DEKLARACIJA_PARAMETRA 2");
    string name = v[2];
    cerr << "TU SAM SAD" << endl;
    if (u.size() == 4) {
        if (t == Int) {
            t = arrint;
        } else if (t == Char) {
            t = arrchar;
        } else if (t == kchar) {
            t = arrkchar;
        } else if (t == kint) {
            t = arrkint;
        } else {
            prodErr(node);
        }
    } else if (u.size() != 2) {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA DEKLARACIJA_PARAMETRA 3");
    }
    return {name, t};
}
void LISTA_DEKLARACIJA(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    cerr << "U LISTA_DEKLARACIJA" << endl;
    if (ulaz == "<deklaracija>") {
        DEKLARACIJA(u[curr]);
    } else if (ulaz == "<lista_deklaracija>") {
        LISTA_DEKLARACIJA(u[curr]);
        curr++;
        ulaz = u[curr]->node;
        if (ulaz != "<deklaracija>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA LISTA_DEKLARACIJA 1");
        DEKLARACIJA(u[curr]);
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA LISTA_DEKLARACIJA 2");
    }
}
void LISTA_INIT_DEKLARATORA(const shared_ptr<Tree> &node, type nt);
void INIT_DEKLARATOR(const shared_ptr<Tree> &node, type nt);
Type IZRAVNI_DEKLARATOR(const shared_ptr<Tree> &node, type nt);
Inistruct INICIJALIZATOR(const shared_ptr<Tree> &node);

void DEKLARACIJA(const shared_ptr<Tree> &node) {
    cerr << "U DEKLARACIJA" << endl;
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz != "<ime_tipa>")
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA DEKLARACIJA 1");
    type t = IME_TIPA(u[curr]);
    curr++;
    ulaz = u[curr]->node;
    if (ulaz != "<lista_init_deklaratora>")
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA DEKLARACIJA 2");
    LISTA_INIT_DEKLARATORA(u[curr], t);
    curr++;
    ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);
    if (v[0] != "TOCKAZAREZ")
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA DEKLARACIJA 3");
}
void LISTA_INIT_DEKLARATORA(const shared_ptr<Tree> &node, type nt) {
    cerr << "U LISTA INIT DEKLARATORA" << endl;
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<init_deklarator>") {
        INIT_DEKLARATOR(u[curr], nt);
    } else if (ulaz == "<lista_init_deklaratora>") {
        LISTA_INIT_DEKLARATORA(u[curr], nt);
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        if (v[0] != "ZAREZ")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA LISTA_INIT_DEKLARATORA 1");
        curr++;
        ulaz = u[curr]->node;
        if (ulaz != "<init_deklarator>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA LISTA_INIT_DEKLARATORA 2");
        INIT_DEKLARATOR(u[curr], nt);
    }
}
void INIT_DEKLARATOR(const shared_ptr<Tree> &node, type nt) {
    cerr << "U INIT DEKLARATOR" << endl;
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz != "<izravni_deklarator>")
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA INIT_DEKLARATOR 1");
    Type t = IZRAVNI_DEKLARATOR(u[curr], nt);
    if (u.size() == 1) {
        if (t.type == kint || t.type == kchar || t.type == arrkint ||
            t.type == arrkchar)
            prodErr(node);
        // file << "   SUB R7, 4, R7" << endl;
    } else if (u.size() == 3) {
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        if (v[0] != "OP_PRIDRUZI")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA INIT_DEKLARATOR 2");
        curr++;
        ulaz = u[curr]->node;
        if (ulaz != "<inicijalizator>")
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA INIT_DEKLARATOR 3");
        Inistruct t2 = INICIJALIZATOR(u[curr]);
        if (t.type == Int || t.type == Char || t.type == kint ||
            t.type == kchar) {
            if (!(checkImp(t2.type, t.type)))
                prodErr(node);
        } else if ((t.type == arrint || t.type == arrchar ||
                    t.type == arrkint || t.type == arrkchar) &&
                   t2.numElem != -1) {
            if (t2.numElem <= t.numElem) {
                cerr << "INIT DEKLARATOR DEBUG " << t.numElem << " "
                     << t2.numElem << endl;
                for (int i = 0; i < t2.numElem; i++) {
                    if (t.type == arrint || t.type == arrkint) {
                        if (!checkImp(t2.types[i], Int)) {
                            cerr << "INIT DEKL ERR 1";
                            prodErr(node);
                        }
                    }
                    if (t.type == arrchar || t.type == arrkchar) {
                        if (!checkImp(t2.types[i], Char)) {
                            cerr << "INIT DEKL ERR 1";
                            prodErr(node);
                        }
                    }
                }
            } else {
                prodErr(node);
            }
        } else {
            prodErr(node);
        }
    }
    gind = -1;
}

Type IZRAVNI_DEKLARATOR(const shared_ptr<Tree> &node, type nt) {
    const vector<shared_ptr<Tree>> &u = node->children;
    cerr << "U IZRAVNI_DEKLARATOR " << u.size() << endl;
    int curr = 0;
    string ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);
    string name = v[2];
    map<string, Type> &m = currScope->table;
    if (v[0] != "IDN")
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA IZRAVNI_DEKLARATOR 1");
    shared_ptr<Tree> ptr2 = node;
    while (ptr2 != nullptr && ptr2->node != "<definicija_funkcije>")
        ptr2 = ptr2->parent;

    if (u.size() == 1) {
        if (ptr2 == nullptr) {
            curr_global = name;
            globals.insert({name, last_global});
            last_global++;
        }
        if (nt == Void)
            prodErr(node);
        if (m.find(name) != m.end())
            prodErr(node);
        m[name] = Type(nt, true);
        unordered_map<string, int> &m2 = currScope->loc_off;
        if (currScope != root) {
            if (node->parent->children.size() < 3) {
                file << "   SUB R7,4,R7" << endl;
                m2[name] = currScope->all_push + 1;
                currScope->all_push++;
            } else {
                m2[name] = currScope->all_push + 1;
            }
        }
        return Type(nt, true);
    } else if (u.size() == 4) {

        curr += 2;
        ulaz = u[curr]->node;
        v = splitSpaces(ulaz);
        if (v[0] == "BROJ") {
            if (nt == Void)
                prodErr(node);
            if (m.find(name) != m.end())
                prodErr(node);
            v = splitSpaces(ulaz);
            int val = stoi(v[2]);
            if (val <= 0 || val > 1024)
                prodErr(node);
            type newt;
            if (nt == Int)
                newt = arrint;
            if (nt == Char)
                newt = arrchar;
            if (nt == kint)
                newt = arrkint;
            if (nt == kchar)
                newt = arrkchar;
            m[name] = Type(newt, val, true);
            if (ptr2 == nullptr) {
                curr_global = name;
                globals.insert({name, last_global});
                last_global += val;
                gind = 0;
            } else {
                file << "    SUB R7, " << hex << 0 << val * 4 << dec << ", R7"
                     << endl;
                currScope->all_push += val;
                currScope->loc_off[name] = currScope->all_push;
            }
            return m[name];
        } else if (v[0] == "KR_VOID") {
            cerr << "OVDJE U IZRAVNOM DEKLARATORU" << endl;
            if (m.find(name) != m.end()) {
                if (!(m[name].fparam.size() == 0 && m[name].retyp == nt))
                    prodErr(node);
            } else {
                cerr << "RADIM NOVU FUNC";
                m[name] = Type(Func, nt, false);
            }
            return m[name];
        } else if (v[0] == "<lista_parametara>") {
            int z = 0;
            Lparam l = LISTA_PARAMETARA(u[curr]);
            vector<type> &types = l.fparam;
            if (m.find(name) != m.end()) {
                Type &f = m[name];
                if (types.size() == f.fparam.size()) {
                    for (int i = 0; i < types.size(); i++) {
                        if (types[i] == f.fparam[i])
                            z++;
                    }
                }
                if (!(z == types.size() && types.size() != 0)) {
                    prodErr(node);
                }
            } else {
                m[name] = Type(Func, l.fparam, l.nparam, nt, false);
            }
            return m[name];
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA IZRAVNI_DEKLARATOR 2");
    }
    return Type(Error, false);
}

Inistruct LISTA_IZRAZA_PRIDRUZIVANJA(const shared_ptr<Tree> &node) {
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<izraz_pridruzivanja>") {
        Type t = IZRAZ_PRIDRUZIVANJA(u[curr]);
        vector<type> typ;
        typ.push_back(t.type);
        return Inistruct(List, typ, 1);
    } else if (ulaz == "<lista_izraza_pridruzivanja>") {
        Inistruct in = LISTA_IZRAZA_PRIDRUZIVANJA(u[curr]);
        curr += 2;
        ulaz = u[curr]->node;
        Type t = IZRAZ_PRIDRUZIVANJA(u[curr]);
        in.numElem++;
        in.types.push_back(t.type);
        return in;
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA LISTA_IZRAZA_PRIDRUZIVANJA 1");
    }
    return Inistruct(Error, vector<type>(), -1);
}

Inistruct INICIJALIZATOR(const shared_ptr<Tree> &node) {
    cerr << "U INICIJALIZATOR" << endl;
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<izraz_pridruzivanja>") {
        Type t = IZRAZ_PRIDRUZIVANJA(u[curr]);
        if (findNode(u[curr], "NIZ_ZNAKOVA")) {
            vector<type> list;
            for (int i = 0; i < t.numElem + 1; i++)
                list.push_back(Char);
            return Inistruct(List, list, t.numElem + 1);
        } else {
            return Inistruct(t.type, vector<type>(), -1);
        }
    } else {
        curr++;
        ulaz = u[curr]->node;
        Inistruct in = LISTA_IZRAZA_PRIDRUZIVANJA(u[curr]);
        return in;
    }

    return Inistruct(Error, vector<type>(), -1);
}

bool checkFunc(const shared_ptr<treeNode> &node) {
    const vector<shared_ptr<treeNode>> &u = node->children;
    cerr << "U CHECKU" << endl;
    bool ret = true;
    if (!u.empty()) {
        for (int i = 0; i < u.size(); i++) {
            if (!checkFunc(u[i]))
                return false;
        }
    }
    for (const auto &e : node->table) {
        if (e.second.type == Func) {
            string name = e.first;
            map<string, Type> &m = root->table;
            if (m.find(name) != m.end()) {
                if (!m[name].fundef || m[name].retyp != e.second.retyp ||
                    m[name].fparam.size() != e.second.fparam.size()) {
                    return false;
                }
                if (m[name].fparam.size() > 0) {
                    vector<type> f1 = m[name].fparam, f2 = e.second.fparam;
                    int z = 1;
                    for (int i = 0; i < f1.size(); i++)
                        if (f1[i] != f2[i])
                            z = 0;
                    if (!z)
                        return false;
                }
            } else {
                ret = false;
            }
        }
        cerr << "FUNC DEBUG " << ret << " " << e.first << endl;
    }
    return ret;
}

Type test1() { return Type(Void, 2, 1); }
Type test() { return test1(); }

int main() {
    string line;
    while (getline(cin, line)) {
        input.push_back(line);
    }
    root = make_shared<treeNode>();
    currScope = root;
    troot = build(input);

    if (!file.is_open()) {
        cout << "Error opening the file." << endl;
        return 1;
    }
    file << "   MOVE 40000, R7" << endl;
    file << "   CALL f_main" << endl;
    file << "   HALT" << endl;
    file << endl;
    // pocetak analize i generatora
    PRIJEVODNA_JEDINICA(troot);

    for (const auto &k : consts) {
        file << k.second << "    DW %D " << stoi(to_string(k.first)) << endl;
    }
    file << "   `ORG 1000" << endl;
    for (const auto &e : global_ints) {
        file << "   DW %D " << e.second << endl;
    }
    cerr << "FINISHED REK" << endl;
    map<string, Type> &m = root->table;
    if (m.find("main") != m.end()) {
        if (!(m["main"].retyp == Int && m["main"].fparam.size() == 0)) {
            cout << "main" << endl;
        }
    } else {
        cout << "main" << endl;
    }
    for (const auto &e : m) {
        cerr << e.first << " " << e.second.type << " " << e.second.fparam.size()
             << endl;
    }
    if (!checkFunc(root))
        cout << "funkcija" << endl;

    destroyTree(troot);
    destroyTreeNode(root);
    file.close();
    cerr << endl << "FINISH" << endl;
    return 0;
}
