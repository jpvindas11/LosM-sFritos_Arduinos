// kernel.h
#ifndef KERNEL_H
#define KERNEL_H

#include "vm.h"

typedef unsigned int  u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef enum {
    PROC_NEW = 0,
    PROC_READY,
    PROC_RUNNING,
    PROC_WAITING,
    PROC_TERMINATED
} proc_state_t;

// Estructura de memoria compartida
#define SHM_SIZE 256
typedef struct shm_region {
    u32 id;
    u8 used;
    u8 data[SHM_SIZE];
    u32 ref_count;
} shm_region_t;

typedef struct pcb {
    u32 pid;
    proc_state_t state;
    u32 pc;          
    u32 burst_time;  
    u32 remaining;   
    struct pcb *next;
    u32 shm_id;
    void *shm_addr;
    /* Per-process simulation context */
    u32 time_counter;     /* increments each iteration to track time used */
    u32 saved_state[4];   /* generic storage for last operands/results */
    u8  func_type;        /* 0=none,1=fib,2=fact,3=square */
} pcb_t;

typedef struct {
    pcb_t *head;
    pcb_t *tail;
    u32    size;
} queue_t;

// sysalls
void sys_create_process(u32 burst);
void sys_terminate_process(u32 pid);
void sys_kill_process(u32 pid);
void sys_yield(void);
void sys_wait_io(u32 pid);
void sys_signal_io(u32 pid);

// IPC - Memoria Compartida
u32 sys_shm_create(void);
void* sys_shm_attach(u32 pid, u32 shm_id);
void sys_shm_write(u32 pid, const char *msg);
void sys_shm_read(u32 pid, char *buf);
void sys_shm_detach(u32 pid);

void scheduler_rr(u32 quantum);

// Queue
void q_init(queue_t *q);
void q_enqueue(queue_t *q, pcb_t *p);
pcb_t* q_dequeue(queue_t *q);
void q_remove(queue_t *q, pcb_t *p);

void kmain(void);

void kputs(const char *s);
void kputu(u32 v);

#endif // KERNEL_H
