EXEC=mpi_example1
NODES=26

build: src/main.c
	mpicc src/main.c -o $(EXEC)

run: build
	mpirun -n $(NODES) --hostfile Hostfile --map-by node --oversubscribe -- $(EXEC)