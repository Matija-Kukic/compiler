#include "analyzerStruct.h"
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
using namespace std;

int main() {
    string line;
    int I = 0;
    tables tab;
    ifstream file("table.txt");
    char state = ' ';
    while (getline(file, line)) {
        if (I < 3) {
            vector<string> v = splitSpaces(line);
            if (v[0] == "%T") {
                for (int i = 1; i < v.size(); i++) {
                    tab.fnSign.insert(v[i]);
                }
            }
            if (v[0] == "%V") {
                for (int i = 1; i < v.size(); i++) {
                    tab.unSign.insert(v[i]);
                }
            }
            if (v[0] == "%Syn") {
                for (int i = 1; i < v.size(); i++) {
                    tab.syn.insert(v[i]);
                }
            }
            I++;
            continue;
        }
        if (line == "Productions") {
            state = 'p';
            continue;
        }
        if (line == "Actions") {
            state = 'a';
            continue;
        }
        if (line == "NEW STATES") {
            state = 'n';
            continue;
        }
        if (state == 'p') {
            vector<string> v = splitSpaces(line);
            string uf = v[0];
            string prod = "";
            for (int i = 0; i < v.size() - 1; i++) {
                prod += v[i];
                if (i < v.size() - 2)
                    prod += " ";
            }
            int st = stoi(v[v.size() - 1]);
            tab.prodMap[st] = {uf, prod};
        }
        if (state == 'a') {
            vector<string> v = splitSpaces(line);
            int cs = stoi(v[0]);
            string trsg = v[1];
            string act = v[2];
            tab.action[cs][trsg] = act;
        }
        if (state == 'n') {
            vector<string> v = splitSpaces(line);
            int cs = stoi(v[0]);
            string tr = v[1];
            string ns = v[2];
            tab.new_state[cs][tr] = ns;
        }
    }
    file.close();
    cout << "Unfinished signs: ";
    for (const auto &item : tab.unSign) {
        cout << item << " ; ";
    }
    cout << endl << "Finished signs: ";
    for (const auto &item : tab.fnSign) {
        cout << item << " ; ";
    }
    cout << endl << "Sync signs: ";
    for (const auto &item : tab.syn) {
        cout << item << " ; ";
    }
    cout << endl;
    cout << "Productions" << endl;
    for (const auto &it : tab.prodMap) {
        // cout << it.first.first << ", " << it.first.second << "; " <<
        // it.second
        //      << endl;
        cout << it.first << " " << it.second.first << " " << it.second.second
             << endl;
    }
    //    cout << "ACTION TABLE" << endl;
    cout << "Actions" << endl;
    for (const auto &it : tab.action) {
        for (const auto &it2 : it.second) {
            // cout << it.first << ", " << it2.first << " ; " << it2.second
            //      << endl;
            cout << it.first << " " << it2.first << " " << it2.second << endl;
        }
    }
    cout << "NEW STATES" << endl;
    for (const auto &it : tab.new_state) {
        for (const auto &it2 : it.second) {
            // cout << it.first << ", " << it2.first << " ; " << it2.second
            //      << endl;
            cout << it.first << " " << it2.first << " " << it2.second << endl;
        }
    }

    return 0;
}
