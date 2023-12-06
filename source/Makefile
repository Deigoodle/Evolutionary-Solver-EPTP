CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -O2

EPTP: main.o
	$(CXX) -o EPTP main.o

main.o: main.cpp
	$(CXX) -c $(CXXFLAGS) main.cpp

clean:
	rm -f EPTP main.o
