# Makefile para Cliente
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
INCLUDES = -I../../../Util -I../../../Datagrams -I../../../Users/src -I../../../FileSystem/src

# Directorios
UTIL_DIR = ../../../Util
DGRAM_DIR = ../../../Datagrams
BUILD_DIR = build
BIN_DIR = bin

# Archivos objeto necesarios
UTIL_OBJS = $(BUILD_DIR)/Socket.o $(BUILD_DIR)/Thread.o $(BUILD_DIR)/Semaphore.o

# Target principal
TARGET = $(BIN_DIR)/client

# Regla por defecto
all: $(TARGET)

# Crear directorios si no existen
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compilar cliente
$(BUILD_DIR)/client.o: client.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar utilidades
$(BUILD_DIR)/Socket.o: $(UTIL_DIR)/Socket.cpp $(UTIL_DIR)/Socket.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/Thread.o: $(UTIL_DIR)/Thread.cpp $(UTIL_DIR)/Thread.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/Semaphore.o: $(UTIL_DIR)/Semaphore.cpp $(UTIL_DIR)/Semaphore.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Enlazar ejecutable
$(TARGET): $(BUILD_DIR)/client.o $(UTIL_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ -lpthread

# Limpiar archivos generados
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Ejecutar cliente
run: $(TARGET)
	./$(TARGET)

# Ejecutar cliente en modo proxy
run-proxy: $(TARGET)
	./$(TARGET) --proxy

# Depurar con valgrind
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

.PHONY: all clean run memcheck