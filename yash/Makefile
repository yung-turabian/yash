CXX=clang++
CXXFLAGS=-std=c++11 -Wall -Wextra -Wunreachable-code -O0

all:
	$(CXX) $(CXXFLAGS) myecho.cpp -o myecho
	$(CXX) $(CXXLFAGS) execve.cpp -o execve

output: main.o
	$(CXX) main.o -o output

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

.PHONY: clean
clean:
	rm *.o output
