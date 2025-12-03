
#ifndef VM_H
#define VM_H

#include "kernel.h"

#define ADDRESS_SIZE   20
#define OFFSET_BITS    12
#define PAGE_NUMBER    (ADDRESS_SIZE - OFFSET_BITS)
#define NUM_PAGES      (1 << PAGE_NUMBER)
#define NUM_FRAMES     8
#define FRAME_INVALID  0xFF

#define ADDRESS_PAGE_NUM(address) ( (u32)(((address) >> OFFSET_BITS) & ((1u << PAGE_NUMBER) - 1)) )
#define ADDRESS_OFFSET(address) ( (u32)((address) & ((1u << OFFSET_BITS) - 1)) )

// flags para la estructura de entrada
#define PTE_VALID  (1u << 0)
#define PTE_READ   (1u << 1)
#define PTE_WRITE  (1u << 2)
#define PTE_EXEC   (1u << 3)
#define PTE_USER   (1u << 4)
#define PTE_DIRTY  (1u << 5)
#define PTE_REF    (1u << 6)

// Entrada de la tabla de paginas
typedef struct {
    u8 frame;
    u8 flags;
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

static u32 access_counter = 0;

// Metadatos
typedef struct {
    u32 access_time;
} page_stats_t;

void pt_init(page_table_t *pt);

int pt_set(page_table_t *pt, u32 vpn, u8 frame, u8 flags);

int pt_get(page_table_t *pt, u32 vpn, u8 *out_frame, u8 *out_flags);

int pt_is_valid(page_table_t *pt, u32 vpn);

int pt_check_permissions(page_table_t *pt, u32 vpn, u8 req_mask);

u8 select_frame_to_replace(replacement_policy_t policy, const page_table_t *pt);

void pt_record_access(page_table_t *pt, u32 vpn);

#endif /* VM_H */
