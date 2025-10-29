# Configuración del compilador
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g
LDFLAGS = -pthread

# Directorios
SRC_DIR = .
UTIL_DIR = ../Util
OBJ_DIR = obj
BIN_DIR = bin

# Archivos
TARGET = $(BIN_DIR)/proxy
SOURCES = main.cpp Proxy.cpp
UTIL_SOURCES = $(UTIL_DIR)/Socket.cpp

# Objetos
OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)
UTIL_OBJECTS = $(OBJ_DIR)/Socket.o

ALL_OBJECTS = $(OBJECTS) $(UTIL_OBJECTS)

# Incluir directorios
INCLUDES = -I$(SRC_DIR) -I$(UTIL_DIR)

# Regla principal
all: directories $(TARGET)

# Crear directorios necesarios
directories:
	@mkdir -p $(OBJ_DIR)  $(BIN_DIR)

# Compilar ejecutable
$(TARGET): $(ALL_OBJECTS)
	@echo "Linking $@..."
	$(CXX) $(CXXFLAGS) $(ALL_OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# Compilar archivos objeto del proxy
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar archivos objeto de utilidades
$(OBJ_DIR)/Socket.o: $(UTIL_DIR)/Socket.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Limpiar archivos generados
clean:
	@echo "Cleaning..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Clean complete."

# Limpiar y recompilar
rebuild: clean all

# Ejecutar el proxy con parámetros por defecto
run: all
	@echo "Running proxy..."
	./$(TARGET) 0.0.0.0 8080 127.0.0.1 9090

# Ejecutar con argumentos personalizados
run-custom: all
	@echo "Running proxy with custom arguments..."
	./$(TARGET) $(ARGS)

# Mostrar ayuda
help:
	@echo "Makefile targets:"
	@echo "  all         - Build the proxy (default)"
	@echo "  clean       - Remove generated files"
	@echo "  rebuild     - Clean and build"
	@echo "  run         - Build and run with default parameters"
	@echo "  run-custom  - Build and run with custom ARGS"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "Usage example:"
	@echo "  make run-custom ARGS=\"192.168.1.50 8080 192.168.1.100 9000\""

# Instalar (opcional)
install: all
	@echo "Installing proxy to /usr/local/bin..."
	sudo cp $(TARGET) /usr/local/bin/arduino-proxy
	@echo "Installation complete. Run with: arduino-proxy"

# Desinstalar (opcional)
uninstall:
	@echo "Uninstalling proxy..."
	sudo rm -f /usr/local/bin/arduino-proxy
	@echo "Uninstall complete."	

.PHONY: all directories clean rebuild run run-custom help install uninstall