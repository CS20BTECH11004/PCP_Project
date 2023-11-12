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
const int INF = boost::integer_traits<int>::const_max;

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

struct VertexType
{
    int node = -1;
    int min_in_weight = INF;
    int pred = 0;
    bool fixed = false;
    std::atomic <int> distance{INF};
};

struct EdgeType
{
    int src;
    int dst;
    int weight;
};
class GraphType
{
    public:
    std::array <VertexType, N> Vertices;
    std::vector <EdgeType> EdgeRange;

    void addEdge(int src, int dst, int weight)
    {
        EdgeType temp = {src, dst, weight};
        EdgeRange.emplace_back(temp);
    }

    void min_in_weight_calc(int j, int in_wt)
    {
        ++Vertices[j].pred; 
        Vertices[j].min_in_weight = std::min(Vertices[j].min_in_weight, in_wt);
    }
};

std::mutex mtx;

int weights[N][N];        // element [i][j] represents edge from node i to node j

boost::lockfree::queue <pair, boost::lockfree::capacity<128>> Heap;

boost::lockfree::queue <int, boost::lockfree::capacity <128>> R;


void processEdgeSP2(GraphType &Graph, EdgeType Edge, std::atomic<int> &min_dist)
{
    bool changed = false;

    int z = Edge.src;
    int k = Edge.dst;
    int weight = Edge.weight;

    int new_dist = Graph.Vertices[z].distance.load() + weight;
    int k_dist = Graph.Vertices[k].distance.load();

    while(new_dist < k_dist)
    {
        if(Graph.Vertices[k].distance.compare_exchange_weak(k_dist, new_dist, std::memory_order_relaxed))
        {
            changed = true;
            break;
        }

        k_dist = Graph.Vertices[k].distance.load();
        new_dist = Graph.Vertices[z].distance.load() + weight;
    }

    --Graph.Vertices[k].pred;
    if(k_dist <= (min_dist + Graph.Vertices[k].min_in_weight) || Graph.Vertices[k].pred <= 0)
    {
        Graph.Vertices[k].fixed = true;
        R.push(k);
    }
    else if(changed)
    {
        Heap.push(pair(k, Graph.Vertices[k].distance.load()));
    }
}

void parSP2Algo(GraphType &Graph, std::atomic<int> &min_dist)
{
    do
    {
        int z;
        if(!R.empty())
        {
            R.pop(z);
            for(auto &Edge: Graph.EdgeRange)
            {
                if(Edge.src != z) {continue;}
                
                int k = Edge.dst;
                if(!Graph.Vertices[k].fixed)
                {
                    processEdgeSP2(std::ref(Graph), Edge, std::ref(min_dist));
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
                if(!Graph.Vertices[j].fixed)
                {
                    Graph.Vertices[j].fixed = true;
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
    int no_of_threads, s, d, w, max_node;
    no_of_threads = 2;
    max_node = 0;
    std::ifstream infile;
    infile.open("input.txt");

    GraphType Graph;

    while(infile >> s)              //Each line of input consists of one edge, in the format "source, destination, weight"
    {
        infile >> d;
        infile >> w;

        max_node = std::max(max_node, std::max(s, d));
        Graph.addEdge(s, d, w);
        Graph.min_in_weight_calc(d, w);

    }

    infile.close();

    Graph.Vertices[0].distance.store(0);
    pair p(0,0);
    Heap.push(p);

    for(int i = 0; i < no_of_threads; ++i)
    {
        threads.emplace_back(parSP2Algo, std::ref(Graph), std::ref(min_dist));
    }

    for(auto &t: threads)
    {
        t.join();
    }

    for(int i = 0; i <= max_node; ++i)
    {
        std::cout << Graph.Vertices[i].distance.load()  << " ";
    }
    std::cout << "\n";

    return 0;
}
