CFLAGS = -Wall -Wextra -march=native -O3 -pipe -g -lm

all: pth omp mpi

pth: CFLAGS += -lpthread
pth: $(basename $(wildcard ine5410_e[1-3]*.c))

omp: CFLAGS += -fopenmp
omp: $(basename $(wildcard ine5410_e4*.c))

mpi: CC = mpicc
mpi: $(basename $(wildcard ine5410_e5*.c))

clean:
	@rm $(basename $(wildcard *.c))
