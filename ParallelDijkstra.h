#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <climits>

#define N 2048
#define SOURCE 1
#define MAXINT 9999999

struct Edge {
    int vertex;
    int weight;
};

void Dijkstra(int n, int source, int* lengths, std::vector<Edge>* graph, int firstvtx, int lastvtx) {
    int u;
    int udist;
    int lminpair[2];
    int gminpair[2];

    std::vector<int> marker(n, 1);

    for (int j = firstvtx; j <= lastvtx; j++) {
        lengths[j - firstvtx] = graph[source][j].weight;
    }

    if (source >= firstvtx && source <= lastvtx) {
        marker[source - firstvtx] = 0;
    }

    for (int i = 1; i < n; i++) {
        lminpair[0] = MAXINT;
        lminpair[1] = -1;
        for (int j = firstvtx; j <= lastvtx; j++) {
            if (marker[j - firstvtx] && lengths[j - firstvtx] < lminpair[0]) {
                lminpair[0] = lengths[j - firstvtx];
                lminpair[1] = j;
            }
        }

        gminpair[0] = lminpair[0];
        gminpair[1] = lminpair[1];
        for (int j = 0; j < n; j++) {
            int temp[2] = { lminpair[0], j };
            if (temp[0] < gminpair[0]) {
                gminpair[0] = temp[0];
                gminpair[1] = temp[1];
            }
        }

        udist = gminpair[0];
        u = gminpair[1];

        if (u == lminpair[1]) {
            marker[u - firstvtx] = 0;
        }

        for (int j = firstvtx; j <= lastvtx; j++) {
            if (marker[j - firstvtx] && (udist + graph[u][j].weight < lengths[j - firstvtx])) {
                lengths[j - firstvtx] = udist + graph[u][j].weight;
            }
        }
    }
}

int main() {
    int weight[N][N];
    int distance[N];
    std::vector<Edge> graph[N];

    int n = N;
    int nlocal = n;
    int npes = 1;

    int firstvtx = 0;
    int lastvtx = n - 1;

    int* localDistance = new int[nlocal];

    std::vector<std::thread> threads;
    for (int i = 0; i < npes; i++) {
        threads.emplace_back(Dijkstra, n, SOURCE, localDistance, graph, firstvtx, lastvtx);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    delete[] localDistance;

    return 0;
}
