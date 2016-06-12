CC = gcc
CFLAGS = -Wall -Wextra -march=native -O3 -pipe -g
LIBS = -lm -lpthread

K_ARGS = 131 2 9 2.0 987

default: all compare

all:
	@$(CC) $(CFLAGS) $(LIBS) km_seq.c -o km_seq
	@$(CC) $(CFLAGS) $(LIBS) km_pth.c -o km_pth

compare: SHELL := /bin/bash
compare:
	@diff <(./km_seq $(K_ARGS)) <(./km_pth $(K_ARGS) $(shell nproc))
