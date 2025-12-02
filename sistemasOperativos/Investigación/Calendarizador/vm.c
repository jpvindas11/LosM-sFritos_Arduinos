
#include "vm.h"

#include <string.h>

void pt_init(page_table_t *pt) {
    if (!pt) return;
    for (u32 i = 0; i < PAGE_NUMBER; i++) {
        pt->entries[i].frame = FRAME_INVALID;
        pt->entries[i].valid = 0;
    }
}

int pt_set(page_table_t *pt, u32 vpn, u8 frame, u8 valid) {
    if (!pt) return -1;
    if (vpn >= PAGE_NUMBER) return -1;
    if (valid && frame >= NUM_FRAMES) return -1;
    pt->entries[vpn].frame = frame;
    pt->entries[vpn].valid = valid ? 1 : 0;
    return 0;
}

int pt_get(page_table_t *pt, u32 vpn, u8 *out_frame, u8 *out_valid) {
    if (!pt) return -1;
    if (vpn >= PAGE_NUMBER) return -1;
    if (out_frame) *out_frame = pt->entries[vpn].frame;
    if (out_valid) *out_valid = pt->entries[vpn].valid;
    return 0;
}

int pt_is_valid(page_table_t *pt, u32 vpn) {
    if (!pt) return 0;
    if (vpn >= PAGE_NUMBER) return 0;
    return pt->entries[vpn].valid ? 1 : 0;
}

static page_stats_t frame_stats[NUM_FRAMES];
static u32 fifo_counter[NUM_FRAMES];

u8 select_frame_to_replace(replacement_policy_t policy, const page_table_t *pt) {
    if (!pt) return FRAME_INVALID;

    switch (policy) {
        case FIFO:
            u8 fifo_frame = 0;
            for (u8 i = 1; i < NUM_FRAMES; i++) {
                if (fifo_counter[i] < fifo_counter[fifo_frame])
                    fifo_frame = i;
            }
            return fifo_frame;

        case LRU:
            u8 lru_frame = 0;
            u32 min_time = frame_stats[0].access_time;
            for (u8 i = 1; i < NUM_FRAMES; i++) {
                if (frame_stats[i].access_time < min_time) {
                    min_time = frame_stats[i].access_time;
                    lru_frame = i;
                }
            }
            return lru_frame;

        case CLOCK:
            u8 clock_frame = 0;
            for (u8 i = 1; i < NUM_FRAMES; i++) {
                if (frame_stats[i].access_time < frame_stats[clock_frame].access_time)
                    clock_frame = i;
            }
            return clock_frame;

        default:
            return FRAME_INVALID;
    }
}

void pt_record_access(page_table_t *pt, u32 vpn) {
    if (!pt || vpn >= PAGE_NUMBER) return;
    u8 frame = pt->entries[vpn].frame;
    if (frame != FRAME_INVALID && frame < NUM_FRAMES) {
        frame_stats[frame].access_time = ++access_counter;
    }
}
