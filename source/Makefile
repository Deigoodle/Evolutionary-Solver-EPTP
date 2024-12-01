CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -O2

EPTP: main.o solver.o
	$(CXX) -o EPTP main.o solver.o

main.o: main.cpp solver.h
	$(CXX) -c $(CXXFLAGS) main.cpp

solver.o: solver.cpp solver.h
	$(CXX) -c $(CXXFLAGS) solver.cpp

clean:
	rm -f EPTP main.o solver.o
