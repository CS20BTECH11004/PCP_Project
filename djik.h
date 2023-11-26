#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <numeric>

namespace Dijkstra{
    const int INF = std::numeric_limits<int>::max();
    void relax(int u, int v, int weight, std::vector<int>& distances, std::priority_queue<std::pair<int, int>>& pq, std::mutex& lock) {
        if (distances[u] + weight < distances[v]) {
            distances[v] = distances[u] + weight;
            std::lock_guard<std::mutex>lock_guard(lock);
            pq.push(std::make_pair(-distances[v], v));
        }
    }
    void process_node(int node, const std::vector<std::vector<int>>& graph, std::vector<int>& distances, std::priority_queue<std::pair<int, int>>& pq, std::mutex& lock) {
        for (int neighbor = 0; neighbor < graph.size(); ++neighbor) {
            int weight = graph[node][neighbor];
            if (weight > 0) {  // Check if there is a connection
                relax(node, neighbor, weight, distances, pq, lock);
            }
        }
    }
    std::vector<int> run(const std::vector<std::vector<int>>& graph, int source, int num_nodes =-1) {
        if(num_nodes<=0)num_nodes = graph.size();        

        std::vector<int> distances(num_nodes, INF);
        distances[source] = 0;

        std::priority_queue<std::pair<int, int>> pq;
        pq.push(std::make_pair(0, source));
        
        while (!pq.empty()) {
            int current_node = pq.top().second;
            pq.pop();

            std::mutex lock;
            std::vector<std::jthread> threads;
            for (int i = 0; i < num_nodes; i++) {
                threads.emplace_back(process_node, current_node, std::ref(graph), std::ref(distances), std::ref(pq), std::ref(lock));
            }
        }
        
        return distances;
    }
}