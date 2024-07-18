CXX=clang++
CXXFLAGS=-std=c++17 -Wall -Wextra -Wunreachable-code -O0
LINUX_LIBS=-lX11 -lpthread 

SUPL=util.cpp builtin.cpp shell.cpp jobs.cpp YaSH_x11.cpp

all: hrry 

hrry: $(SUPL) main.cpp include/yungLog.h
	$(CXX) $(CXXFLAGS) $(LINUX_LIBS) -o $@ main.cpp $(SUPL) 

.PHONY: clean
clean:
	rm *.o output app echo
