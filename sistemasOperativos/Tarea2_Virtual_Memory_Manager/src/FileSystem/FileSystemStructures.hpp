/// @copyright Los Más Fritos - 2025

#ifndef FS_STRUCTURES
#define FS_STRUCTURES

#include <string>
#include <cmath>
#include <inttypes.h>

#define BLOCK_SIZE 1024
#define TOTAL_BLOCK 16384
#define DISK_SIZE (TOTAL_BLOCK * BLOCK_SIZE)  // 16 MB
#define TOTAL_I_NODES 2048
#define TOTAL_DIRECT_POINTERS 12
#define INODE_FREE_SLOT 0xFFFF
#define BLOCK_FREE_SLOT 0xFFFFFFFF
#define FILE_NAME_LENGTH 28

// Helper macro para calcular bloques necesarios
#define BLOCKS_NEEDED(bytes) (((bytes) + BLOCK_SIZE - 1) / BLOCK_SIZE)

// Bitmap de inodos: 2048 bits = 256 bytes = 1 bloque
#define INODE_BITMAP_START 1
#define INODE_BITMAP_BLOCKS 1  // 256 bytes caben en 1 bloque de 1KB

// Bitmap de bloques: 16384 bits = 2048 bytes = 2 bloques
#define BLOCK_BITMAP_START (INODE_BITMAP_START + INODE_BITMAP_BLOCKS)  // = 2
#define BLOCK_BITMAP_BLOCKS 2  // 2048 bytes necesitan 2 bloques de 1KB

// Tabla de inodos: 2048 inodos × 82 bytes = 167936 bytes
#define INODE_SIZE 82  // Tamaño real de tu estructura packed
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)  // 1024/82 = 12 inodos por bloque
#define INODE_TABLE_START (BLOCK_BITMAP_START + BLOCK_BITMAP_BLOCKS)  // = 4
#define INODE_TABLE_BLOCKS ((TOTAL_I_NODES + INODES_PER_BLOCK - 1) / INODES_PER_BLOCK)  // ceil(2048/12) = 171 bloques

// Directorio de archivos: 2048 × 32 bytes = 65536 bytes = 64 bloques
#define FILE_DIRECTORY_START (INODE_TABLE_START + INODE_TABLE_BLOCKS)  // = 175
#define FILE_ENTRY_SIZE 32
#define FILE_DIRECTORY_BLOCKS BLOCKS_NEEDED(TOTAL_I_NODES * FILE_ENTRY_SIZE)  // 64 bloques
#define MAX_FILES TOTAL_I_NODES

// Bloques de datos: todo lo que sobra
#define DATA_BLOCKS_START (FILE_DIRECTORY_START + FILE_DIRECTORY_BLOCKS)  // = 239
#define TOTAL_DATA_BLOCKS (TOTAL_BLOCK - DATA_BLOCKS_START)  // 16384 - 239 = 16145 bloques

// Constantes para cálculos de indirección
#define POINTERS_PER_BLOCK (BLOCK_SIZE / sizeof(uint32_t))  // 256 punteros por bloque

// Rangos de bloques:
// Directos: 0-11 (12 bloques = 12 KB)
// Indirecto simple: 12-267 (256 bloques = 256 KB)
// Indirecto doble: 268-65803 (256×256 = 65536 bloques = 64 MB)
#define MAX_DIRECT_BLOCKS TOTAL_DIRECT_POINTERS  // 12
#define MAX_SINGLE_INDIRECT_BLOCKS (MAX_DIRECT_BLOCKS + POINTERS_PER_BLOCK)  // 268
#define MAX_DOUBLE_INDIRECT_BLOCKS (MAX_SINGLE_INDIRECT_BLOCKS + POINTERS_PER_BLOCK * POINTERS_PER_BLOCK)  // 65804


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

    // Usamos int32 en lugar de time_t para consistencia de tamaño
    uint32_t creationTime;
    uint32_t lastModifiedTime;
    uint32_t lastAccessTime;  

    uint32_t directBlocks[TOTAL_DIRECT_POINTERS];
    uint32_t singleIndirectPointer;
    uint32_t doubleIndirectPointer;
    // 82 bytes
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
    uint8_t isOpen;
    uint8_t type;
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