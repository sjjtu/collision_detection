CC = mpic++
CCFLAGS = -Wall -O3

out: Detector.cpp ball.cpp BallSorter.cpp main.cpp 
	@$(CC) Detector.cpp ball.cpp BallSorter.cpp main.cpp $(CCFLAGS)


clean: out.txt meta.txt
	@rm out.txt meta.txt