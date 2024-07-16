CXX=clang++
CXXFLAGS=-std=c++17 -Wall -Wextra -Wunreachable-code -O0
LIBS=-lncurses -lX11

SUPL=util.cpp builtin.cpp shell.cpp jobs.cpp YaSH_x11.cpp

all:
	$(CXX) $(CXXFLAGS) echo.cpp -o echo
	$(CXX) -o app $(CXXFLAGS) $(LIBS) main.cpp $(SUPL) 

output: main.o
	$(CXX) main.o -o output

.PHONY: clean
clean:
	rm *.o output
