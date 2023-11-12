#include <iostream>
#include <thread>
#include <array>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <vector>
#include <fstream>
#include <boost/integer_traits.hpp>
#include <boost/lockfree/queue.hpp>


#define N 2048

class pair
{
    public:
    int first;
    int second;

    pair()
    {
        first = -1;
        second = -1;
    }

    pair(int i, int j)
    {
        first = i;
        second = j;
    }
};

const int INF = boost::integer_traits<int>::const_max;

std::mutex mtx;

int weights[N][N];        // element [i][j] represents edge from node i to node j with 
int min_in_weight[N];
std::array<std::atomic<int>, N> pred, distance;
bool fixed[N] = {false};

boost::lockfree::queue <pair, boost::lockfree::capacity<128>> Heap;

boost::lockfree::queue <int, boost::lockfree::capacity <128>> R;

void initialise()
{

    for(int i = 0; i < N; i++)
    {
        pred[i].store(0);
        distance[i].store(INF);
        min_in_weight[i] = INF;
        for(int j = 0; j < N; j++)
        {
            if(weights[j][i] > 0)
            {
                ++pred[i];
                min_in_weight[i] = std::min(min_in_weight[i], weights[j][i]);
            }
        }
    }
    distance[0] = 0;
    pair p1(0,0);
    Heap.push(p1);
}

void processEdgeSP2(int z, int k, std::atomic<int> &min_dist)
{
    bool changed = false;

    int new_dist = distance[z] + weights[z][k];
    int k_dist = distance[k];

    while(new_dist < distance[k])
    {
        if(distance[k].compare_exchange_weak(k_dist, new_dist, std::memory_order_relaxed))
        {
            changed = true;
            break;
        }

        k_dist = distance[k];
        new_dist = distance[z] + weights[z][k];
    }

    --pred[k];
    if(k_dist <= (min_dist + min_in_weight[k]) || pred[k] <= 0)
    {
        fixed[k] = true;
        R.push(k);
    }
    else if(changed)
    {
        Heap.push(pair(k, distance[k].load()));
    }
}

void parSP2Algo(std::atomic<int> &min_dist)
{
    do
    {
        int z;
        if(!R.empty())
        {
            R.pop(z);
            for(int i=0; i < N; ++i)
            {
                if( (!fixed[i]) && (weights[z][i] > 0) )
                {
                    processEdgeSP2(z, i, std::ref(min_dist));
                }
            }
        }

        if(R.empty())
        {
            if(!Heap.empty())
            {
                pair vertex;
                Heap.pop(vertex);
                int j = vertex.first;
                min_dist = vertex.second;
                if(!fixed[j])
                {
                    fixed[j] = true;
                    R.push(j);
                }

            }
        }
    } while (!( R.empty() && Heap.empty() ));
}

int main()
{
    std::vector <std::thread> threads;
    std::atomic <int> min_dist{0};
    int n, no_of_threads;
    no_of_threads = 2;
    std::ifstream infile;
    infile.open("input.txt");
    infile >> n;                                            //Input consists of number of nodes

    for(int i = 0; i < n; ++i)                              //followed by the weighted adjacency matrix
    {
        for(int j = 0; j < n; ++j)
        {
            infile >> weights[i][j];
        }
    }

    infile.close();

    initialise();

    for(int i = 0; i < no_of_threads; ++i)
    {
        threads.emplace_back(parSP2Algo, std::ref(min_dist));
    }

    for(auto &t: threads)
    {
        t.join();
    }

    for(int i = 0; i < n; ++i)
    {
        std::cout << distance[i] << " ";
    }
    std::cout << "\n";

    return 0;
}
