// kernel.c
#include "kernel.h"

#define MAX_PROCS 16
#define MAX_SHM 8

static pcb_t PROC_TABLE[MAX_PROCS];
static u8    PROC_USED[MAX_PROCS];
static shm_region_t SHM_TABLE[MAX_SHM];

static queue_t READY_Q;
static queue_t WAIT_Q;

static pcb_t *current = 0;
static u32 NEXT_PID = 1;
static u32 NEXT_SHM_ID = 1;

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xB8000


/* Keyboard and input functions */
#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64


// VGA texto (80x25)
static volatile u16 *VGA = (u16*)0xB8000; 
static u32 vga_row = 0, vga_col = 0; 
static u8 vga_attr = 0x0F; // blanco sobre negro

static void vga_putc(char c) {
    if (c == '\n') {
        vga_row++;
        vga_col = 0;
    } else if (c == '\r') {
        vga_col = 0; // retorno de carro
    } else {
        u32 idx = (vga_row * VGA_WIDTH) + vga_col;
        VGA[idx] = ((u16)vga_attr << 8) | (u8)c;
        vga_col++;
        if (vga_col >= VGA_WIDTH) {
            vga_col = 0;
            vga_row++;
        }
    }

    // Si llega al final de la pantalla, hacer scroll
    if (vga_row >= VGA_HEIGHT) {
        // Mover todas las filas hacia arriba
        for (u32 y = 1; y < VGA_HEIGHT; y++) {
            for (u32 x = 0; x < VGA_WIDTH; x++) {
                VGA[(y - 1) * VGA_WIDTH + x] = VGA[y * VGA_WIDTH + x];
            }
        }

        // Limpiar la última fila
        for (u32 x = 0; x < VGA_WIDTH; x++) {
            VGA[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = ((u16)vga_attr << 8) | ' ';
        }

        vga_row = VGA_HEIGHT - 1;
        vga_col = 0;
    }
}

static void delay(volatile unsigned long count) {
    while (count--) {
        u32 m = 2300000/23400000;
        __asm__ volatile("nop");
    }
}


void kputs(const char *s) { 
    while (*s) vga_putc(*s++); 
    vga_putc('\n'); 
}

void kputu(u32 v) { 
    char buf[11]; int i = 10; 
    buf[i--] = '\0'; 
    if (v == 0) { 
        buf[i] = '0'; 
        kputs(&buf[i]); 
        return; 
    }
    while (v && i >= 0) { 
        buf[i--] = '0' + (v % 10); v /= 10; 
    } 
    kputs(&buf[i+1]); 
}

static void vga_clear(void) {
    for (u32 i = 0; i < 80 * 25; i++)
        VGA[i] = ((u16)vga_attr << 8) | ' ';
    vga_row = vga_col = 0;
}

void q_init(queue_t *q) { q->head = q->tail = 0; q->size = 0; }

void q_enqueue(queue_t *q, pcb_t *p) {
    p->next = 0;
    if (!q->head) q->head = q->tail = p;
    else { q->tail->next = p; q->tail = p; }
    q->size++;
}

pcb_t* q_dequeue(queue_t *q) {
    pcb_t *h = q->head;
    if (!h) return 0;
    q->head = h->next;
    if (!q->head) q->tail = 0;
    h->next = 0;
    q->size--;
    return h;
}

void q_remove(queue_t *q, pcb_t *p) {
    if (!q->head || !p) return;
    if (q->head == p) { (void)q_dequeue(q); return; }
    pcb_t *prev = q->head;
    pcb_t *cur  = q->head->next;
    while (cur) {
        if (cur == p) {
            prev->next = cur->next;
            if (q->tail == cur) q->tail = prev;
            cur->next = 0;
            q->size--;
            return;
        }
        prev = cur; cur = cur->next;
    }
}

static pcb_t* alloc_pcb(void) {
    for (u32 i = 0; i < MAX_PROCS; i++) {
        if (!PROC_USED[i]) {
            PROC_USED[i] = 1;
            PROC_TABLE[i].shm_id = 0;
            PROC_TABLE[i].shm_addr = 0;
            PROC_TABLE[i].time_counter = 0;
            PROC_TABLE[i].pc = 0;
            PROC_TABLE[i].burst_time = 0;
            PROC_TABLE[i].remaining = 0;
            PROC_TABLE[i].func_type = 0;
            for (u32 j = 0; j < 4; j++) PROC_TABLE[i].saved_state[j] = 0;
            return &PROC_TABLE[i];
        }
    }
    return 0;
}

static pcb_t* find_pcb(u32 pid) {
    for (u32 i = 0; i < MAX_PROCS; i++) {
        if (PROC_USED[i] && PROC_TABLE[i].pid == pid) 
            return &PROC_TABLE[i];
    }
    return 0;
}

/* Forward declaration */
static void wait_enter(void);

void sys_create_process(u32 burst) {
    pcb_t *p = alloc_pcb();
    if (!p) { 
        kputs("ERROR: sin espacio para PCB"); 
        return; 
    }
    p->pid = NEXT_PID++;
    p->state = PROC_NEW;
    p->pc = 0;
    p->burst_time = burst;
    p->remaining = burst;
    p->next = 0;

    p->state = PROC_READY;
    q_enqueue(&READY_Q, p);

    kputs("create PID:");
    kputu(p->pid);
    wait_enter();
}

void sys_terminate_process(u32 pid) {
    pcb_t *p = find_pcb(pid);
    if (!p) return;
    p->state = PROC_TERMINATED;

    q_remove(&READY_Q, p);
    q_remove(&WAIT_Q, p);

    kputs("terminate PID:");
    kputu(pid);
    wait_enter();
}

void sys_kill_process(u32 pid) {
    pcb_t *p = find_pcb(pid);
    if (!p) {
        kputs("ERROR: PID no encontrado:");
        kputu(pid);
        return;
    }
    
    /* Mark as terminated */
    p->state = PROC_TERMINATED;
    
    /* Remove from any queue it's in */
    q_remove(&READY_Q, p);
    q_remove(&WAIT_Q, p);
    
    /* If it's the current running process, clear it */
    if (current == p) {
        current = 0;
    }
    
    /* Detach shared memory if attached */
    if (p->shm_id != 0) {
        sys_shm_detach(pid);
    }
    
    /* Free the PCB slot */
    for (u32 i = 0; i < MAX_PROCS; i++) {
        if (&PROC_TABLE[i] == p) {
            PROC_USED[i] = 0;
            break;
        }
    }
    
    kputs("kill PID:");
    kputu(pid);
    wait_enter();
}

void sys_yield(void) {
    if (!current) return;

    current->state = PROC_READY;
    q_enqueue(&READY_Q, current);
    current = 0;
}

void sys_wait_io(u32 pid) {
    pcb_t *p = find_pcb(pid);
    if (!p) return;

    if (p == current) 
        current = 0;
    q_remove(&READY_Q, p);
    p->state = PROC_WAITING;
    q_enqueue(&WAIT_Q, p);
}

void sys_signal_io(u32 pid) {
    pcb_t *p = find_pcb(pid);
    if (!p) return;

    q_remove(&WAIT_Q, p);
    p->state = PROC_READY;
    q_enqueue(&READY_Q, p);
}

static void dispatch(pcb_t *p) {
    current = p;
    p->state = PROC_RUNNING;
    kputs("dispatch PID:");
    kputu(p->pid);
    wait_enter();
}

static void run_for_quantum(pcb_t *p, u32 q) {
    u32 target = p->burst_time;
    for (u32 i = 0; i < q && p->remaining > 0; i++) {
        /* Track time used */
        p->time_counter++;

        switch (p->func_type) {
            case 1: /* Fibonacci */
                if (p->pc == 0) {
                    p->saved_state[0] = 0; /* a */
                    p->saved_state[1] = 1; /* b */
                    p->saved_state[2] = 0; /* result */
                }
                if (p->pc < target) {
                    if (p->pc == 0) {
                        /* first iteration returns b (fib(1)=1) */
                        p->saved_state[2] = p->saved_state[1];
                    } else {
                        u32 next = p->saved_state[0] + p->saved_state[1];
                        p->saved_state[0] = p->saved_state[1];
                        p->saved_state[1] = next;
                        p->saved_state[2] = p->saved_state[1];
                    }
                    kputs("Fibonacci result:");
                    kputu(p->saved_state[2]);
                    wait_enter();
                    p->pc++;
                    p->remaining--;
                }
                break;
            case 2: /* Factorial */
                if (p->pc == 0) {
                    p->saved_state[0] = 1; /* acc */
                    p->saved_state[1] = 1; /* multiplier */
                    p->saved_state[2] = 1; /* result */
                }
                if (p->pc < target) {
                    p->saved_state[0] = p->saved_state[0] * p->saved_state[1];
                    p->saved_state[2] = p->saved_state[0];
                    p->saved_state[1]++; /* next multiplier */
                    p->pc++;
                    p->remaining--;
                }
                kputs("Factorial result:");
                kputu(p->saved_state[2]);
                wait_enter();
                break;
            case 3: /* Square via sum of odd numbers (n*n) */
                if (p->pc == 0) {
                    p->saved_state[0] = 0; /* acc */
                    p->saved_state[1] = 1; /* next odd */
                    p->saved_state[2] = 0; /* result */
                }
                if (p->pc < target) {
                    p->saved_state[0] += p->saved_state[1];
                    p->saved_state[2] = p->saved_state[0];
                    p->saved_state[1] += 2; /* next odd */
                    p->pc++;
                    p->remaining--;
                }
                kputs("Square result:");
                kputu(p->saved_state[2]);
                wait_enter();
                break;
            default: /* generic step */
                p->pc++;
                p->remaining--;
                break;
        }
    }
}

void scheduler_rr(u32 quantum) {
    while (1) {
        if (!READY_Q.head) {
            if (WAIT_Q.head) {
                pcb_t *w = q_dequeue(&WAIT_Q);
                w->state = PROC_READY;
                q_enqueue(&READY_Q, w);
                kputs("wake PID:");
                kputu(w->pid);
                wait_enter();
            }
        }

        pcb_t *p = q_dequeue(&READY_Q);
        if (!p) continue;

        dispatch(p);
        run_for_quantum(p, quantum);

        if (p->remaining == 0) {
            p->state = PROC_TERMINATED;
            kputs("done PID:");
            kputu(p->pid);
            wait_enter();
            
            // recomendación de Copilot de liberar el slot del proceso terminado
            current = 0;

            for (u32 i = 0; i < MAX_PROCS; i++) {
                if (&PROC_TABLE[i] == p) { 
                    PROC_USED[i] = 0; 
                    break; 
                }
            }
        } else {
            p->state = PROC_READY;
            current = 0;
            q_enqueue(&READY_Q, p);
        }

        if (WAIT_Q.head) {
            pcb_t *w = q_dequeue(&WAIT_Q);
            w->state = PROC_READY;
            q_enqueue(&READY_Q, w);
            kputs("io complete PID:");
            kputu(w->pid);
            wait_enter();
        }

        int active = 0;
        for (u32 i = 0; i < MAX_PROCS; i++) 
            if (PROC_USED[i]) { 
                active = 1; 
                break; 
            }
        if (!active) {
          kputs("No more processes");
          break;
        }
    }
}

u32 sys_shm_create(void) {
    for (u32 i = 0; i < MAX_SHM; i++) {
        if (!SHM_TABLE[i].used) {
            SHM_TABLE[i].id = NEXT_SHM_ID++;
            SHM_TABLE[i].used = 1;
            SHM_TABLE[i].ref_count = 0;
            
            for (u32 j = 0; j < SHM_SIZE; j++) {
                SHM_TABLE[i].data[j] = 0;
            }
            
            kputs("shm_create ID:");
            kputu(SHM_TABLE[i].id);
            wait_enter();
            
            return SHM_TABLE[i].id;
        }
    }
    kputs("Error: sin espacio para SHM");
    return 0;
}

void* sys_shm_attach(u32 pid, u32 shm_id) {
    pcb_t *p = find_pcb(pid);
    if (!p) {
        kputs("Error: proceso no encontrado");
        return 0;
    }
    
    shm_region_t *shm = 0;
    for (u32 i = 0; i < MAX_SHM; i++) {
        if (SHM_TABLE[i].used && SHM_TABLE[i].id == shm_id) {
            shm = &SHM_TABLE[i];
            break;
        }
    }
    
    if (!shm) {
        kputs("Error: SHM no encontrada");
        return 0;
    }
    
    p->shm_id = shm_id;
    p->shm_addr = (void*)shm->data;
    shm->ref_count++;
    
    kputs("shm_attach PID:");
    kputu(pid);
    kputs("to SHM:");
    kputu(shm_id);
    wait_enter();
    
    return p->shm_addr;
}

void sys_shm_write(u32 pid, const char *msg) {
    pcb_t *p = find_pcb(pid);
    if (!p || !p->shm_addr) {
        kputs("Error: proceso sin SHM adjuntada");
        return;
    }
    
    u8 *data = (u8*)p->shm_addr;
    u32 i = 0;
    while (msg[i] && i < SHM_SIZE - 1) {
        data[i] = msg[i];
        i++;
    }
    data[i] = '\0';
    
    kputs("shm_write PID:");
    kputu(pid);
    kputs("msg:");
    kputs(msg);
    wait_enter();
}

void sys_shm_read(u32 pid, char *buf) {
    pcb_t *pcb = find_pcb(pid);
    if (!pcb || !pcb->shm_addr) {
        kputs("Error: proceso sin SHM adjuntada");
        return;
    }
    
    u8 *data = (u8*)pcb->shm_addr;
    u32 i = 0;
    while (data[i] && i < SHM_SIZE - 1) {
        buf[i] = data[i];
        i++;
    }
    buf[i] = '\0';
    
    kputs("shm_read PID:");
    kputu(pid);
    kputs("msg:");
    kputs(buf);
    wait_enter();
}

void sys_shm_detach(u32 pid) {
    pcb_t *p = find_pcb(pid);
    if (!p || !p->shm_id) return;
    
    for (u32 i = 0; i < MAX_SHM; i++) {
        if (SHM_TABLE[i].used && SHM_TABLE[i].id == p->shm_id) {
            SHM_TABLE[i].ref_count--;
            
            if (SHM_TABLE[i].ref_count == 0) {
                SHM_TABLE[i].used = 0;
                kputs("shm_destroy ID:");
                kputu(SHM_TABLE[i].id);
            }
            break;
        }
    }
    
    p->shm_id = 0;
    p->shm_addr = 0;
    
    kputs("shm_detach PID:");
    kputu(pid);
    wait_enter();
}

static u8 kgetc(void) {
    /* Wait for keyboard input */
    while (1) {
        u8 status;
        __asm__ volatile("inb $0x64, %0" : "=a"(status));
        if (status & 0x01) {  /* Data available */
            u8 scancode;
            __asm__ volatile("inb $0x60, %0" : "=a"(scancode));
            
            /* Convert scancode to ASCII (simplified) */
            if (scancode >= 0x02 && scancode <= 0x0B) {
                /* Numbers 1-9 */
                return '0' + (scancode - 0x01);
            } else if (scancode == 0x0B) {
                /* 0 key */
                return '0';
            } else if (scancode == 0x1C) {
                /* Enter key */
                return '\n';
            } else if (scancode == 0x0E) {
                /* Backspace */
                return '\b';
            }
        }
        delay(10000);
    }
}

static void kputc(char c) {
    vga_putc(c);
}

static u32 read_number(void) {
    u32 num = 0;
    char c;
    
    kputs("Ingrese un numero: ");
    
    while (1) {
        c = kgetc();
        
        if (c >= '0' && c <= '9') {
            num = num * 10 + (c - '0');
            kputc(c);  /* Echo */
        } else if (c == '\n' || c == '\r') {
            kputc('\n');
            break;
        } else if (c == '\b') {
            if (num > 0) {
                num = num / 10;
                kputs("\b \b");  /* Backspace visual */
            }
        }
    }
    
    return num;
}

static void wait_enter(void) {
    kputs("Presione Enter para continuar...");
    while (kgetc() != '\n') { }
    kputc('\n');
}

/* Menu functions */
static void list_processes(void) {
    kputs("=== PROCESOS EXISTENTES ===");
    
    int found = 0;
    for (u32 i = 0; i < MAX_PROCS; i++) {
        if (PROC_USED[i]) {
            found = 1;
            kputs("PID: ");
            kputu(PROC_TABLE[i].pid);
            kputs(" | Tipo: ");
            
            u8 ft = PROC_TABLE[i].func_type;
            if (ft == 1) {
                kputs("Fibonacci");
            } else if (ft == 2) {
                kputs("Factorial");
            } else if (ft == 3) {
                kputs("Square");
            } else {
                kputs("None");
            }
            
            kputs(" | Estado: ");
            if (PROC_TABLE[i].state == PROC_READY) {
                kputs("READY");
            } else if (PROC_TABLE[i].state == PROC_RUNNING) {
                kputs("RUNNING");
            } else if (PROC_TABLE[i].state == PROC_WAITING) {
                kputs("WAITING");
            } else if (PROC_TABLE[i].state == PROC_TERMINATED) {
                kputs("TERMINATED");
            } else {
                kputs("NEW");
            }
            
            delay(100000000);
        }
    }
    
    if (!found) {
        kputs("No hay procesos");
    }
}

static void show_menu(void) {
    kputs("");
    kputs("====== MENU PRINCIPAL ======");
    kputs("1. Ver procesos existentes");
    kputs("2. Crear nuevo proceso");
    kputs("3. Matar un proceso");
    kputs("4. Ejecutar procesos (Scheduler)");
    kputs("5. Salir");
    kputs("");
    kputs("Seleccione una opcion (1-5):");
}

static void menu_create_process(void) {
    kputs("");
    kputs("=== CREAR NUEVO PROCESO ===");
    
    u32 burst = read_number();
    
    if (burst == 0) {
        kputs("Error: burst_time debe ser mayor a 0");
        return;
    }
    
    sys_create_process(burst);
    
    /* Asignar tipo de función basado en PID */
    pcb_t *p = find_pcb(NEXT_PID - 1);
    if (p) {
        u32 pid = p->pid;
        if (pid % 3 == 1) {
            p->func_type = 1; /* Fibonacci */
            kputs("Tipo asignado: Fibonacci");
        } else if (pid % 3 == 2) {
            p->func_type = 2; /* Factorial */
            kputs("Tipo asignado: Factorial");
        } else {
            p->func_type = 3; /* Square */
            kputs("Tipo asignado: Square");
        }
    }
    wait_enter();
}

static void menu_kill_process(void) {
    kputs("");
    kputs("=== MATAR PROCESO ===");
    kputs("Procesos disponibles para matar:");
    list_processes();
    
    kputs("");
    u32 pid = read_number();
    
    if (pid == 0) {
        kputs("Operacion cancelada");
        return;
    }
    
    if (!find_pcb(pid)) {
        kputs("ERROR: PID no existe");
        return;
    }
    
    kputs("Matando PID ");
    kputu(pid);
    kputs("...");
    sys_kill_process(pid);
    wait_enter();
}

static void test_vm(void) {
    kputs("=== VM Test ===");
    pcb_t *p = find_pcb(1);
    if (!p) {
        kputs("Error: PID 1 no encontrado");
        return;
    }
    
    // Mapea pagina virtual 0 a frame 0
    pt_set(&p->pt, 0, 0, PTE_VALID | PTE_READ | PTE_WRITE | PTE_USER);
    kputs("Test 1: Mapped VPN 0 -> Frame 0");
    
    // Escribir un valor
    u32 vaddr1 = 0x100;
    u8 value_write = 25;
    u8 value_read = 0;
    
    int write_result = vm_write_byte(&p->pt, &p->tlb, vaddr1, value_write);
    kputs("Test 2: Write 25 at 0x100:");
    if (write_result == 0) {
        kputs("  SUCCESS");
        
    } else {
        kputs("  FAILED");
    }
    
    // Leer un valor
    int read_result = vm_read_byte(&p->pt, &p->tlb, vaddr1, &value_read);
    kputs("Test 3: Read from 0x100:");
    if (read_result == 0) {
        kputs("  Read SUCCESS");
    } else {
        kputs("  Read FAILED");
    }
    kputs("  Value read: ");
    kputu(value_read);
    
    // Verifica que el valor leído sea el correcto
    if (value_read == value_write) {
        kputs("Test 4: VERIFICATION OK - valores coinciden!");
    } else {
        kputs("Test 4: VERIFICATION FAILED - valores NO coinciden");
        kputs("  Expected: ");
        kputu(value_write);
        kputs("  Got: ");
        kputu(value_read);
    }
    
    // Mapea pag 1 y prueba otro valor
    pt_set(&p->pt, 1, 1, PTE_VALID | PTE_READ | PTE_WRITE | PTE_USER);
    kputs("Test 5: Mapped VPN 1 -> Frame 1");
    
    u32 vaddr2 = (1 << OFFSET_BITS) + 50;
    u8 value2_write = 100;
    u8 value2_read = 0;
    
    vm_write_byte(&p->pt, &p->tlb, vaddr2, value2_write);
    kputs("Test 6: Write 100 at different page (OFF=50)");
    
    vm_read_byte(&p->pt, &p->tlb, vaddr2, &value2_read);
    kputs("Test 7: Read from page 1:");
    kputs("  Value read: ");
    kputu(value2_read);
    
    if (value2_read == value2_write) {
        kputs("Test 8: VERIFICATION OK - second value matches!");
    } else {
        kputs("Test 8: VERIFICATION FAILED - second value mismatch");
    }
    
    kputs("=== VM Test Complete ===");
}

void kmain(void) {
    // Limpiar pantalla primero
    vga_clear();
    vga_attr = 0x0F;
    
    // Debug: mensaje de inicio - letras una por una
    vga_putc('K');
    vga_putc('E');
    vga_putc('R');
    vga_putc('N');
    vga_putc('E');
    vga_putc('L');
    vga_putc('\n');
    vga_putc('B');
    vga_putc('O');
    vga_putc('O');
    vga_putc('T');
    vga_putc('\n');
    
    // Inicializa colas y tabla
    q_init(&READY_Q);
    q_init(&WAIT_Q);
    for (u32 i = 0; i < MAX_PROCS; i++) { PROC_USED[i] = 0; }
    for (u32 i = 0; i < MAX_SHM; i++) { SHM_TABLE[i].used = 0; }

    vga_putc('I');
    vga_putc('N');
    vga_putc('I');
    vga_putc('T');
    vga_putc('\n');
    
    kputs("Kernel con RR e IPC");

    // Crear región de memoria compartida
    u32 shm_id = sys_shm_create();

    // Crear procesos iniciales
    sys_create_process(7);   // PID 1
    sys_create_process(20);  // PID 2
    sys_create_process(11);  // PID 3

    /* Assign function types to the created processes:
       PID 1 -> Fibonacci, PID 2 -> Factorial, PID 3 -> Square */
    pcb_t *p1 = find_pcb(1);
    if (p1) p1->func_type = 1; /* Fibonacci */
    pcb_t *p2 = find_pcb(2);
    if (p2) p2->func_type = 2; /* Factorial */
    pcb_t *p3 = find_pcb(3);
    if (p3) p3->func_type = 3; /* Square */

    // Adjuntar memoria compartida a procesos 1 y 2
    sys_shm_attach(1, shm_id);
    sys_shm_attach(2, shm_id);

    // PID 1 escribe un mensaje
    sys_shm_write(1, "PID 1");

    // PID 2 lee el mensaje
    char buffer[SHM_SIZE];
    sys_shm_read(2, buffer);

    // Simula que el PID 2 entra a I/O al inicio
    sys_wait_io(2);

    /* Menu principal */
    u8 running = 1;
    while (running) {
        vga_clear();
        show_menu();
        
        /* Leer opción del usuario */
        kputs("");
        u32 option = read_number();
        
        switch (option) {
            case 1:
                /* Ver procesos existentes */
                vga_clear();
                list_processes();
                kputs("");
                wait_enter();
                break;
                
            case 2:
                /* Crear nuevo proceso */
                vga_clear();
                menu_create_process();
                break;
                
            case 3:
                /* Matar un proceso */
                vga_clear();
                menu_kill_process();
                break;
                
            case 4:
                /* Ejecutar scheduler */
                vga_clear();
                kputs("Iniciando Scheduler con quantum=3");
                kputs("");
                wait_enter();
                scheduler_rr(3);
                kputs("");
                kputs("Scheduler finalizado");
                wait_enter();
                break;
                
            case 5:
                /* Salir */
                kputs("Finalizando Kernel...");
                running = 0;
                break;
                
            default:
                kputs("Opcion invalida. Intente de nuevo.");
                wait_enter();
                break;
        }
    }
    
    // Liberar recursos al finalizar
    sys_shm_detach(1);
    sys_shm_detach(2);
}
