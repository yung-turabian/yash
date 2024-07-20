CC = clang++
CFLAGS = -Wall -std=c++17
TESTS = $(wildcard test/*.cpp)
TARGET = test_runner

all: $(TARGET)
	./test_runner

$(TARGET): $(TESTS:.cpp=.o)
	$(CC) $(CFLAGS) -o $(TARGET) $^

test/%.o: test/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) test/*.o

run: $(TARGET)
	./$(TARGET)
