# ========================
# CONFIG
# ========================
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS =

TARGET = storageVisualizer

# AHORA TODO SE GENERA EN storageVisualizer/
BIN_DIR = bin
BUILD_DIR = build
FILESYSTEM = FileSystem

# Rutas relativas desde storageVisualizer/
FILESYSTEM_DIR = ../FileSystem/src

# Fuentes
SRC = \
    main.cpp \
    Visualizer.cpp \
    $(FILESYSTEM_DIR)/FileSystem.cpp

OBJ = $(SRC:%=$(BUILD_DIR)/%.o)

# ========================
# MODOS
# ========================
sensor: CXXFLAGS += -DDISK_A
sensor: $(BIN_DIR)/$(TARGET)

user: CXXFLAGS += -DDISK_B
user: $(BIN_DIR)/$(TARGET)

# ========================
# LINK
# ========================
$(BIN_DIR)/$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# ========================
# COMPILE RULE
# ========================
$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ========================
# RUN
# ========================
run:
	$(BIN_DIR)/$(TARGET)

# ========================
# CLEAN
# ========================
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(FILESYSTEM)

.PHONY: sensor user run clean
