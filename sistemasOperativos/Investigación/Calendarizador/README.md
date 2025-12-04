# Kernel con Round Robin e IPC

Este es un kernel simple que implementa:
- Scheduler Round Robin
- IPC (Inter-Process Communication) mediante memoria compartida
- Gestión de procesos con estados (NEW, READY, RUNNING, WAITING, TERMINATED)
- Colas de procesos listos y en espera

## Dependencias

### Ubuntu/Debian

```bash
# Compilador y herramientas de 


# Emulador QEMU para el sistema operativo
sudo apt-get install -y qemu-system-x86


# mtools para crear sistemas de archivos
sudo apt-get install -y mtools

# nasm para ensamblar el bootloader
sudo apt-get install -y nasm
```


## Compilación

```bash
# Limpiar archivos anteriores
make clean

# Compilar el kernel
make


## Comandos Makefile

- `make run` - Compila, crea ISO y ejecuta en QEMU
- `make clean` - Elimina archivos objeto, kernel.elf, ISO y directorios temporales

## Estructura del Proyecto

```
Calendarizador/
├── boot.s              # Código de arranque en assembly
├── kernel.c            # Código principal del kernel
├── kernel.h            # Definiciones y declaraciones
├── linker.ld           # Script del enlazador
├── grub_from_iso.cfg   # Configuración de GRUB
├── Makefile            # Script de compilación
└── README.md           # Este archivo
```

## Ejecución

Una vez compilado, se puede ejecutar de dos formas:

### 1. Con ISO (recomendado)
```bash
make run
```

### 2. Directamente con kernel.elf
```bash
qemu-system-i386 -kernel kernel.elf
```

## Características del Kernel

### Scheduler Round Robin
- Quantum configurable (por defecto: 3 unidades)
- Gestión de cola de procesos listos (READY_Q)
- Gestión de cola de procesos en espera de I/O (WAIT_Q)

### IPC - Memoria Compartida
- `sys_shm_create()` - Crear región de memoria compartida
- `sys_shm_attach(pid, shm_id)` - Adjuntar proceso a memoria compartida
- `sys_shm_write(pid, msg)` - Escribir mensaje en memoria compartida
- `sys_shm_read(pid, buf)` - Leer mensaje de memoria compartida
- `sys_shm_detach(pid)` - Desadjuntar proceso de memoria compartida

### Syscalls de Procesos
- `sys_create_process(burst)` - Crear nuevo proceso
- `sys_terminate_process(pid)` - Terminar proceso
- `sys_yield()` - Ceder CPU voluntariamente
- `sys_wait_io(pid)` - Poner proceso en espera de I/O
- `sys_signal_io(pid)` - Despertar proceso de I/O

### Ejemplo de uso del menú

====== MENU PRINCIPAL ======

1. Ver procesos existentes

2. Crear nuevo proceso

3. Matar un proceso

4. Ejecutar procesos (Scheduler)

5. Salir

Seleccione una opcion (1-5): 2

=== CREAR NUEVO PROCESO ===

Ingrese burst time (recomendado 5-20):

Nota: Se asignara automaticamente el siguiente PID

create PID:

4

Tipo asignado: Fibonacci


## Autor

Proyecto de investigación - Sistemas Operativos

- Los Más Fritos
