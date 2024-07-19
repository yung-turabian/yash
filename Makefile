CXX=clang++
CXXFLAGS=-std=c++17 -Wall -Wextra -Wunreachable-code -O0
LINUX_LIBS=-lX11 -lpthread 

SUPL=util.cpp builtin.cpp shell.cpp jobs.cpp YaSH_x11.cpp

all: hrry 

hrry: $(SUPL) hrry.cpp include/yungLog.h
	$(CXX) $(CXXFLAGS) $(LINUX_LIBS) -o $@ hrry.cpp $(SUPL)

Scanner: playground.cpp scanner.cpp
	$(CXX) $(CXXFLAGS) -o $@ playground.cpp scanner.cpp

Tokenizer: playground.cpp tokenizer.cpp
	$(CXX) $(CXXFLAGS) -o $@ playground.cpp tokenizer.cpp

.PHONY: clean
clean:
	rm *.o hrry Tokenizer Parser
