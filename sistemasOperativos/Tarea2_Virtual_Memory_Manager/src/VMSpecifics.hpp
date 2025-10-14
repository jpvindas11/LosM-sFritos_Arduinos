/// @copyright Los Más Fritos - 2025

#ifndef VIRTUALMEMORY_SPECIFICS
#define VIRTUALMEMORY_SPECIFICS

#define PAGE_TABLE_SIZE 8  // Entries
#define PAGE_SIZE 1024  // Bytes
#define FRAME_SIZE 1024  // Bytes
#define NUM_FRAMES 8
#define PHYSICAL_MEMORY_SIZE (NUM_FRAMES * FRAME_SIZE)  // Bytes
#define OFFSET_MASK (PAGE_SIZE - 1)

#endif  // VIRTUALMEMORY_SPECIFICS
