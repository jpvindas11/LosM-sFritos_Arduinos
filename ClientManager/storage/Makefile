# === Configuración general ===
CXX := g++
CXXFLAGS := -Wall -Wextra -g -std=c++17 \
    -I. -I../Util -I../FileSystem/src -I../Datagrams
LDFLAGS := 
TARGET := bin/storage

# === Carpetas ===
SRC_DIR := .
BUILD_DIR := build
BIN_DIR := bin

# === Fuentes principales (de esta carpeta) ===
STORAGE_SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# === Dependencias externas ===
UTIL_SRCS := $(wildcard ../Util/*.cpp)
FILESYS_SRCS := $(wildcard ../FileSystem/src/*.cpp)

# === Todos los fuentes a compilar ===
SRCS := $(STORAGE_SRCS) $(UTIL_SRCS) $(FILESYS_SRCS)

# === Archivos objeto ===
OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(notdir $(SRCS)))

# === Regla principal ===
all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)
	@echo "✅ Build completo: $@"

# === Compilación de objetos ===
# Objetos de storage
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Objetos de Util
$(BUILD_DIR)/%.o: ../Util/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Objetos de FileSystem
$(BUILD_DIR)/%.o: ../FileSystem/src/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Ejecución ===
run: all
	@echo "🚀 Ejecutando programa..."
	@./$(TARGET)

# === Limpieza ===
clean:
	@echo "🧹 Limpiando archivos generados..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all run clean
