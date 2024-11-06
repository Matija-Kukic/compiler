#include "analyzerStruct.h"
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
pair<string, string> splitSemi(vector<string> &v) {
    string ret1 = "", ret2 = "";
    int bf = 0;
    for (int i = 0; i < v.size(); i++) {
        if (v[i] == ":") {
            bf = 1;
        } else if (!bf) {
            ret1 += v[i];
            if (v[i + 1] != ":") {
                ret1 += " ";
            }
        } else if (bf) {
            ret2 += v[i];
            if (i != v.size() - 1) {
                ret2 += " ";
            }
        }
    }
    return {ret1, ret2};
}
LU::LU(string sign, int row, string content)
    : sign(sign), row(row), content(content) {};
LRI::LRI(int lrs, int id, string s) : lrs(lrs), id(id), s(s) {};
