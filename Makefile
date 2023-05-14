CC = mpic++
CCFLAGS = -Wall -O3

out: Detector.cpp ball.cpp BallSorter.cpp main.cpp 
	@echo "Compiling ..."
	@$(CC) Detector.cpp ball.cpp BallSorter.cpp main.cpp $(CCFLAGS)


clean: out.txt meta.txt
	@echo "Removing ..."
	@rm out.txt meta.txt