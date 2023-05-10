CC = mpic++
CCFLAGS = -Wall -O3

out: Detector.cpp ball.cpp BallSorter.cpp main.cpp 
	$(CC) Detector.cpp ball.cpp BallSorter.cpp main.cpp 