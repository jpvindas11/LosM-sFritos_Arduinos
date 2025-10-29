CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -I../Util -I../FileSystem/src
LDFLAGS = -pthread

TARGET = arduino_simulator
SOURCES = ArduinoSimulator.cpp SimulatorMain.cpp
UTIL_SOURCES = ../Util/Socket.cpp

OBJECTS = $(SOURCES:.cpp=.o)
UTIL_OBJECTS = $(UTIL_SOURCES:../Util/%.cpp=%.o)
ALL_OBJECTS = $(OBJECTS) $(UTIL_OBJECTS)

all: $(TARGET)

$(TARGET): $(ALL_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(ALL_OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: ../Util/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) 127.0.0.1 8080 3

clean:
	rm -f *.o $(TARGET)

.PHONY: all run clean