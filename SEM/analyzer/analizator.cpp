#include "structs.h"
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <utility>
using namespace std;

shared_ptr<treeNode> root = make_shared<treeNode>();
shared_ptr<treeNode> currScope = root;
shared_ptr<Tree> troot;

int currIn = 0;
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
        }
        stack.push({newNode, depth});
    }
    return root;
}

void printTree(const shared_ptr<Tree> &node, int depth) {
    if (!node)
        return;
    cout << string(depth, ' ') << node->node << endl;
    for (const auto &child : node->children) {
        printTree(child, depth + 1);
    }
}

bool checkImp(type t1, type t2) {
    if (t1 == kint) {
        if (t2 == Int)
            return true;
    } else if (t1 == kchar) {
        if (t2 == Int || t2 == Char || t2 == kint)
            return true;
    } else if (t1 == Char) {
        if (t2 == Int) {
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
    root.reset();
    currScope.reset();
    troot.reset();
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
    cerr << redS << message << redE << endl;
    root.reset();
    currScope.reset();
    troot.reset();
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
        ptr.reset();
        if (declared) {
            return t;
        } else {
            prodErr(node);
        }
    } else if (v[0] == "BROJ") {
        return Type(Int, false);
    } else if (v[0] == "ZNAK") {
        return Type(Char, false);
    } else if (v[0] == "NIZ_ZNAKOVA") {
        return Type(arrkchar, false);
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
                if (t.type == Func && t.fparam.size() == 0) {
                    return Type(t.retyp, false);
                } else {
                    prodErr(node);
                }
            } else if (v2[0] == "<lista_argumenata>") {
                vector<type> params = LISTA_ARGUMENATA(u[curr]);
                int z = 0;
                if (params.size() == t.fparam.size() && params.size() != 0) {
                    for (int i = 0; i < params.size(); i++) {
                        if (params[i] == t.fparam[i] &&
                            checkImp(params[i], t.fparam[i]))
                            // dovrsi checkImp
                            z++;
                    }
                }
                if (z == params.size()) {
                    return Type(t.retyp, false);
                } else {
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
        if (ulaz != "ZAREZ") {
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
    if (ulaz == "<postfiks_izraz>") {
        Type t = POSTFIKS_IZRAZ(u[curr]);
        return t;
    } else if (ulaz == "OP_INC" || ulaz == "OP_DEC") {
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<unarni_izraz>") {
            Type t = UNARNI_IZRAZ(u[curr]);
            if (t.lexp == 1 && checkImp(t.type, Int)) {
                return t;
            } else {
                prodErr(node);
            }
        } else {
            errPrint("KRIVI ULAZ ZNAKOVA 1 UNARNI IZRAZ");
        }
    } else if (ulaz == "<unarni_operator>") {
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
    } else if (ulaz == "L_ZAGRADA") {
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<ime_tipa>") {
            type t = IME_TIPA(u[curr]);
            if (!(t == Int || t == kint || t == Char || t == kchar)) {
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
                if (checkImp(t2.type, t) || (t2.type == Int && t == Char)) {
                    return Type(t, false);
                } else {
                    prodErr(node);
                }
            } else {
                errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA CAST_IZRAZ 2");
            }
        } else {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA CAST_IZRAZ 2");
        }
    } else {
        errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA CAST_IZRAZ 1");
    }
    return Type(Error, false);
}

type IME_TIPA(const shared_ptr<Tree> &node) {
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
    int curr = 0;
    string ulaz = u[curr]->node;
    vector<string> v = splitSpaces(ulaz);
    if (v[0] == "KR_VOID") {
        return Void;
    } else if (v[0] == "KR_CHAR") {
        return Char;
    } else if (v[0] == "KR_INT") {
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
        curr++;
        ulaz = u[curr]->node;
        if (!checkImp(t.type, Int)) {
            prodErr(node);
        }
        curr++;
        ulaz = u[curr]->node;
        vector<string> v = splitSpaces(ulaz);
        if (!(v[0] == "PLUS" || v[0] == "MINUS")) {
            errPrint("KRIVI ULAZNI ZNAKOVI GRAMATIKA ADITIVNI_IZRAZ 1");
        }
        curr++;
        ulaz = u[curr]->node;
        if (ulaz == "<multiplikativni_izraz>") {
            Type t2 = MULTIPLIKATIVNI_IZRAZ(u[curr]);
            if (!checkImp(t2.type, Int))
                prodErr(node);
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
    const vector<shared_ptr<Tree>> &u = node->children;
    int curr = 0;
    string ulaz = u[curr]->node;
    if (ulaz == "<log_ili_izraz>") {
        return LOG_ILI_IZRAZ(u[curr]);
    } else if (ulaz == "<postfiks_izraz>") {
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
Type test1() { return Type(Void, 2, 1); }
Type test() { return test1(); }

int main() {
    // map<string, Type> &m = currScope->table;
    // Type t = Type(arrint, 10, 1);
    // m.insert({"bla", t});
    // Type t2 = m["bla"];
    // cout << t2.type << " " << t2.numElem << " " << t2.lexp << endl;
    // cout << currScope->table["bla"].numElem << endl;
    // cout << currScope->parent << " " << currScope->children.size() << endl;
    // if (currScope->parent == nullptr) {
    //     cout << "BLA" << endl;
    // }
    string line;
    while (getline(cin, line)) {
        input.push_back(line);
    }
    root->children.push_back(make_shared<treeNode>());
    troot = build(input);
    printTree(troot, 0);
    POSTFIKS_IZRAZ(troot);
    // Type t3 = test();
    // cout << t3.type << " " << t3.numElem << " " << t3.lexp << endl;
    // root.reset();
    // currScope.reset();
    cerr << "FINISH" << endl;
    return 0;
}
