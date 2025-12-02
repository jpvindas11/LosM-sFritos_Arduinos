
#include "vm.h"

#include <string.h>

void pt_init(page_table_t *pt) {
    if (!pt) return;
    for (u32 i = 0; i < NUM_VPAGES; i++) {
        pt->entries[i].frame = FRAME_INVALID;
        pt->entries[i].valid = 0;
    }
}

int pt_set(page_table_t *pt, u32 vpn, u8 frame, u8 valid) {
    if (!pt) return -1;
    if (vpn >= NUM_VPAGES) return -1;
    if (valid && frame >= NUM_FRAMES) return -1;
    pt->entries[vpn].frame = frame;
    pt->entries[vpn].valid = valid ? 1 : 0;
    return 0;
}

int pt_get(page_table_t *pt, u32 vpn, u8 *out_frame, u8 *out_valid) {
    if (!pt) return -1;
    if (vpn >= NUM_VPAGES) return -1;
    if (out_frame) *out_frame = pt->entries[vpn].frame;
    if (out_valid) *out_valid = pt->entries[vpn].valid;
    return 0;
}

int pt_is_valid(page_table_t *pt, u32 vpn) {
    if (!pt) return 0;
    if (vpn >= NUM_VPAGES) return 0;
    return pt->entries[vpn].valid ? 1 : 0;
}
