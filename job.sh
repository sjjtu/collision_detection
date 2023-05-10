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
# Number of nodes
#SBATCH --nodes=1
# Number of tasks per core (prevent hyperthreading)
#SBATCH --ntasks-per-core=1
#SBATCH --cpus-per-task=1
# Use Dardel's main partition
#SBATCH -p main

N=1000

srun --ntasks-per-node 1 ./a.out $N
srun --ntasks-per-node 2 ./a.out $N
srun --ntasks-per-node 4 ./a.out $N
srun --ntasks-per-node 8 ./a.out $N
srun --ntasks-per-node 16 ./a.out $N
srun --ntasks-per-node 32 ./a.out $N
srun --ntasks-per-node 64 ./a.out $N

wait