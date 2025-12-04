CC = gcc
AS = as
LD = ld

CFLAGS = -m32 -fno-builtin -fno-stack-protector -Wall -Wextra
ASFLAGS = --32
LDFLAGS = -m elf_i386 -T linker.ld

OBJS = boot.o kernel.o vm.o
KERNEL_ELF = kernel.elf
KERNEL_ISO = kernel.iso
ISO_DIR = iso

all: $(KERNEL_ISO)

boot.o: boot.s
	@echo "▶ Compilando bootloader: boot.s"
	@$(AS) $(ASFLAGS) boot.s -o boot.o
	@echo "✓ Bootloader compilado"

kernel.o: kernel.c kernel.h
	@echo "▶ Compilando kernel: kernel.c"
	@$(CC) $(CFLAGS) -c kernel.c -o kernel.o
	@echo "✓ Kernel compilado"

vm.o: vm.c vm.h
	@echo "▶ Compilando memoria virtual: vm.c"
	@$(CC) $(CFLAGS) -c vm.c -o vm.o
	@echo "✓ Memoria virtual compilada"

$(KERNEL_ELF): $(OBJS)
	@echo "▶ Enlazando objetos..."
	@$(LD) $(LDFLAGS) $(OBJS) -o $(KERNEL_ELF)
	@echo "✓ Binario ELF creado: $(KERNEL_ELF)"

$(KERNEL_ISO): $(KERNEL_ELF)
	@echo "▶ Preparando estructura ISO..."
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_ELF) $(ISO_DIR)/boot/
	@cp grub_from_iso.cfg $(ISO_DIR)/boot/grub/grub.cfg
	@echo "▶ Generando imagen ISO..."
	@grub-mkrescue -o $(KERNEL_ISO) $(ISO_DIR)/ 2>/dev/null
	@echo "✓ ISO creada: $(KERNEL_ISO)"
	@ls -lh $(KERNEL_ISO)

run: $(KERNEL_ISO)
	@echo ""
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║   Ejecutando Kernel Los M-sFritos en QEMU                  ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Instrucciones:"
	@echo "  • Escribe números (1-5) para navegar el menú"
	@echo "  • Presiona Enter para confirmar"
	@echo "  • Para salir: Presiona Ctrl+Alt+G luego Ctrl+C en la terminal"
	@echo ""
	@qemu-system-i386 -cdrom $(KERNEL_ISO) -m 512M

compile: $(KERNEL_ISO)
	@echo ""
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║            Compilación Exitosa                             ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Archivos generados:"
	@ls -lh boot.o kernel.o vm.o $(KERNEL_ELF) $(KERNEL_ISO) 2>/dev/null | awk '{printf "  %-30s %8s\n", $$9, $$5}'
	@echo ""
	@echo "Para ejecutar: make run"
	@echo ""

clean:
	@echo "▶ Limpiando archivos compilados..."
	@rm -f *.o $(KERNEL_ELF)
	@rm -f $(KERNEL_ISO)
	@rm -rf $(ISO_DIR)
	@echo "✓ Directorio limpio"

rebuild: clean all
	@echo "✓ Reconstrucción completada"

debug:
	@echo "═══════════════════════════════════════════════════════════"
	@echo "Configuración del Makefile:"
	@echo "═══════════════════════════════════════════════════════════"
	@echo "AS:     $(AS) $(ASFLAGS)"
	@echo "CC:     $(CC) $(CFLAGS)"
	@echo "LD:     $(LD) $(LDFLAGS)"
	@echo "QEMU:   qemu-system-i386"
	@echo "═══════════════════════════════════════════════════════════"
	@file boot.o kernel.o vm.o $(KERNEL_ELF) 2>/dev/null | sed 's/^/  /'
	@echo "═══════════════════════════════════════════════════════════"

help:
	@echo ""
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║  Makefile - Kernel Los M-sFritos                           ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Targets disponibles:"
	@echo ""
	@echo "  make              Compilar kernel (default)"
	@echo "  make run          Compilar y ejecutar en QEMU"
	@echo "  make compile      Compilar sin ejecutar"
	@echo "  make clean        Limpiar archivos compilados"
	@echo "  make rebuild      Limpiar y recompilar todo"
	@echo "  make debug        Mostrar configuración y tipos de archivos"
	@echo "  make help         Mostrar esta ayuda"
	@echo ""
	@echo "Ejemplos:"
	@echo "  make              # Compilar"
	@echo "  make run          # Compilar y ejecutar"
	@echo "  make clean        # Limpiar"
	@echo ""

.PHONY: all iso run compile clean rebuild debug help