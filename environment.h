#ifndef ENV
#define ENV

#include <unordered_map>
#include <list>
#include <vector>
#include <string>
#include <iostream>

using namespace std;


class Environment {
private:
    vector<unordered_map<string, int>> levels;
    vector<unordered_map<string, string>> type_levels;

    int search_rib(string var) {
        int idx = levels.size() - 1;
        while (idx >= 0) {
            if (levels[idx].find(var) != levels[idx].end()) {
                return idx;
            }
            idx--;
        }
        return -1;
    }

public:
    Environment() {}

    void clear() {
        levels.clear();
        type_levels.clear();
    }

    void add_level() {
        unordered_map<string, int> l;
        unordered_map<string, string> t;
        levels.push_back(l);
        type_levels.push_back(t);
    }

    void add_var(string var, int value, string type) {
        if (levels.size() == 0) {
            cout << "Environment sin niveles: no se pueden agregar variables" << endl;
            exit(0);
        }
        levels.back()[var] = value;
        type_levels.back()[var] = type;
    }

    void add_var(string var, string type) {
        levels.back()[var] = 0;
        type_levels.back()[var] = type;
    }

    bool remove_level() {
        if (levels.size() > 0) {
            levels.pop_back();
            type_levels.pop_back();
            return true;
        }
        return false;
    }


    bool update(string x, int v) {
        int idx = search_rib(x);
        if (idx < 0) return false;
        levels[idx][x] = v;
        return true;
    }


    bool check(string x) {
        int idx = search_rib(x);
        return (idx >= 0);
    }

    int lookup(string x) {
        int idx = search_rib(x);
        if (idx < 0) {
            cout << "Variable no declarada: " << x << endl;
            exit(0);
        }
        return levels[idx][x];
    }

    string lookup_type(string x) {
        int idx = search_rib(x);
        if (idx < 0) {
            cout << "Variable no declarada: " << x << endl;
            exit(0);
        }
        return type_levels[idx][x];
    }

    bool typecheck(string var, string expected_type) {
        string actual_type = lookup_type(var);
        if (actual_type != expected_type) {
            cout << "Error de tipo: se esperaba " << expected_type << " pero se encontró " << actual_type << " para la variable " << var << endl;
            return false;
        }
        return true;
    }
};

#endif