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

    struct VarInfo {
        int value;
        string type;
        VarInfo(int v = 0, string t = "int") : value(v), type(t) {}
    };

    list<unordered_map<string, VarInfo>> env;

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

    bool remove_level() {
        if (levels.size() > 0) {
            levels.pop_back();
            type_levels.pop_back();
            return true;
        }
        return false;
    }

    void add_var(string id, string type) {
        env.back()[id] = VarInfo(0, type);
    }

    void add_var(string id, int value, string type) {
        env.back()[id] = VarInfo(value, type);
    }

    bool update(string id, int value) {
        for (auto it = env.rbegin(); it != env.rend(); ++it) {
            if (it->find(id) != it->end()) {
                (*it)[id].value = value;
                return true;
            }
        }
        return false;
    }

    bool check(string id) {
        for (auto it = env.rbegin(); it != env.rend(); ++it) {
            if (it->find(id) != it->end()) {
                return true;
            }
        }
        return false;
    }

    int lookup(string id) {
        for (auto it = env.rbegin(); it != env.rend(); ++it) {
            if (it->find(id) != it->end()) {
                return (*it)[id].value;
            }
        }
        return 0;
    }

    string getType(string id) {
        for (auto it = env.rbegin(); it != env.rend(); ++it) {
            if (it->find(id) != it->end()) {
                return (*it)[id].type;
            }
        }
        return "int"; // tipo por defecto
    }

    bool typecheck(string var, string expected_type) {
        string actual_type = getType(var);
        if (actual_type != expected_type) {
            cout << "Error de tipo: se esperaba " << expected_type << " pero se encontró " << actual_type << " para la variable " << var << endl;
            return false;
        }
        return true;
    }

    int lookup_type(string id) {
        for (auto it = env.rbegin(); it != env.rend(); ++it) {
            if (it->find(id) != it->end()) {
                return (*it)[id].value;
            }
        }
        return 0;
    }
};

#endif
