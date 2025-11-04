# === Configuración general ===
CXX := g++
CXXFLAGS := -Wall -Wextra -g -std=c++17 \
    -I. -I.. -I../Util -I../FileSystem/src
LDFLAGS :=
TARGET := bin/UserLogs

# === Carpetas ===
SRC_DIR := .
GEN_DIR := genServer
UTIL_DIR := ../Util
FILESYS_DIR := ../FileSystem/src
DATAGRAMS_DIR := ../Datagrams
STORAGE_GEN_DIR := ../storage/$(GEN_DIR)

BUILD_DIR := build
BIN_DIR := bin

# === Fuentes (por carpeta) ===
STORAGE_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
GENSRV_SRCS  := $(wildcard $(STORAGE_GEN_DIR)/*.cpp)
UTIL_SRCS    := $(wildcard $(UTIL_DIR)/*.cpp)
FILESYS_SRCS := $(wildcard $(FILESYS_DIR)/*.cpp)
DATAGRAMS_SRCS := $(wildcard $(DATAGRAMS_DIR)/*.cpp)

# === Lista completa de fuentes con rutas originales ===
SRCS := $(STORAGE_SRCS) $(GENSRV_SRCS) $(UTIL_SRCS) $(FILESYS_SRCS) $(DATAGRAMS_SRCS)

# === Convertir rutas "../..." en rutas relativas "storage/..." "Util/..." para objetos ===
# Ej: ../storage/genServer/StorageServer.cpp -> storage/genServer/StorageServer.cpp
SRCS_REL := $(patsubst ../%,%,$(SRCS))

# === Archivos objeto (se crean bajo build/, preservando subdirectorios) ===
OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS_REL))

# === Regla principal ===
all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)
	@echo "✅ Build completo: $@"

# === Reglas de compilación (no entran en conflicto) ===
# local UserLogs/*.cpp -> build/*.o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ../storage/genServer/*.cpp -> build/storage/genServer/*.o
$(BUILD_DIR)/storage/$(GEN_DIR)/%.o: ../storage/$(GEN_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ../Util/*.cpp -> build/Util/*.o
$(BUILD_DIR)/Util/%.o: $(UTIL_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ../FileSystem/src/*.cpp -> build/FileSystem/src/*.o
$(BUILD_DIR)/FileSystem/src/%.o: $(FILESYS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ../Datagrams/*.cpp -> build/Datagrams/*.o (si tienes fuentes ahí)
$(BUILD_DIR)/Datagrams/%.o: $(DATAGRAMS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Ejecución (opcional) ===
run: all
	@echo "🚀 Ejecutando programa..."
	@./$(TARGET)

# === Limpieza ===
clean:
	@echo "🧹 Limpiando archivos generados..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "✨ Clean complete"

.PHONY: all run clean
