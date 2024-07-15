CXX=clang++
CXXFLAGS=-std=c++11 -Wall -Wextra -Wunreachable-code -O0
LIBS=-lncurses

SUPL=util.cpp builtin.cpp

all:
	$(CXX) $(CXXFLAGS) echo.cpp -o echo
	$(CXX) $(CXXFLAGS) $(LIBS) main.cpp $(SUPL) -o app

output: main.o
	$(CXX) main.o -o output

.PHONY: clean
clean:
	rm *.o output
