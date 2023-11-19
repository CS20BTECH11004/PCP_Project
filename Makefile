AMAN_INCLUDES = -I /opt/homebrew/include -L /opt/homebrew/lib

all: dijkstra
	echo "All complete"
dijkstra:
	g++-13 ParallelDijkstra.h -o parDijk.o
parSP2:
	g++-13 -std=c++20 -Wall $(AMAN_INCLUDES) parSP2_WAM.cpp -o parSP2.o                        

clear:
	rm *.o