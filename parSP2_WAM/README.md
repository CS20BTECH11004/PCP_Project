The parSP2_WAM file takes data of the graph from an "input.txt" file and then uses the parSP2 algorithm to find the shortest distance from vertex 0 to all other nodes.

The first line of the input file must contain the total number of nodes n.
From then on, the file must contain the weighted adjacent matrix of the graph, ie the element in W[i][j] is the length of edge from vertex i to vertex j in the graph.

The program then outputs the shortest distance from vertex 0 to all vertexes in order: 0, 1, ... n.