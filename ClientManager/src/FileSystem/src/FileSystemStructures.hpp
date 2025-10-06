/// @copyright Los Más Fritos - 2025

#ifndef FS_STRUCTURES
#define FS_STRUCTURES

#include <string>
#include <cmath>
#include <inttypes.h>

#define BLOCK_SIZE 256
#define TOTAL_BLOCK 65536  // 2^16 = direcciones de 16 bits
#define DISK_SIZE (TOTAL_BLOCK * BLOCK_SIZE)  // 16 MB
#define TOTAL_I_NODES 512
#define TOTAL_DIRECT_POINTERS 12
#define INODE_FREE_SLOT 0xFFFF    // Para uint16_t (iNodePointer)
#define BLOCK_FREE_SLOT 0xFFFFFFFF // Para uint32_t (bloques)
#define FILE_NAME_LENGTH 28

// Helper macro para calcular bloques necesarios
#define BLOCKS_NEEDED(bytes) ((bytes + BLOCK_SIZE - 1) / BLOCK_SIZE)

// Bitmap de inodos: 512 bits = 64 bytes = 1 bloque
#define INODE_BITMAP_START 1
#define INODE_BITMAP_BLOCKS BLOCKS_NEEDED(TOTAL_I_NODES / 8)

// Bitmap de bloques: 65536 bits = 8192 bytes = 32 bloques
#define BLOCK_BITMAP_START (INODE_BITMAP_START + INODE_BITMAP_BLOCKS)
#define BLOCK_BITMAP_BLOCKS BLOCKS_NEEDED(TOTAL_BLOCK / 8)

// Tabla de inodos: 512 inodos × 106 bytes = 54272 bytes
#define INODE_SIZE 94
#define INODES_PER_BLOCK 2
#define INODE_TABLE_START (BLOCK_BITMAP_START + BLOCK_BITMAP_BLOCKS)
#define INODE_TABLE_BLOCKS (TOTAL_I_NODES / INODES_PER_BLOCK)

// Directorio de archivos
#define FILE_DIRECTORY_START (INODE_TABLE_START + INODE_TABLE_BLOCKS)
#define FILE_ENTRY_SIZE 32
#define FILE_DIRECTORY_BLOCKS BLOCKS_NEEDED(TOTAL_I_NODES * FILE_ENTRY_SIZE)
#define MAX_FILES TOTAL_I_NODES

// Bloques de datos: todo lo que sobra
#define DATA_BLOCKS_START (FILE_DIRECTORY_START + FILE_DIRECTORY_BLOCKS)
#define TOTAL_DATA_BLOCKS (TOTAL_BLOCK - DATA_BLOCKS_START)

// Constantes para cálculos de indirección
#define POINTERS_PER_BLOCK (BLOCK_SIZE / sizeof(uint32_t))  // 64 punteros por bloque

// Rangos de bloques:
// Directos: 0-11 (12 bloques)
// Indirecto simple: 12-75 (64 bloques)
// Indirecto doble: 76-4171 (64*64 = 4096 bloques)
#define MAX_DIRECT_BLOCKS TOTAL_DIRECT_POINTERS  // 12
#define MAX_SINGLE_INDIRECT_BLOCKS (MAX_DIRECT_BLOCKS + POINTERS_PER_BLOCK)  // 76
#define MAX_DOUBLE_INDIRECT_BLOCKS (MAX_SINGLE_INDIRECT_BLOCKS + POINTERS_PER_BLOCK * POINTERS_PER_BLOCK)  // 4172

/**
 * @brief Estructura que contiene la información presente en el bloque 0 del file system
 * 
 * Maneja metadatos para el funcionamiento del sistema
 */
