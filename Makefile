CXX=clang++
CXXFLAGS=-std=c++17 -Wall -Wextra -Wunreachable-code -O0
LIBS=-lncurses -lX11 -lpthread 

SUPL=util.cpp builtin.cpp shell.cpp jobs.cpp YaSH_x11.cpp

all: app echo

echo: echo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

app: $(SUPL) main.cpp include/yungLog.h
	$(CXX) $(CXXFLAGS) $(LIBS) -o $@ main.cpp $(SUPL) 

output: main.o
	$(CXX) -o output main.o

.PHONY: clean
clean:
	rm *.o output app echo
