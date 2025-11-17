# Compilador
CXX = g++

# Flags
CXXFLAGS = -Wall -Wextra -O2 -std=c++11

# Librerías
LIBS = -lssh

# Archivos
SRC = ssh.cpp main.cpp FileManager.cpp
OBJ = $(SRC:.cpp=.o)
HEADERS = ssh.hpp FileManager.hpp
BIN = ssh

# Target principal
all: $(BIN)

# Enlazar
$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(BIN) $(LIBS)

# Compilar objetos
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ejecutar
run: $(BIN)
	./$(BIN)

# Limpiar
clean:
	rm -f $(BIN) $(OBJ)

# Recompilar todo
rebuild: clean all

# Targets falsos
.PHONY: all run clean rebuild
