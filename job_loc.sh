#! /bin/bash
make
mpirun -n 1 a.out $1
mpirun -n 2 a.out $1
mpirun -n 4 a.out $1
mpirun -n 8 a.out $1