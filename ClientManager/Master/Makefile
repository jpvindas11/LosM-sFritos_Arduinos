# Configuración del compilador
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g -I. -I../Util
LDFLAGS = -pthread

# Directorios
SRC_DIR = .
UTIL_DIR = ../Util
OBJ_DIR = obj
BIN_DIR = bin

# Archivos
TARGET = $(BIN_DIR)/master
SOURCES = main.cpp MasterServer.cpp MasterEntryPoint.cpp
UTIL_SOURCES = $(UTIL_DIR)/Socket.cpp $(UTIL_DIR)/Semaphore.cpp $(UTIL_DIR)/Thread.cpp $(UTIL_DIR)/UDPSocket.cpp

# Objetos
OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)
UTIL_OBJECTS = $(UTIL_SOURCES:../Util/%.cpp=$(OBJ_DIR)/%.o)
ALL_OBJECTS = $(OBJECTS) $(UTIL_OBJECTS)

# Incluir directorios
INCLUDES = -I$(SRC_DIR) -I$(UTIL_DIR)

# Regla principal
all: directories $(TARGET)

# Crear directorios necesarios
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Compilar ejecutable
$(TARGET): $(ALL_OBJECTS)
	@echo "Linking $@..."
	$(CXX) $(CXXFLAGS) $(ALL_OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# Compilar archivos objeto del master
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar archivos objeto de utilidades
$(OBJ_DIR)/Socket.o: $(UTIL_DIR)/Socket.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/Semaphore.o: $(UTIL_DIR)/Semaphore.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/Thread.o: $(UTIL_DIR)/Thread.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/UDPSocket.o: $(UTIL_DIR)/UDPSocket.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Limpiar archivos generados
clean:
	@echo "Cleaning..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Clean complete."

# Limpiar y recompilar
rebuild: clean all

# Ejecutar el master con parámetros por defecto
run: all
	@echo "Running master server..."
	./$(TARGET) 127.0.0.1

# Ejecutar con argumentos personalizados
run-custom: all
	@echo "Running master server with custom arguments..."
	./$(TARGET) $(ARGS)

# Ejecutar en modo local (localhost)
run-local: all
	@echo "Running master server on localhost..."
	./$(TARGET) 127.0.0.1

# Ejecutar en todas las interfaces (útil para desarrollo)
run-all: all
	@echo "Running master server on all interfaces..."
	./$(TARGET) 0.0.0.0

# Mostrar ayuda
help:
	@echo "Makefile targets:"
	@echo "  all         - Build the master server (default)"
	@echo "  clean       - Remove generated files"
	@echo "  rebuild     - Clean and build"
	@echo "  run         - Build and run with default parameters (127.0.0.1)"
	@echo "  run-local   - Build and run on localhost"
	@echo "  run-all     - Build and run on all interfaces (0.0.0.0)"
	@echo "  run-custom  - Build and run with custom ARGS"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "Server ports:"
	@echo "  Users:   14010"
	@echo "  Auth:    14020"
	@echo "  Arduino: 14030"
	@echo "  Storage: 14040"
	@echo ""
	@echo "Usage examples:"
	@echo "  make run                              # Run on localhost"
	@echo "  make run-custom ARGS=\"192.168.1.50\" # Run on specific IP"
	@echo "  make run-all                          # Run on all interfaces"

# Instalar (opcional)
install: all
	@echo "Installing master server to /usr/local/bin..."
	sudo cp $(TARGET) /usr/local/bin/master-server
	@echo "Installation complete. Run with: master-server"

# Desinstalar (opcional)
uninstall:
	@echo "Uninstalling master server..."
	sudo rm -f /usr/local/bin/master-server
	@echo "Uninstall complete."

# Verificar puertos disponibles
check-ports:
	@echo "Checking if master server ports are available..."
	@echo "Users port (14010):"
	@-lsof -i :14010 || echo "  Port 14010 is free"
	@echo "Auth port (14020):"
	@-lsof -i :14020 || echo "  Port 14020 is free"
	@echo "Arduino port (14030):"
	@-lsof -i :14030 || echo "  Port 14030 is free"
	@echo "Storage port (14040):"
	@-lsof -i :14040 || echo "  Port 14040 is free"

.PHONY: all directories clean rebuild run run-custom run-local run-all help install uninstall check-ports