#include "structs.h"
#include <iostream>
#include <memory>
#include <string>

using namespace std;

shared_ptr<treeNode> root = make_shared<treeNode>();
shared_ptr<treeNode> currScope = root;
int currIn = 0;
vector<string> input;

string clean(string input) {
    int i = 0;
    while (input[i] == ' ')
        i++;
    return input.substr(i, input.size());
}

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

Type IZRAZ();
vector<type> LISTA_ARGUMENATA();

void errPrint(string message) {
    cerr << message << endl;
    root.reset();
    currScope.reset();
    exit(0);
}

Type PRIMARNI_IZRAZ(void) {
    string ulaz = input[currIn];
    currIn += 1;
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
                t.type = m[name].type;
                t.numElem = m[name].numElem;
                t.lexp = m[name].lexp;
                ptr = nullptr;
            } else {
                ptr = ptr->parent;
            }
        }
        if (declared) {
            return t;
        } else {
            ptr.reset();
            errPrint("ERROR WITH IDN IN PRIMARNI IZRAZ");
            return Type(Error, false);
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
        ulaz = input[currIn];
        currIn++;
        Type ret = Type(Error, false);
        if (ulaz == "<izraz>") {
            ret = IZRAZ();
        } else {
            errPrint("PRIMARNI_IZRAZ GRESKA KRIVI NIZ ZNAKOVA GRAMATIKE");
        }
        ulaz = input[currIn];
        currIn++;
        vector<string> v2 = splitSpaces(ulaz);
        if (v2[0] != "D_ZAGRADA") {
            errPrint("PRIMARNI_IZRAZ GRESKA KRIVI NIZ ZNAKOVA GRAMATIKE");
        }
        return ret;
    } else {
        errPrint("PRIMARNI_IZRAZ GRESKA KRIVI NIZ ZNAKOVA GRAMATIKE");
        return Type(Error, false);
    }
}

Type POSTFIKS_IZRAZ(void) {
    string ulaz = input[currIn];
    currIn += 1;
    if (ulaz == "<primarni_izraz>") {
        Type t = PRIMARNI_IZRAZ();
        return t;
    } else if (ulaz == "<postfiks_izraz") {
        Type t = POSTFIKS_IZRAZ();
        ulaz = input[currIn];
        currIn++;
        vector v = splitSpaces(ulaz);
        if (v[0] == "L_UGL_ZAGRADA") {

            if (t.type == arrint || t.type == arrchar || t.type == arrkint ||
                t.type == arrkchar) {
                ulaz = input[currIn];
                currIn++;
                if (ulaz == "<izraz>") {
                    Type t2 = IZRAZ();
                    if (t2.type == Int || t2.type == Char || t2.type == kchar ||
                        t2.type == kint) {
                        ulaz = input[currIn];
                        currIn++;
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
                                errPrint("Greska u tipu <postfiks_izraz> "
                                         "POSTFIKS_IZRAZ");
                            }
                        } else {
                            errPrint(
                                "GRESKA U ULAZNIM ZNAKOVIMA POSTFIKS_IZRAZ");
                        }
                    } else {
                        errPrint("GRESKA NIJE DAN DOBAR TIP U <izraz> -u");
                    }

                } else {
                    errPrint("GRESKA U ULAZNIM ZNAKOVIMA POSTFIKS_IZRAZ");
                }
            } else {
                errPrint("GRESKA nismo dobili valjani niz(X) za indeksiranje ");
            }

        } else if (v[0] == "L_ZAGRADA") {
            ulaz = input[currIn];
            currIn += 1;
            vector v2 = splitSpaces(ulaz);
            if (v2[0] == "D_ZAGRADA") {
                if (t.type == Func && t.fparam.size() == 0) {
                    return Type(t.retyp, false);
                } else {
                    errPrint("GRESKA FJA IMA KRIVE PARAMETRE KOJI NISU VOID");
                }
            } else if (v2[0] == "<lista_argumenata>") {
                vector<type> params = LISTA_ARGUMENATA();
                int z = 0;
                if (params.size() == t.fparam.size() && params.size() != 0) {
                    for (int i = 0; i < params.size(); i++) {
                        if (params[i] == t.fparam[i])
                            z++;
                    }
                }
                if (z == params.size()) {
                    // dalje
                } else {
                    errPrint("FUNKCIJA I LISTA PARAMETARA SE NE PODUDARAJU");
                }
            }
        } else {
            errPrint("GRESKA U ULAZNIM ZNAKOVIMA POSTFIKS_IZRAZ");
        }
    } else {
        errPrint("KRIVI ULAZ ZNAKOVA GRAMATIKE POSTFIKS_IZRAZ");
    }
    return Type(Error, false);
}
Type IZRAZ() { return Type(Error, false); }
vector<type> LISTA_ARGUMENATA() {
    vector<type> v;
    return v;
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
        input.push_back(clean(line));
        // cout << clean(line) << endl;
    }
    root->children.push_back(make_shared<treeNode>());
    // Type t3 = test();
    // cout << t3.type << " " << t3.numElem << " " << t3.lexp << endl;
    root.reset();
    currScope.reset();
    cout << "FINISH" << endl;
    return 0;
}
