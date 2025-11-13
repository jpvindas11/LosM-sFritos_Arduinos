# Makefile para Sistema Operativo de un solo sector
# [ECCI - UCR] Sistemas Operativos - 2025
# Grupo Los Más fritos

# Variables
ASM = nasm
ASMFLAGS = -f bin
QEMU = qemu-system-i386
QEMUFLAGS = -drive format=raw,file=boot.bin -nographic

# Archivos
SRC = boot.asm
BIN = boot.bin

# Regla principal: ensamblar y ejecutar
all: $(BIN) run

# Ensamblar el código
$(BIN): $(SRC)
	$(ASM) $(ASMFLAGS) $(SRC) -o $(BIN)
	@echo "Programa ensamblado: $(BIN)"

# Ejecutar en QEMU
run: $(BIN)
	@echo "Ejecutando en QEMU..."
	$(QEMU) $(QEMUFLAGS)

# Solo ensamblar (sin ejecutar)
build: $(BIN)

# Limpiar archivos generados
clean:
	rm -f $(BIN)
	@echo "Archivos limpiados"

# Recompilar desde cero
rebuild: clean all

# Mostrar ayuda
help:
	@echo "Makefile para Sistema Operativo"
	@echo ""
	@echo "Uso:"
	@echo "  make          - Ensamblar y ejecutar"
	@echo "  make build    - Solo ensamblar"
	@echo "  make run      - Solo ejecutar (requiere boot.bin)"
	@echo "  make clean    - Limpiar archivos generados"
	@echo "  make rebuild  - Limpiar y recompilar"
	@echo "  make help     - Mostrar esta ayuda"

.PHONY: all run build clean rebuild help