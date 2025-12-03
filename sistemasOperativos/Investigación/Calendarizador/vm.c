
#include "vm.h"

static page_stats_t frame_stats[NUM_FRAMES];
static unsigned int fifo_counter[NUM_FRAMES];

static unsigned char phys_mem[NUM_FRAMES * 4096];

void pt_init(page_table_t *pt) {
    if (!pt) return;
    for (unsigned int i = 0; i < PAGE_NUMBER; i++) {
        pt->entries[i].frame = FRAME_INVALID;
        pt->entries[i].flags = 0;
    }
}

int pt_set(page_table_t *pt, unsigned int pageNum, unsigned char frame, unsigned char valid) {
    if (!pt) return -1;
    if (pageNum >= PAGE_NUMBER) return -1;
    if ((valid & PTE_VALID) && frame >= NUM_FRAMES) return -1;
    pt->entries[pageNum].frame = frame;
    pt->entries[pageNum].flags = valid;
    return 0;
}

int pt_get(page_table_t *pt, unsigned int pageNum, unsigned char *out_frame, unsigned char *out_valid) {
    if (!pt) return -1;
    if (pageNum >= PAGE_NUMBER) return -1;
    if (out_frame) *out_frame = pt->entries[pageNum].frame;
    if (out_valid) *out_valid = pt->entries[pageNum].flags;
    return 0;
}

int pt_is_valid(page_table_t *pt, unsigned int pageNum) {
    if (!pt) return 0;
    if (pageNum >= PAGE_NUMBER) return 0;
    return (pt->entries[pageNum].flags & PTE_VALID) ? 1 : 0;
}

unsigned char select_frame_to_replace(replacement_policy_t policy, const page_table_t *pt) {
    if (!pt) return FRAME_INVALID;

    switch (policy) {
        case FIFO:
            unsigned char fifo_frame = 0;
            for (unsigned char i = 1; i < NUM_FRAMES; i++) {
                if (fifo_counter[i] < fifo_counter[fifo_frame])
                    fifo_frame = i;
            }
            return fifo_frame;

        case LRU:
            unsigned char lru_frame = 0;
            unsigned int min_time = frame_stats[0].access_time;
            for (unsigned char i = 1; i < NUM_FRAMES; i++) {
                if (frame_stats[i].access_time < min_time) {
                    min_time = frame_stats[i].access_time;
                    lru_frame = i;
                }
            }
            return lru_frame;

        case CLOCK:
            unsigned char clock_frame = 0;
            for (unsigned char i = 1; i < NUM_FRAMES; i++) {
                if (frame_stats[i].access_time < frame_stats[clock_frame].access_time)
                    clock_frame = i;
            }
            return clock_frame;

        default:
            return FRAME_INVALID;
    }
}

void pt_record_access(page_table_t *pt, unsigned int pageNum) {
    if (!pt || pageNum >= PAGE_NUMBER) return;
    unsigned char frame = pt->entries[pageNum].frame;
    if (frame != FRAME_INVALID && frame < NUM_FRAMES) {
        frame_stats[frame].access_time = ++access_counter;
        pt->entries[pageNum].flags |= PTE_REF;
    }
}

int pt_check_permissions(page_table_t *pt, unsigned int pageNum, unsigned char req_mask) {
    if (!pt) return 0;
    if (pageNum >= PAGE_NUMBER) return 0;
    unsigned char flags = pt->entries[pageNum].flags;
    if (!(flags & PTE_VALID)) return 0;

    if ((req_mask & PTE_READ) && !(flags & PTE_READ)) return 0;
    if ((req_mask & PTE_WRITE) && !(flags & PTE_WRITE)) return 0;
    if ((req_mask & PTE_EXEC) && !(flags & PTE_EXEC)) return 0;
    if ((req_mask & PTE_USER) && !(flags & PTE_USER)) return 0;
    return 1;
}

void tlb_init(tlb_t *tlb) {
    if (!tlb) return;
    for (unsigned char i = 0; i < TLB_SIZE; i++) {
        tlb->entries[i].pageNum = 0xFFFFFFFFu;
        tlb->entries[i].frame = FRAME_INVALID;
        tlb->entries[i].flags = 0;
    }
    tlb->next_index = 0;
}

