compile the project using this command
g++ -std=c++20 main.cpp -o main -L/opt/homebrew/lib -I/opt/homebrew/include

please note that the project uses the boost library. please change the include and linking flag according to the location of the library in your machine


the executibale generates its own graph so no input is required.
graphs genreated are stored in inp_graph.txt

run the excutible:

deafult params:
./main

custom params:
./main [number of nodes] [sparcity] [random seed] [min edge qeight] [max edge weight]

sparcity lies between 0.0 - 1.0
edge weights are postive ints


results.ipynb contains the code we used to make the graphs
