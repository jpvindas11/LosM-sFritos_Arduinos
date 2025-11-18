# Compilador y banderas
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
INCLUDES = -I../Util -I../FileSystem/src -Isrc

# Directorios
UTIL_DIR = ../Util
FS_DIR = ../FileSystem/src
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Archivos objeto
UTIL_OBJS = $(BUILD_DIR)/Socket.o $(BUILD_DIR)/UDPSocket.o $(BUILD_DIR)/Thread.o $(BUILD_DIR)/Semaphore.o
FS_OBJS = $(BUILD_DIR)/FileSystem.o
SRC_OBJS = $(BUILD_DIR)/AuthenticationServer.o $(BUILD_DIR)/main.o

# Target principal
TARGET = $(BIN_DIR)/authserver

# Regla por defecto
all: $(TARGET)

# Crear directorios si no existen
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compilar archivos fuente del proyecto
$(BUILD_DIR)/AuthenticationServer.o: $(SRC_DIR)/AuthenticationServer.cpp $(SRC_DIR)/AuthenticationServer.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar utilidades
$(BUILD_DIR)/Socket.o: $(UTIL_DIR)/Socket.cpp $(UTIL_DIR)/Socket.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
$(BUILD_DIR)/UDPSocket.o: $(UTIL_DIR)/UDPSocket.cpp $(UTIL_DIR)/UDPSocket.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/Thread.o: $(UTIL_DIR)/Thread.cpp $(UTIL_DIR)/Thread.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/Semaphore.o: $(UTIL_DIR)/Semaphore.cpp $(UTIL_DIR)/Semaphore.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar FileSystem
$(BUILD_DIR)/FileSystem.o: $(FS_DIR)/FileSystem.cpp $(FS_DIR)/FileSystem.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Enlazar ejecutable
$(TARGET): $(SRC_OBJS) $(UTIL_OBJS) $(FS_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ -lpthread -lsodium

# Limpiar archivos generados
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Ejecutar servidor
run: $(TARGET)
	./$(TARGET)

# Depurar con valgrind
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

.PHONY: all clean run memcheck
