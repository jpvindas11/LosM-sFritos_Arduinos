
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

// Entrada de la tabla de paginas
typedef struct {
    u8 frame;
    u8 valid;
} pte_t;

// Tabla de paginas
typedef struct {
    pte_t entries[NUM_PAGES];
} page_table_t;

void pt_init(page_table_t *pt);

int pt_set(page_table_t *pt, u32 vpn, u8 frame, u8 valid);

int pt_get(page_table_t *pt, u32 vpn, u8 *out_frame, u8 *out_valid);

int pt_is_valid(page_table_t *pt, u32 vpn);

#endif /* VM_H */
