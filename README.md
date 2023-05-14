## Dependencies
Make sure the have a g++ compiler (e. g. GNU) as well as a MPI library for C/C++ installed.


## Compile, run example and clean up:
- Local: run the provided bash script `job_loc.sh` with the number of balls N given as an argument

`bash job_loc.sh <N>`

- On Dardel: run the provided bash script `job.sh` using sbatch with the number of balls N given as an argument. The shell script assumes the g++ compiler command is `CC`, replace if necessary:

`sbatch job.sh <N>`

This runs the Makefile, compiles the necessary files and runs the program with the given N.

Optionally, you can choose to only compile:
g++ Detector.cpp ball.cpp BallSorter.cpp main.cpp 