LIBS=-lsodium
FLAGS=-Wall -Wextra -g
XSTD=-std=c++17

SOURCES = src/arduinoSimulation.cpp \
          src/AuthenticationServer.cpp \
          src/main.cpp \
          src/Master.cpp \
          ../Util/Semaphore.cpp \
          ../Util/Thread.cpp \
          ../FileSystem/src/FileSystem.cpp

OBJECTS = $(SOURCES:%.cpp=build/%.o)
INCLUDES = -Isrc -I../Util -I../FileSystem/src
EXECUTABLE = bin/Users

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p bin
	g++ $(FLAGS) $(XSTD) $^ -o $@ $(LIBS)

build/%.o: %.cpp
	@mkdir -p $(dir $@)
	g++ -c $(FLAGS) $(XSTD) $(INCLUDES) $< -o $@

clean:
	rm -rf build bin

.PHONY: all clean