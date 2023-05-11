#! /bin/bash
make
mpirun -n 1 a.out $1
make clean

mpirun -n 2 a.out $1
make clean

mpirun -n 4 a.out $1
make clean

mpirun -n 8 a.out $1
make clean

wait