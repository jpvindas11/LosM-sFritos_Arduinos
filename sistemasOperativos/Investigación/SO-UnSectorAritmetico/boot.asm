; [ECCI - UCR] Sistemas Operativos - 2025
; Grupo Los Más fritos
; Sistema operativo de un solo sector
; Realiza operaciones aritméticas básicas (Suma, resta, multiplicación y división)
; Ensamble a binario: nasm -f bin boot.asm -o boot.bin
; Emule con:  qemu-system-i386 -drive format=raw,file=boot.bin -nographic

org 0x7c00

start:
    cli
    xor ax,ax

    ; Pila en SS:SP -> 0x0000:0x7C00 
    mov ss,ax
    mov sp,0x7c00
    ; DS y ES en 0 para etiquetas DS:SI = 0x0000:0x7C00.
    mov ds,ax
    mov es,ax
    sti
    call limpiarPantalla

main_loop:
    mov si, txtMenu
    call imprimirHilera
    call recibirTecla     
    cmp al, '1'
    je Suma
    cmp al, '2'
    je Resta
    cmp al, '3'
    je Multi
    cmp al, '4'
    je Divi
    cmp al, '5'
    je usuarioLimpiaPantalla
    cmp al, '6'
    je salirSO
    jmp main_loop

; -----------------------
; Opcion 1: Suma

Suma:
    mov byte [maxDigitos], 3
    call capturarDosNum
    add ax, bx
    jmp mostrarResultado

; -----------------------
; Opcion 2: Resta

Resta:
    mov byte [maxDigitos], 3
    call capturarDosNum
    sub ax, bx
    jmp mostrarResultado

; -----------------------
; Opcion 3: Multiplicacion

Multi:
    mov byte [maxDigitos], 2
    call capturarDosNum
    mul bx
    jmp mostrarResultado

; -----------------------
; Opcion 4: Division

Divi:
    mov byte [maxDigitos], 3
    call capturarDosNum
    cmp bx, 0
    je divisionPorCero
    xor dx, dx
    div bx
    jmp mostrarResultado

; -----------------------
; Opcion 5: Limpiar pantalla
; -----------------------
usuarioLimpiaPantalla:
    call limpiarPantalla
    jmp main_loop

done_option:
    mov si, txtPresione
    call imprimirHilera
    call recibirTecla   
    jmp main_loop

; -----------------------
; Opcion 6: Salir

salirSO:
    mov si, txtSalida
    call imprimirHilera
    cli
    hlt
    jmp $

; -----------------------
; Rutinas de BIOS

; Mostrar resultado de operacion
mostrarResultado:
    mov si, txtResultado
    call imprimirHilera
    call imprimirNumero
    call imprimirCRLF
    
    jmp done_option

; imprimirChar: AL = char
imprimirChar:
    pusha
    mov ah, 0x0E
    mov bh, 0
    int 0x10
    popa
    ret

; imprimirHilera: DS:SI -> zero-terminated string
imprimirHilera:
    pusha
.imprima:
    lodsb
    cmp al, 0
    je .done
    call imprimirChar
    jmp .imprima
.done:
    popa
    ret

; imprimirCRLF
imprimirCRLF:
    mov al, 0x0D
    call imprimirChar
    mov al, 0x0A
    call imprimirChar
    ret

; imprimirNumero: AX = número a imprimir
imprimirNumero:
    pusha
    mov cx, 0
    mov bx, 10
    
    ; Verifica el bit más significativo para ver la flag negativa
    test ax, 0x8000
    jz .positivo
    
    ; Manejar negativo
    push ax
    mov al, '-'
    call imprimirChar
    pop ax
    neg ax
    
.positivo:
    cmp ax, 0
    jne .dividir
    mov al, '0'
    call imprimirChar
    popa
    ret
    
.dividir:
    xor dx, dx
    div bx
    push dx
    inc cx
    cmp ax, 0
    jne .dividir
    
.imprimir:
    pop ax
    add al, '0'
    call imprimirChar
    loop .imprimir
    popa
    ret

; recibirTecla: devuelve AL = ASCII
recibirTecla:
    mov ah, 0
    int 0x16
    ret

;  leerNumero: lee hasta 3 digitos, devuelve en ax
leerNumero:
    xor bx, bx
    mov cl, [maxDigitos]
    xor ch, ch
.rn_loop:
    call recibirTecla
    cmp al, 0x0D
    je .rn_done
    cmp al, '0'
    jb .rn_loop
    cmp al, '9'
    ja .rn_loop
    push ax
    call imprimirChar
    pop ax
    sub al, '0'
    mov ah, 0
    
    ; Traducir a base 10
    push ax
    mov ax, bx
    mov dx, 10
    mul dx
    mov bx, ax
    pop ax
    add bx, ax
    
    loop .rn_loop
.rn_done:
    mov ax, bx
    ret

; Mostrar mensaje al dividir por 0
divisionPorCero:
    mov si, txtError
    call imprimirHilera
    call imprimirCRLF
    jmp done_option

; Captura dos números
capturarDosNum:
    mov si, txtNumero1
    call imprimirHilera
    call leerNumero
    push ax
    call imprimirCRLF
    
    mov si, txtNumero2
    call imprimirHilera
    call leerNumero
    mov bx, ax
    call imprimirCRLF
    
    pop ax
    ret

limpiarPantalla:
    pusha
    mov ah, 0x06
    xor al, al    
    mov bh, 0x07  
    mov ch, 0
    mov cl, 0
    mov dh, 24
    mov dl, 79
    int 0x10
    popa
    ret

; -----------------------
; Data

maxDigitos db 3

txtMenu db 0x0D,0x0A
         db "Menu",0x0D,0x0A
         db "1)Sumar",0x0D,0x0A
         db "2)Restar",0x0D,0x0A
         db "3)Multiplicar",0x0D,0x0A
         db "4)Dividir",0x0D,0x0A
         db "5)Limpiar",0x0D,0x0A
         db "6)Salir",0x0D,0x0A
         db "SO: ",0

txtSalida db 0x0D,0x0A, "SO: Fin....",0x0D,0x0A,0
txtEscriba db 0x0D,0x0A, "Escriba: ",0
txtPresione db 0x0D,0x0A, "Teclazo..",0
txtNumero1 db "Num1: ", 0
txtNumero2 db "Num2: ", 0
txtResultado db "= ", 0
txtError db "Error: Division por 0", 0

    times 510 - ($-$$) db 0
    dw 0xAA55