# Makefile para Proxy
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
INCLUDES = -I../../../Util -I../../../Datagrams

# Directorios
UTIL_DIR = ../../../Util
BUILD_DIR = build
BIN_DIR = bin

# Archivos objeto necesarios
UTIL_OBJS = $(BUILD_DIR)/Socket.o $(BUILD_DIR)/Thread.o $(BUILD_DIR)/Semaphore.o

# Target principal
TARGET = $(BIN_DIR)/proxy

# Regla por defecto
all: $(TARGET)

# Crear directorios si no existen
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compilar proxy
$(BUILD_DIR)/proxy.o: proxy.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar utilidades
$(BUILD_DIR)/Socket.o: $(UTIL_DIR)/Socket.cpp $(UTIL_DIR)/Socket.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/Thread.o: $(UTIL_DIR)/Thread.cpp $(UTIL_DIR)/Thread.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/Semaphore.o: $(UTIL_DIR)/Semaphore.cpp $(UTIL_DIR)/Semaphore.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Enlazar ejecutable
$(TARGET): $(BUILD_DIR)/proxy.o $(UTIL_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ -lpthread

# Limpiar archivos generados
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Ejecutar proxy
run: $(TARGET)
	./$(TARGET)

# Depurar con valgrind
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

.PHONY: all clean run memcheck
