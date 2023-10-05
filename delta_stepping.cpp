#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include <limits.h>
#include <cmath>

using namespace std;

int delta_stepping(int n, int s, vector<vector<int>> l, float delta) {
    vector<set<pair<int, int>>> heavy, light;
    vector<set<int>> B;
    vector<int> d;

    for (int v = 0; v < n; v++) {
        for (int w = 0; w < n; w++) {
            if (w == v)
                continue;
            
            pair<int, int> vw = {v, w};
            if (l[v][w] > delta)
                heavy[v].insert(vw);
            else
                light[v].insert(vw);

            d[v] = INT_MAX;
        }
    }
    
    relax(s, 0, d, B, delta);
    int i = 0;
    
    while (!B.empty()) {
        set<int> S;
        set<pair<int, int>> Req;

        while (!B[i].empty()) {
            for (int v: B[i]) {
                for(pair<int, int> vw: light[v]) {
                    Req.insert(pair<int, int> {vw.second, d[v] + l[v][vw.second]});
                }
                S.insert(v);
                B[i] = set<int> {};
            }

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

}

void relax(int v, int x, vector<int> &d, vector<set<int>> &B, float delta) {
    if (x < d[v]) {
        B[floor(d[v]/delta)].erase(v);
        B[floor(x / delta)].insert(v);
        d[v] = x;
    }
}