# Makefile para Arduino Simulator

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -I../Util
TARGET = arduino_simulator

# Directorios
SRC_DIR = .
UTIL_DIR = ../Util

# Archivos fuente
SOURCES = SimulatorMain.cpp \
          ArduinoSimulator.cpp \
          $(UTIL_DIR)/Socket.cpp

# Archivos objeto
OBJECTS = $(SOURCES:.cpp=.o)

# Regla principal
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "✓ Compilación exitosa: $(TARGET)"

# Regla para compilar archivos .cpp a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar archivos compilados
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "✓ Archivos temporales eliminados"

# Ejecutar el simulador
run: $(TARGET)
	./$(TARGET)

# Regla para recompilar todo
rebuild: clean all

.PHONY: all clean run rebuild