CC = clang
CC_FLAGS = -Wall -lstdc++ -std=c++11
CC_INCLUDE = -I include/ -I lib/

SOURCES = $(shell find src/*.cpp)
OBJECTS = $(patsubst src/%.cpp, build/%.o, $(SOURCES))

bin/busboy : $(OBJECTS)
	$(CC) $(CC_INCLUDE) $(CC_FLAGS) $(OBJECTS) -o $@

build/%.o : src/%.cpp include/%.hpp
	$(CC) $(CC_INCLUDE) $(CC_FLAGS) -c $< -o $@

clean :
	rm -rf bin/*
	rm -rf build/*

dbg :
	@echo SOURCES: $(SOURCES)
	@echo OBJECTS: $(OBJECTS)
