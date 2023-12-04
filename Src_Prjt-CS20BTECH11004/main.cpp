#include "baseline.h"
#include "djik.h"
#include "delta_stepping.h"
#include "parSP2.h"
#include<unordered_set>
#include<vector>
#include<iostream>
#include <fstream>

#define GRAPH_FILE "inp_graph.txt"

int n = 50;
float sparcity = 0.5;
int seed = 0; 
int min_w = 1 ,max_w = 10;

void readArgs(int argc, char *argv[]){
    try{

        n = std::stoi(argv[1]);
        sparcity = std::stof(argv[2]);
        seed = std::stoi(argv[3]);
        min_w = std::stoi(argv[4]);
        max_w = std::stoi(argv[5]);
    }
    catch(const std::exception& e){
        std::cout<<"Usage: n sparcity seed min_w max_w \ninvalid params :( using default values\n\n";
        n = 50;
        sparcity = 0.5;
        seed = 0; 
        min_w = 1;
        max_w = 10;
    }
    srand(seed);
}
std::vector<std::vector<int>> create_graph(int n, float sparcity, int min_w, int max_w){
    std::vector<std::vector<int>> graph(n, std::vector<int>(n,0));
    int num_edges = int(n*(n-1)/2 * (1 - sparcity));
    for(int i = 0; i < num_edges; ){
        int a = rand()%n,
            b = rand()%n;
        if(a!=b && graph[a][b] == 0){
            graph[a][b] = min_w + rand() % (max_w - min_w + 1);
            graph[b][a] = graph[a][b];
            i++;
        }
    }
    return graph;
}
void save_to_file(std::vector<std::vector<int>> & graph){
    std::ofstream file(GRAPH_FILE, std::ios::trunc);
    file << graph.size() << std::endl;
    for(auto &i: graph){
        for(auto &j: i){
            file << j << " ";
        }
        file << "\n";
    }
}
int main(int argc, char *argv[]){
    readArgs(argc, argv);

    std::cout << "Graph params:\n"
              << "random seed: " <<  seed << '\n'
              << "# nodes: " << n <<'\n'
              << "sparcity: " <<sparcity << '\n'
              << "edge weight: [" << min_w << ", " << max_w << "]\n"
              << std::endl;  

    std::cout << "creating graph... ";
    auto graph = create_graph(n,sparcity, min_w, max_w);
    std::cout << "done." << std::endl;
    save_to_file(graph);
    std::cout << "input graph saved in " << GRAPH_FILE << "\n\n";
    

    std::cout << "Baseline:\t";
    auto base_start = std::chrono::high_resolution_clock::now();
    auto res0 = Baseline::run(graph);
    auto base_end = std::chrono::high_resolution_clock::now();
    for(auto &i: res0) std::cout << i << " ";
    std::cout << std::endl;

    Baseline::run(graph);

    std::cout << "dijkstra's:\t";
    auto dijk_start = std::chrono::high_resolution_clock::now();
    auto res1 = Dijkstra::run(graph);
    auto dijk_end = std::chrono::high_resolution_clock::now();
    for(auto &i: res1) std::cout << i << " ";
    std::cout << std::endl;

    std::cout << "Delta Stepping:\t";
    auto delta_start = std::chrono::high_resolution_clock::now();
    auto res2 = DeltaStepping::run(graph);
    auto delta_end = std::chrono::high_resolution_clock::now();
    for(auto &i: res2) std::cout << i << " ";
    std::cout << std::endl;
    
    
    std::cout << "parSP2:\t\t";
    auto parsp2_start = std::chrono::high_resolution_clock::now();
    auto res3 = parSP2::run(graph);
    auto parsp2_end = std::chrono::high_resolution_clock::now();
    for(auto &i: res3) std::cout << i << " ";
    std::cout << std::endl;


    std::cout<< "results:\n"
            << "baseline runtime: " << std::chrono::duration_cast<std::chrono::microseconds>(base_end - base_start).count() << '\n'
            << "dijkstra's runtime: " << std::chrono::duration_cast<std::chrono::microseconds>(dijk_end - dijk_start).count() << '\n'
            << "delta stepping runtime: " << std::chrono::duration_cast<std::chrono::microseconds>(delta_end - delta_start).count() << '\n'
            << "parSP2 runtime: " << std::chrono::duration_cast<std::chrono::microseconds>(parsp2_end - parsp2_start).count() << '\n'
            << std::endl;
}