unsigned char tlb_lookup(const tlb_t *tlb, unsigned int pageNum, unsigned char *out_flags) {
    if (!tlb) return FRAME_INVALID;
    for (unsigned char i = 0; i < TLB_SIZE; i++) {
        if (tlb->entries[i].pageNum == pageNum) {
            if (out_flags) *out_flags = tlb->entries[i].flags;
            return tlb->entries[i].frame;  //hit
        }
    }
    return FRAME_INVALID;  //miss
}

void tlb_insert(tlb_t *tlb, unsigned int pageNum, unsigned char frame, unsigned char flags) {
    if (!tlb) return;
    unsigned char idx = tlb->next_index;
    tlb->entries[idx].pageNum = pageNum;
    tlb->entries[idx].frame = frame;
    tlb->entries[idx].flags = flags;
    tlb->next_index = (tlb->next_index + 1) % TLB_SIZE;  //rr
}

void tlb_flush(tlb_t *tlb) {
    if (!tlb) return;
    for (unsigned char i = 0; i < TLB_SIZE; i++) {
        tlb->entries[i].pageNum = 0xFFFFFFFFu;
        tlb->entries[i].frame = FRAME_INVALID;
        tlb->entries[i].flags = 0;
    }
    tlb->next_index = 0;
}

unsigned int translate(page_table_t *pt, tlb_t *tlb, unsigned int vaddr, unsigned char req_mask) {
    if (!pt) return 0xFFFFFFFFu;
    
    unsigned int pageNum = ADDRESS_PAGE_NUM(vaddr);
    unsigned int offset = ADDRESS_OFFSET(vaddr);

    unsigned char tlb_flags = 0;
    unsigned char frame = (tlb != NULL) ? tlb_lookup(tlb, pageNum, &tlb_flags) : FRAME_INVALID;
    
    if (frame == FRAME_INVALID) {
        if (!pt_is_valid(pt, pageNum)) return 0xFFFFFFFFu;  // page fault
        
        if (!pt_check_permissions(pt, pageNum, req_mask)) {
            return 0xFFFFFFFFu;
        }
        
        pt_get(pt, pageNum, &frame, &tlb_flags);
        
        pt_record_access(pt, pageNum);
        
        if (tlb != NULL) {
            tlb_insert(tlb, pageNum, frame, tlb_flags);
        }
    } else {
        if ((req_mask & PTE_READ) && !(tlb_flags & PTE_READ)) return 0xFFFFFFFFu;
        if ((req_mask & PTE_WRITE) && !(tlb_flags & PTE_WRITE)) return 0xFFFFFFFFu;
        if ((req_mask & PTE_EXEC) && !(tlb_flags & PTE_EXEC)) return 0xFFFFFFFFu;
        if ((req_mask & PTE_USER) && !(tlb_flags & PTE_USER)) return 0xFFFFFFFFu;
    }
    
    unsigned int phys_addr = (frame * 4096) + offset;
    return phys_addr;
}

int vm_read_byte(page_table_t *pt, tlb_t *tlb, unsigned int vaddr, unsigned char *out_byte) {
    if (!out_byte) return 1;
    
    unsigned int phys_addr = translate(pt, tlb, vaddr, PTE_READ);
    if (phys_addr == 0xFFFFFFFFu) return 1;
    
    if (phys_addr >= (NUM_FRAMES * 4096)) return 1;
    
    *out_byte = phys_mem[phys_addr];
    return 0;
}

int vm_write_byte(page_table_t *pt, tlb_t *tlb, unsigned int vaddr, unsigned char byte) {
    unsigned int phys_addr = translate(pt, tlb, vaddr, PTE_WRITE);
    if (phys_addr == 0xFFFFFFFFu) return 1;
    
    if (phys_addr >= (NUM_FRAMES * 4096)) return 1;
    
    phys_mem[phys_addr] = byte;
    unsigned char pageNum = ADDRESS_PAGE_NUM(vaddr);
    if (pt) pt->entries[pageNum].flags |= PTE_DIRTY;
    
    return 0;
}
