CC = clang
CC_FLAGS = -Wall -lstdc++ -std=c++11 -g
CC_INCLUDE = -I include/ -I lib/

ENTRY   = src/Main.cpp
SOURCES = $(filter-out $(ENTRY), $(shell find src/*.cpp))
OBJECTS = $(patsubst src/%.cpp, build/%.o, $(SOURCES))
TESTS   = $(shell find test/*.cpp)

bin/busboy : $(OBJECTS) $(ENTRY)
	$(CC) $(CC_INCLUDE) $(ENTRY) $(CC_FLAGS) $(OBJECTS) -o $@

bin/busboy-tests : $(OBJECTS) $(TESTS)
	$(CC) $(CC_INCLUDE) $(CC_FLAGS) $(OBJECTS) $(TESTS) -o $@

build/Main.o : src/Main.cpp
	$(CC) $(CC_INCLUDE) $(CC_FLAGS) -c $< -o $@

build/%.o : src/%.cpp include/%.hpp
	$(CC) $(CC_INCLUDE) $(CC_FLAGS) -c $< -o $@

tests : bin/busboy-tests

clean :
	rm -rf bin/*
	rm -rf build/*

dbg :
	@echo SOURCES: $(SOURCES)
	@echo OBJECTS: $(OBJECTS)
