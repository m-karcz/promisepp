CC = g++
INCLUDES = $(wildcard include/*.hpp)
CCFLAGS = -Wall -Wextra -std=c++11

tests: $(wildcard test/*.cpp) $(INCLUDES)
	$(CC) $^ -o tests /usr/lib/libgtest.a /usr/lib/libgtest_main.a -lpthread -Iinclude $(CCFLAGS)

run_tests: tests
	./tests 
