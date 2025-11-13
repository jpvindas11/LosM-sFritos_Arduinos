# Sistema Operativo de Un Sector

Sistema operativo pequeño que ejecuta operaciones aritméticas desde un solo sector de arranque.

## Características

- **Suma** de dos números de tres dígitos
- **Resta** de dos números de tres dígitos
- **Multiplicación** de dos números de dos dígitos
- **División** de dos números de tres dígitos (con detección de división por cero)
- **Limpiar pantalla**
- **Salir del sistema**

## Dependencias

### Ubuntu/Debian

```bash
sudo apt-get update
```

```bash
sudo apt-get install nasm
```

```bash
sudo apt-get install qemu-system-x86
```

## Compilación

```bash
make
```

## Comandos Makefile

- `make` o `make all` - Ensambla y ejecuta automáticamente
- `make build` - Solo ensambla (crea boot.bin)
- `make run` - Solo ejecuta (requiere boot.bin existente)
- `make clean` - Elimina archivos generados
- `make rebuild` - Limpia y recompila desde cero
- `make help` - Muestra ayuda de comandos disponibles

## Estructura del Proyecto

```
.
├── boot.asm    # Código fuente en Assembly
├── boot.bin    # Binario generado (512 bytes)
├── Makefile    # Script de compilación
└── README.md   # Este archivo
```

## Ejecución Manual

### Ensamblar
```bash
nasm -f bin boot.asm -o boot.bin
```

### Ejecutar en QEMU
```bash
qemu-system-i386 -drive format=raw,file=boot.bin -nographic
```

### Salir de QEMU
Presiona `Ctrl+A` luego `X`

## Uso del Sistema

El sistema te mostrará un menú así al iniciar:

```
Menu
1)Sumar
2)Restar
3)Multiplicar
4)Dividir
5)Limpiar
6)Salir
SO: 
```

Para cualquier operación aritmética, se pueden poner dos números, y se confirmar apretando ENTER.
Se puede limpiar la pantalla para eliminar residuos de opciones anteriores.
Además de poder salir del sistema.

## Notas Técnicas

- **Tamaño**: Exactamente 512 bytes (un sector de arranque)
- **Arquitectura**: x86 (16-bit Real Mode)
- **Boot signature**: `0xAA55` en los últimos 2 bytes
- **Dirección de carga**: `0x7C00`

## Autor

**Grupo:** Los Más Fritos  
**Curso:** Sistemas Operativos - ECCI, UCR  
**Año:** 2025