all:
	gcc graph.c Dijkstra.c main.c KernighanLin.c output.c -o sliceIt
debug:
	gcc -Wall -pedantic graph.c Dijkstra.c main.c KernighanLin.c output.c -o sliceIt
clean:
	@[ -f ./sliceIt ] && rm sliceIt || true