typedef struct SuperBlock {
    uint16_t magicNumber = 0xAA55;
    
    // Información de bitmaps
    uint32_t inodeBitmapStart = INODE_BITMAP_START;
    uint32_t inodeBitmapBlocks = INODE_BITMAP_BLOCKS;
    uint32_t blockBitmapStart = BLOCK_BITMAP_START;
    uint32_t blockBitmapBlocks = BLOCK_BITMAP_BLOCKS;
    
    // Información de tabla de inodos
    uint32_t inodeTableStart = INODE_TABLE_START;
    uint32_t inodeTableBlocks = INODE_TABLE_BLOCKS;
    
    // Información de bloques de datos
    uint32_t dataBlocksStart = DATA_BLOCKS_START;
    uint32_t totalDataBlocks = TOTAL_DATA_BLOCKS;

    // Información de archivos
    uint32_t fileDirectoryStart = FILE_DIRECTORY_START;
    uint32_t fileDirectoryBlocks = FILE_DIRECTORY_BLOCKS;
    uint32_t maxFiles = MAX_FILES;
    
    // Contadores
    uint32_t totalInodes = TOTAL_I_NODES;
    uint32_t freeInodes = TOTAL_I_NODES;
    uint32_t totalBlocks = TOTAL_BLOCK;
    uint32_t freeBlocks = TOTAL_DATA_BLOCKS;

    // 56 bytes
} SuperBlock;

/**
 * @brief Inicializa el super bloque
 * @return El super bloque inicializado
 */
inline SuperBlock initSuperBlock() {
    SuperBlock block;

    block.magicNumber = 0xAA55;
    
    // Información de bitmaps
    block.inodeBitmapStart = INODE_BITMAP_START;
    block.inodeBitmapBlocks = INODE_BITMAP_BLOCKS;
    block.blockBitmapStart = BLOCK_BITMAP_START;
    block.blockBitmapBlocks = BLOCK_BITMAP_BLOCKS;
    
    // Información de tabla de inodos
    block.inodeTableStart = INODE_TABLE_START;
    block.inodeTableBlocks = INODE_TABLE_BLOCKS;
    
    // Información de bloques de datos
    block.dataBlocksStart = DATA_BLOCKS_START;
    block.totalDataBlocks = TOTAL_DATA_BLOCKS;

    // Información de archivos
    block.fileDirectoryStart = FILE_DIRECTORY_START;
    block.fileDirectoryBlocks = FILE_DIRECTORY_BLOCKS;
    block.maxFiles = MAX_FILES;
    
    // Contadores
    block.totalInodes = TOTAL_I_NODES;
    block.freeInodes = TOTAL_I_NODES;
    block.totalBlocks = TOTAL_BLOCK;
    block.freeBlocks = TOTAL_DATA_BLOCKS;

    return block;
}

/**
 * @brief Estructura del inodo, que contiene metadatos para archivos
 * 
 * Maneja información como permisos, usuarios, tiempos, bloques asociados, etc.
 */
typedef struct __attribute__((packed)) iNode {
    uint32_t size;
    uint16_t permissions;
    uint32_t userId;
    uint32_t groupId;

    // Usamos int64 en lugar de time_t para consistencia de tamaño
    uint64_t creationTime;
    uint64_t lastModifiedTime;
    uint64_t lastAccessTime;  

    uint32_t directBlocks[TOTAL_DIRECT_POINTERS];
    uint32_t singleIndirectPointer;
    uint32_t doubleIndirectPointer;
    // 94 bytes
} iNode;

/**
 * @brief Inicializa el inodo
 * @return El inodo inicializado
 */
inline iNode initInode() {
    iNode node;
    node.size = 0;
    node.permissions = 0;
    node.userId = 0;
    node. groupId = 0;

    node.creationTime = 0;
    node.lastModifiedTime = 0;
    node.lastAccessTime = 0;

    for (size_t i = 0; i < TOTAL_DIRECT_POINTERS; ++i) {
        node.directBlocks[i] = BLOCK_FREE_SLOT;
    }
    node.singleIndirectPointer = BLOCK_FREE_SLOT;
    node.doubleIndirectPointer = BLOCK_FREE_SLOT;
    return node;
}

/**
 * @brief Estructura del archivo representado en el directorio
 * 
 * Maneja información sobre el nombre y el inodo asociado al archivo
 */
typedef struct File {
    char fileName[FILE_NAME_LENGTH];
    uint16_t iNodePointer;
    uint16_t filler;
    // 32 bytes
} File;

/**
 * @brief Estructura del directorio que contiene los archivos
 * 
 * Maneja un arreglo de archivos existentes
 */
typedef struct Directory {
    File entries[TOTAL_I_NODES];
} Directory;

#endif