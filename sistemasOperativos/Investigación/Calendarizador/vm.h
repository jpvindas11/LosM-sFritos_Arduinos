
#ifndef VM_H
#define VM_H

#include <stddef.h>

#define ADDRESS_SIZE   20
#define OFFSET_BITS    12
#define PAGE_NUMBER    (ADDRESS_SIZE - OFFSET_BITS)
#define NUM_PAGES      (1 << PAGE_NUMBER)
#define NUM_FRAMES     8
#define FRAME_INVALID  0xFF
#define TLB_SIZE       16

// Macros de extraccion definidos por copilot
#define ADDRESS_PAGE_NUM(address) ( (unsigned int)(((address) >> OFFSET_BITS) & ((1u << PAGE_NUMBER) - 1)) )
#define ADDRESS_OFFSET(address) ( (unsigned int)((address) & ((1u << OFFSET_BITS) - 1)) )

// flags dados por Copilot
#define PTE_VALID  (1u << 0)
#define PTE_READ   (1u << 1)
#define PTE_WRITE  (1u << 2)
#define PTE_EXEC   (1u << 3)
#define PTE_USER   (1u << 4)
#define PTE_DIRTY  (1u << 5)
#define PTE_REF    (1u << 6)
#define PTE_SHARED (1u << 7)

// Entrada de la tabla de paginas
typedef struct {
    unsigned char frame;
    unsigned char flags;
} pte_t;

// Tabla de paginas
typedef struct {
    pte_t entries[NUM_PAGES];
} page_table_t;

// Tipos de reemplazo de páginas
typedef enum {
    FIFO = 0,
    LRU = 1,
    CLOCK = 2
} replacement_policy_t;

// Metadatos (se pueden agregar mas de requerirse)
typedef struct {
    unsigned int access_time;
} page_stats_t;

// TLB
typedef struct {
    unsigned int pageNum;
    unsigned char frame;
    unsigned char flags;
} tlb_entry_t;

typedef struct {
    tlb_entry_t entries[TLB_SIZE];
    unsigned char next_index;  // indice para rr
} tlb_t;

void tlb_init(tlb_t *tlb);

unsigned char tlb_lookup(const tlb_t *tlb, unsigned int pageNum, unsigned char *out_flags);

void tlb_insert(tlb_t *tlb, unsigned int pageNum, unsigned char frame, unsigned char flags);

void tlb_flush(tlb_t *tlb);

void pt_init(page_table_t *pt);

int pt_set(page_table_t *pt, unsigned int pageNum, unsigned char frame, unsigned char flags);

int pt_get(page_table_t *pt, unsigned int pageNum, unsigned char *out_frame, unsigned char *out_flags);

int pt_is_valid(page_table_t *pt, unsigned int pageNum);

int pt_check_permissions(page_table_t *pt, unsigned int pageNum, unsigned char req_mask);

unsigned char select_frame_to_replace(replacement_policy_t policy, const page_table_t *pt);

void pt_record_access(page_table_t *pt, unsigned int pageNum);

unsigned int translate(page_table_t *pt, tlb_t *tlb, unsigned int address, unsigned char req_mask);

int vm_read_byte(page_table_t *pt, tlb_t *tlb, unsigned int address, unsigned char *out_byte);

int vm_write_byte(page_table_t *pt, tlb_t *tlb, unsigned int address, unsigned char byte);

#endif /* VM_H */
