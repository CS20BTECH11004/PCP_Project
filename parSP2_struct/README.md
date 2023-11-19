THe parSP2_struct takes data of graph from "input.txt" and then uses parSP2 algorithm to find shortest distance from vertex 0 to all vertices.

Each line of the input contains information about an edge of the graph in the format:
src dest weight

for example, a line 1 2 5 indicates that an edge exists from vertex 1 to vertex 2 with weight 5.

The program then outputs shortest distances from vertex 0 to all other vertexes in the order 0, 1,2, ... n.