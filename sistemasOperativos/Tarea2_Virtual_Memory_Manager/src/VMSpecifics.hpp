/// @copyright Los Más Fritos - 2025

#ifndef VIRTUALMEMORY_SPECIFICS
#define VIRTUALMEMORY_SPECIFICS

#define PAGE_TABLE_SIZE 28  // Entries
#define PAGE_SIZE 28  // Bytes
#define TLB_SIZE 16  // Entries
#define FRAME_SIZE 28  // Bytes
#define NUM_FRAMES 256
#define PHYSICAL_MEMORY_SIZE (NUM_FRAMES * FRAME_SIZE)  // Bytes
#define OFFSET_MASK (PAGE_SIZE - 1)

#endif  // VIRTUALMEMORY_SPECIFICS
