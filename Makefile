all:
	gcc src/graph.c src/Dijkstra.c src/main.c src/KernighanLin.c src/output.c -o sliceIt -lm
debug:
	gcc -Wall -pedantic src/graph.c src/Dijkstra.c src/main.c src/KernighanLin.c src/output.c -o sliceIt -lm
clean:
	@[ -f ./sliceIt ] && rm sliceIt || true