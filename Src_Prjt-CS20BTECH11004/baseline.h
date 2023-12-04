#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <queue>

namespace Baseline
{

    const int INF = std::numeric_limits<int>::max();

    std::vector<int> run(std::vector<std::vector<int>> &graph, int source = 0)
    {
        int size = graph.size();
        std::vector<int> distance(size, INF);
        std::vector<bool> visited(size, false);

        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
        distance[source] = 0;
        pq.push({0, source});

        while (!pq.empty())
        {
            int u = pq.top().second;
            pq.pop();
            if (visited[u])
                continue;
            visited[u] = true;
            for (int v = 0; v < size; ++v)
            {
                if (!visited[v] && graph[u][v] && distance[u] != INF && distance[u] + graph[u][v] < distance[v])
                {
                    distance[v] = distance[u] + graph[u][v];
                    pq.push({distance[v], v});
                }
            }
        }
        return distance;
    }

};