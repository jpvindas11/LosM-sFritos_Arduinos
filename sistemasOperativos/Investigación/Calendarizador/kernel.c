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
    delay(500000000000);
}

void sys_terminate_process(u32 pid) {
    pcb_t *p = find_pcb(pid);
    if (!p) return;
    p->state = PROC_TERMINATED;

    q_remove(&READY_Q, p);
    q_remove(&WAIT_Q, p);

    kputs("terminate PID:");
    kputu(pid);
    delay(500000000000);
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
    delay(500000000000);
}

static void run_for_quantum(pcb_t *p, u32 q) {
    for (u32 i = 0; i < q && p->remaining > 0; i++) {
        p->pc++;
        p->remaining--;
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
                delay(500000000000);
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
            delay(500000000000);
            
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
            delay(500000000000);
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
            delay(500000000000);
            
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
    delay(500000000000);
    
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
    delay(500000000000);
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
    delay(500000000000);
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
    delay(500000000000);
}

void kmain(void) {
    // Inicializa colas y tabla
    q_init(&READY_Q);
    q_init(&WAIT_Q);
    for (u32 i = 0; i < MAX_PROCS; i++) { PROC_USED[i] = 0; }
    for (u32 i = 0; i < MAX_SHM; i++) { SHM_TABLE[i].used = 0; }

    vga_clear();
    kputs("Kernel con RR e IPC");

    // Crear región de memoria compartida
    u32 shm_id = sys_shm_create();

    // Crea procesos que usarán IPC
    sys_create_process(7);   // Escritor
    sys_create_process(20);  // Lector
    sys_create_process(11);  // Proceso normal

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

    kputs("Iniciando Scheduler");
    
    // Quantum fijo de 3 unidades de tiempo
    scheduler_rr(3);

    kputs("Kernel finalizado");
    
    // Liberar recursos al finalizar
    sys_shm_detach(1);
    sys_shm_detach(2);
}