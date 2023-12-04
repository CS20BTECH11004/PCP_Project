#include <iostream>
#include <thread>
#include <array>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <vector>
#include <fstream>
#include <queue>
#include <boost/integer_traits.hpp>
#include <boost/lockfree/queue.hpp>

namespace parSP2
{

    const int N =  2000;

    struct customCompare
    { // compare based on pair.second
        bool operator()(const std::pair<int, int> &left, const std::pair<int, int> &right) const
        {
            return left.second > right.second;
        }
    };

    const int INF = std::numeric_limits<int>::max();

    std::mutex mtx;

    // std::vector<std::vector<int>> weights;
    int weights[N][N];
    int min_in_weight[N];
    std::array<std::atomic<int>, N> pred, distance;
    bool fixed[N] = {false};

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, customCompare> Heap;
    boost::lockfree::queue<int, boost::lockfree::capacity<128>> R, Q;

    void initialise()
    {
        for (int i = 0; i < N; i++)
        {
            pred[i].store(0);
            distance[i].store(INF);
            min_in_weight[i] = INF;
            for (int j = 0; j < N; j++)
            {
                if (weights[j][i] > 0)
                {
                    ++pred[i];
                    min_in_weight[i] = std::min(min_in_weight[i], weights[j][i]);
                }
            }
        }
        distance[0] = 0;
        std::pair<int, int> p1(0, 0);
        Heap.push(p1);
    }

    void processEdgeSP2(int z, int k, std::atomic<int> &min_dist)
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
        if ((k_dist <= (min_dist + min_in_weight[k])) || (pred[k] <= 0))
        {
            fixed[k] = true;
            R.push(k);
        }
        else if (changed)
        {
            Q.push(k);
        }
    }

    void parSP2Algo(std::atomic<int> &min_dist)
    {

        do
        {
            int z;
            if (!R.empty())
            {
                R.pop(z);
                for (int i = 0; i < N; ++i)
                {
                    if ((!fixed[i]) && (weights[z][i] > 0))
                    {
                        processEdgeSP2(z, i, std::ref(min_dist));
                    }
                }
                mtx.lock();
                while (!Q.empty())
                {
                    int a;
                    Q.pop(a);
                    if (!fixed[a])
                    {
                        Heap.push(std::pair<int, int>(a, distance[a].load()));
                    }
                }
                mtx.unlock();
            }

            if (R.empty())
            {
                mtx.lock();
                if (!Heap.empty())
                {
                    std::pair<int, int> vertex = Heap.top();
                    Heap.pop();
                    int j = vertex.first;
                    min_dist = vertex.second;
                    if (!fixed[j])
                    {
                        fixed[j] = true;
                        R.push(j);
                    }
                }
                mtx.unlock();
            }
        } while (!(R.empty() && Heap.empty()));
    }

    std::vector<int> run(std::vector<std::vector<int>> &graph, int source = 0)
    {
        std::vector<std::thread> threads;
        std::atomic<int> min_dist{0};
        int n, no_of_threads;
        n = graph.size();
        no_of_threads = graph.size();
        for(int i=0;i < n;i++){
            for(int j=0; j<n;j++){
                weights[i][j] = graph[i][j];
            }
        }

        initialise();

        for (int i = 0; i < no_of_threads; ++i)
        {
            threads.emplace_back(parSP2Algo, std::ref(min_dist));
        }

        for (auto &t : threads)
        {
            t.join();
        }

        std::vector<int> res(n);
        for (int i = 0; i < n; ++i)
        {
            res[i] = distance[i].load();
        }

        return res;
    }
};