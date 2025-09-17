/// @copyright Los Más Fritos - 2025

#include <cstdint>
#include <limits.h>
#include <string>

#define DISK_SIZE 2097152  // 2MB
#define TOTAL_I_NODES 512
#define BLOCK_SIZE 256
#define TOTAL_POINTERS 12
#define BLOCK_TOTAL (DISK_SIZE / BLOCK_SIZE)
#define FREE_BLOCK -1
#define FREE_INDEX -1

// Se asume que por cada 4KB se utilizará un i-nodo (convención ext2/ext3)

// Esas macros es para elegir tamaños que rastreen los posibles indices o cantidad de cada tipo
// intentando de no desperdiciar espacio

#if TOTAL_I_NODES <= UINT8_MAX
  typedef uint8_t inode_size_t;
#elif TOTAL_I_NODES <= UINT16_MAX
  typedef uint16_t inode_size_t;
#else
  typedef uint32_t inode_size_t;
#endif

#if BLOCK_TOTAL <= UINT8_MAX
  typedef uint8_t blockNum_size_t;
#elif BLOCK_TOTAL <= UINT16_MAX
  typedef uint16_t blockNum_size_t;
#else
  typedef uint32_t blockNum_size_t;
#endif

#if BLOCK_SIZE <= UINT8_MAX
  typedef uint8_t block_size_t;
#elif BLOCK_SIZE <= UINT16_MAX
  typedef uint16_t block_size_t;
#else
  typedef uint32_t block_size_t;
#endif

/// @brief 
typedef struct dataBlock {
  bool isUsed;
  block_size_t offset;
  char data[BLOCK_SIZE];
} dataBlock_t;

/// @brief ... 
typedef struct singleFileIndex {
  bool isUsed;
  block_size_t dataPtr[TOTAL_POINTERS];
  block_size_t usedDataPtr;
} singleFileIndex_t;

/// @brief ... 
typedef struct doubleFileIndex {
  bool isUsed;
  singleFileIndex_t dataIndex[TOTAL_POINTERS];
  block_size_t usedIndex;
} doubleFileIndex_t;

/// @brief ... 
typedef struct iNode {
  uint32_t user;  // 32 bits
  uint32_t groupId;   // 32 bits
  char path[PATH_MAX]; // 256 bytes -> 2048 bits
  time_t creationTime;  // long -> 64 bits
  uint16_t permissions;   // 16 bits
  uint32_t size; // 32 bits
  bool isUsed; // 8 bits
  block_size_t directBlocks [TOTAL_POINTERS]; // block_size_t bits * 12 indices = 192 bits
  blockNum_size_t lastUsedBlock; // 16 bits
  singleFileIndex_t singleIndirect; // 16 bits
  doubleFileIndex_t doubleIndirect; // 16 bits

  // 2472 bits
  // 309 bytes

  // Necesitamos 618 bloques para almacenar la info de los 512 inodos
  // Alrededor de %7.5 de bloques totales
} iNode_t;

/// @brief ... 
typedef struct fileEntry {
  char fileName[NAME_MAX];
  inode_size_t iNodeIndex;
  bool isUsed;
} fileEntry_t;

/// @brief ... 
typedef struct directory {
  char dirName[NAME_MAX];
  time_t creationTime;
  fileEntry files[TOTAL_I_NODES];
  inode_size_t usedInodes;
} directory_t;

// Como podríamos acomodar la unidad??
// 
