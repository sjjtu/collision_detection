#!/bin/bash -l

# This examples launches 8 MPI ranks on 2 nodes, with 4 ranks per
# node.

# time allocation
#SBATCH -A edu23.sf2568
# job name
#SBATCH -J myjob
# email notification
#SBATCH --mail-type=BEGIN,END
# 10 minutes wall-clock time will be given to this job
#SBATCH -t 00:10:00
# Number of nodes, maximum number
#SBATCH --nodes=16
# Number of tasks per core (prevent hyperthreading)
#SBATCH --ntasks-per-core=1
#SBATCH --cpus-per-task=1
# Use Dardel's main partition
#SBATCH -p main

# compile
make CC=CC

srun --nodes 1 ./a.out $1

srun --nodes 2 ./a.out $1

srun --nodes 4 ./a.out $1

srun --nodes 8 ./a.out $1

srun --nodes 16 ./a.out $1

make clean

wait
