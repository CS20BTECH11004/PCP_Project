#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include <limits.h>
#include <cmath>

using namespace std;

void relax(int v, int x, vector<int> &d, vector<set<int>> &B, float delta) {
    if (x < d[v]) {
        if (d[v] != INT_MAX)
            B[floor(d[v]/delta)].erase(v);
        B[floor(x / delta)].insert(v);
        d[v] = x;
    }
    cout << "Tentative distances after relaxing " << v << endl;
    cout << "[";
    for (int i: d)
        cout << i << " ";
    cout << "]" << endl;
}

bool isEmpty(vector<set<int>> &B) {
    for (set<int> s: B) {
        if (!s.empty())
            return false;
    }
    return true;
}

vector<int> delta_stepping(int n, int s, vector<vector<int>> l, float delta) {
    vector<set<pair<int, int>>> heavy(n), light(n);
    vector<set<int>> B(n);
    vector<int> d(n, INT_MAX);
    d[0] = 0;

    for (int v = 0; v < n; v++) {
        for (int w = 0; w < n; w++) {
            if (w == v || l[v][w] == 0)
                continue;
            
            pair<int, int> vw = {v, w};
            if (l[v][w] > delta)
                heavy[v].insert(vw);
            else
                light[v].insert(vw);
        }
    }

    // Print elements of the 'heavy' vector
    std::cout << "Heavy:" << std::endl;
    for (const auto& s : heavy) {
        for (const auto& element : s) {
            std::cout << "(" << element.first << ", " << element.second << ") ";
        }
        std::cout << std::endl;
    }

    // Print elements of the 'light' vector
    std::cout << "Light:" << std::endl;
    for (const auto& s : light) {
        for (const auto& element : s) {
            std::cout << "(" << element.first << ", " << element.second << ") ";
        }
        std::cout << std::endl;
    }
    
    //relax(s, -1, d, B, delta);
    B[0].insert(0);
    int i = 0;
    
    while (!isEmpty(B)){
        set<int> S;
        set<pair<int, int>> Req;

        // Print B
        for (const auto& s : B) {
            for (int element : s) {
                cout << element << " ";
            }
            cout << endl;
        }

        while (!B[i].empty()) {
            for (int v: B[i]) {
                for(pair<int, int> vw: light[v]) {
                    Req.insert(pair<int, int> {vw.second, d[v] + l[v][vw.second]});
                }
            
                // for (pair<int, int> element : Req) {
                //     cout << "(" << element.first << ", " << element.second << ") ";
                // } cout << endl;
                S.insert(v);
            }
            B[i] = set<int> {};
            
            for (pair<int, int> vx: Req) {
                relax(vx.first, vx.second, d, B, delta);
            }
        }

        for (int v: S) {
            for(pair<int, int> vw: heavy[v]) {
                Req.insert(pair<int, int> {vw.second, d[v] + l[v][vw.second]});
            }
        }

        for (pair<int, int> vx: Req) {
            relax(vx.first, vx.second, d, B, delta);
        }

        i++;
    }

    vector<int> sh(n, 0);
    for (int v = 0; v < n; v++) {
        sh[v] = d[v];
    }

    return sh;
}


void printSolution(vector<int> &d){
    cout << "Vertex \t\t\t Distance from Source" << endl;
    for (int i = 0; i < d.size(); i++)
        cout << i << " \t\t\t\t" << d[i] << endl;
}

int main() {
    // vector<vector<int>> graph = {{0, 3, 0, 5, 3, 0, 3},
    //                              {3, 0, 3, 0, 0, 0, 0},
    //                              {0, 3, 0, 1, 0, 0, 0},
    //                              {5, 0, 1, 0, 0, 0, 0},
    //                              {3, 0, 0, 0, 0, 5, 0},
    //                              {0, 0, 0, 0, 5, 0, 0},
    //                              {3, 0, 0, 0, 0, 0, 0}};

    vector<vector<int>> graph = {{0, 4, 0, 0, 8, 0, 0, 0, 0},
                                 {4, 0, 8, 0, 11, 0, 0, 0, 0},
                                 {0, 8, 0, 7, 0, 0, 4, 0, 2},
                                 {0, 0, 7, 0, 0, 0, 14, 9, 0},
                                 {8, 11, 0, 0, 0, 1, 0, 0, 7},
                                 {0, 0, 0, 0, 1, 0, 2, 0, 6},
                                 {0, 0, 4, 14, 0, 2, 0, 10, 0},
                                 {0, 0, 0, 9, 0, 0, 10, 0, 0},
                                 {0, 0, 2, 0, 7, 6, 0, 0, 0}};

    float delta = 3.0;
    vector<int> sh = delta_stepping(9, 0, graph, delta);

    printSolution(sh);
}