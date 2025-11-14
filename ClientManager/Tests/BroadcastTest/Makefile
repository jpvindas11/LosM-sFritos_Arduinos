# Compilador y flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I../../Util
LDFLAGS = -lpthread

# Directorios
UTIL_DIR = ../../Util
OBJ_DIR = obj

# Archivos fuente de utilidades
UTIL_SOURCES = $(UTIL_DIR)/Socket.cpp $(UTIL_DIR)/UDPSocket.cpp
UTIL_OBJECTS = $(OBJ_DIR)/Socket.o $(OBJ_DIR)/UDPSocket.o

# Archivos fuente del proyecto
SERVER_SOURCE = server.cpp
CLIENT_SOURCE = client.cpp

# Ejecutables
SERVER_TARGET = server
CLIENT_TARGET = client

# Regla por defecto: compilar todo
all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Crear directorio de objetos si no existe
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Compilar objetos de utilidades
$(OBJ_DIR)/Socket.o: $(UTIL_DIR)/Socket.cpp $(UTIL_DIR)/Socket.hpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/UDPSocket.o: $(UTIL_DIR)/UDPSocket.cpp $(UTIL_DIR)/UDPSocket.hpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar servidor
$(SERVER_TARGET): $(SERVER_SOURCE) $(UTIL_OBJECTS)
	$(CXX) $(CXXFLAGS) $(SERVER_SOURCE) $(UTIL_OBJECTS) -o $(SERVER_TARGET) $(LDFLAGS)

# Compilar cliente
$(CLIENT_TARGET): $(CLIENT_SOURCE) $(UTIL_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CLIENT_SOURCE) $(UTIL_OBJECTS) -o $(CLIENT_TARGET) $(LDFLAGS)

# Limpiar archivos compilados
clean:
	rm -rf $(OBJ_DIR) $(SERVER_TARGET) $(CLIENT_TARGET)

# Limpiar y recompilar
rebuild: clean all

# Solo servidor
server: $(SERVER_TARGET)

# Solo cliente
client: $(CLIENT_TARGET)

# Ejecutar servidor
run-server: $(SERVER_TARGET)
	./$(SERVER_TARGET)

# Ejecutar cliente
run-client: $(CLIENT_TARGET)
	./$(CLIENT_TARGET)

# Reglas que no son archivos
.PHONY: all clean rebuild server client run-server run-client