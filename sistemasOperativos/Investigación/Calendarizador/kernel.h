// kernel.h
#ifndef KERNEL_H
#define KERNEL_H

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

typedef struct pcb {
    u32 pid;
    proc_state_t state;
    u32 pc;          
    u32 burst_time;  
    u32 remaining;   
    struct pcb *next;
} pcb_t;

typedef struct {
    pcb_t *head;
    pcb_t *tail;
    u32    size;
} queue_t;

// sysalls
void sys_create_process(u32 burst);
void sys_terminate_process(u32 pid);
void sys_yield(void);
void sys_wait_io(u32 pid);
void sys_signal_io(u32 pid);

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