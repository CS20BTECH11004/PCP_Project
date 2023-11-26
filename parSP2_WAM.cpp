#include <iostream>
#include <thread>
#include <array>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <vector>
#include <fstream>
#include "lockfreequeue.h"

/*
NOTE: check for correctness. I changes the source node to be variable. set the soure to be always 0 if errors occur.
*/
namespace parSP2{
    const int MAX_N = 2048; // max number of nodes
    const int INF = std::numeric_limits<int>::max();

    // std::atomic<int> min_dist; 
    LFQueue<int> R;
    LFQueue<std::pair<int, int>> Heap;
    std::vector<bool> fixed;
    std::atomic<int> min_dist{0};
    std::mutex mtx;
    std::vector<int> min_in_weight;
    std::array<std::atomic<int>, MAX_N> pred, distance;
    // bool fixed[MAX_N] = {false};

    void initialise(const std::vector<std::vector<int>> & weights, int source)
    {
        int n = weights.size();
        min_in_weight = std::vector<int>(n, INF);
        fixed = std::vector(n, false);
        for (int i = 0; i < n; i++)
        {
            pred[i].store(0);
            distance[i].store(INF);
            // min_in_weight[i] = INF;
            for (int j = 0; j < n; j++)
            {
                if (weights[j][i] > 0)
                {
                    ++pred[i];
                    min_in_weight[i] = std::min(min_in_weight[i], weights[j][i]);
                }
            }
        }
        distance[source] = 0;
        std::pair<int, int> p1(source, 0);
        Heap.enqueue(p1);
    }

    void processEdgeSP2(int z, int k, std::atomic<int> &min_dist, const std::vector<std::vector<int>>& weights)
    {
        bool changed = false;

        int new_dist = distance[z] + weights[z][k];
        int k_dist = distance[k];

        while (new_dist < distance[k])
        {
            if (distance[k].compare_exchange_weak(k_dist, new_dist, std::memory_order_relaxed))
            {
                changed = true;
                break;
            }

            k_dist = distance[k];
            new_dist = distance[z] + weights[z][k];
        }

        --pred[k];
        if (k_dist <= (min_dist + min_in_weight[k]) || pred[k] <= 0)
        {
            fixed[k] = true;
            R.enqueue(k);
        }
        else if (changed)
        {
            Heap.enqueue(std::pair(k, distance[k].load()));
        }
    }

    void parSP2Algo(const std::vector<std::vector<int>>& weights)
    {
        do
        {
            int z;
            if (!R.is_empty())
            {
                R.dequeue(z);
                for (int i = 0; i < MAX_N; ++i)
                {
                    if ((!fixed[i]) && (weights[z][i] > 0))
                    {
                        processEdgeSP2(z, i, std::ref(min_dist), std::ref(weights));
                    }
                }
            }

            if (R.is_empty())
            {
                if (!Heap.is_empty())
                {
                    std::pair<int, int> vertex;
                    Heap.dequeue(vertex);
                    int j = vertex.first;
                    min_dist = vertex.second;
                    if (!fixed[j])
                    {
                        fixed[j] = true;
                        R.enqueue(j);
                    }
                }
            }
        } while (!(R.is_empty() && Heap.is_empty()));
    }
    
    std::vector<int> run(const std::vector<std::vector<int>>& weights, int source = 0, int no_of_threads = 1)
    {

        int n = weights.size();
        initialise(weights, 0);
        std::vector<std::thread> threads;
        // int weights[MAX_N][MAX_N]; // element [i][j] represents edge from node i to node j with

        for (int i = 0; i < no_of_threads; ++i) threads.emplace_back(parSP2Algo, std::ref(weights));
        for (auto &t : threads) t.join();
        for (int i = 0; i < n; ++i) std::cout << distance[i] << " ";

        std::vector<int> res(n);
        for(int i=0;i<n;i++)res[i] = distance[i].load();
        return res;
    }
};

int main()
{
    int n;
    std::ifstream infile;
    infile.open("input.txt");
    infile >> n; // Input consists of number of nodes

    std::vector<std::vector<int>> weights(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i) // followed by the weighted adjacency matrix
    {
        for (int j = 0; j < n; ++j)
        {
            infile >> weights[i][j];
        }
    }

    infile.close();
    parSP2::run(weights, 0, 2);
}
