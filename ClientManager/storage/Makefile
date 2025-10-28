CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
INCLUDES = -I. -I../Util -I../FileSystem/src

TARGET = bin/sensorserver
SOURCES = main.cpp SensorServer.cpp StorageServer.cpp ../Util/Socket.cpp ../FileSystem/src/FileSystem.cpp
OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS := $(addprefix obj/, $(OBJECTS))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p bin disks
	$(CXX) $(CXXFLAGS) $^ -o $@ -pthread

obj/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

obj/../%.o: ../%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: all
	@[ -f disks/sensorStorage.bin ] || dd if=/dev/zero of=disks/sensorStorage.bin bs=1M count=100 2>/dev/null
	./$(TARGET) 0.0.0.0 9000 127.0.0.1 10000

clean:
	@rm -rf obj bin disks/sensorStorage.bin

rebuild: clean all

.PHONY: all run clean rebuild