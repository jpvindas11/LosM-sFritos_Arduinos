# Makefile para Servidor
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
INCLUDES = -I../../../Util -I../../../Datagrams -I../../../Users/src -I../../../FileSystem/src

# Directorios
UTIL_DIR = ../../../Util
USERS_DIR = ../../../Users/src
FS_DIR = ../../../FileSystem/src
BUILD_DIR = build
BIN_DIR = bin

# Archivos objeto necesarios
UTIL_OBJS = $(BUILD_DIR)/Socket.o $(BUILD_DIR)/Thread.o $(BUILD_DIR)/Semaphore.o
USER_OBJS = $(BUILD_DIR)/AuthenticationServer.o $(BUILD_DIR)/Master.o $(BUILD_DIR)/arduinoSimulation.o
FS_OBJS = $(BUILD_DIR)/FileSystem.o

# Target principal
TARGET = $(BIN_DIR)/server

# Regla por defecto
all: $(TARGET)

# Crear directorios si no existen
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compilar servidor
$(BUILD_DIR)/server.o: server.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar utilidades
$(BUILD_DIR)/Socket.o: $(UTIL_DIR)/Socket.cpp $(UTIL_DIR)/Socket.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/Thread.o: $(UTIL_DIR)/Thread.cpp $(UTIL_DIR)/Thread.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/Semaphore.o: $(UTIL_DIR)/Semaphore.cpp $(UTIL_DIR)/Semaphore.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar módulos de usuarios
$(BUILD_DIR)/AuthenticationServer.o: $(USERS_DIR)/AuthenticationServer.cpp $(USERS_DIR)/AuthenticationServer.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/Master.o: $(USERS_DIR)/Master.cpp $(USERS_DIR)/Master.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/arduinoSimulation.o: $(USERS_DIR)/arduinoSimulation.cpp $(USERS_DIR)/arduinoSimulation.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar FileSystem
$(BUILD_DIR)/FileSystem.o: $(FS_DIR)/FileSystem.cpp $(FS_DIR)/FileSystem.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Enlazar ejecutable
$(TARGET): $(BUILD_DIR)/server.o $(UTIL_OBJS) $(USER_OBJS) $(FS_OBJS) | $(BIN_DIR)
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