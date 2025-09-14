/// @copyright Los Más Fritos - 2025

#include <cstdint>
#include <string>

#define DISK_SIZE 2097152  // 2MB
#define TOTAL_I_NODES 512
#define BLOCK_SIZE 256
#define TOTAL_POINTERS 12
#define BLOCK_TOTAL (DISK_SIZE / BLOCK_SIZE)

// Se asume que por cada 4KB se utilizará un i-nodo (convención ext2/ext3)

// Esas macros es para elegir tamaños que rastreen los posibles indices o cantidad de cada tipo
// intentando de no desperdiciar espacio

#if TOTAL_I_NODES <= UINT8_MAX
    typedef uint8_t iNodeNum;
#elif TOTAL_I_NODES <= UINT16_MAX
    typedef uint16_t iNodeNum;
#else
    typedef uint32_t iNodeNum;
#endif

#if BLOCK_TOTAL <= UINT8_MAX
    typedef uint8_t blockIndex;
#elif BLOCK_TOTAL <= UINT16_MAX
    typedef uint16_t blockIndex;
#else
    typedef uint32_t blockIndex;
#endif

#if BLOCK_SIZE <= UINT8_MAX
    typedef uint8_t offset;
#elif BLOCK_SIZE <= UINT16_MAX
    typedef uint16_t offset;
#else
    typedef uint32_t offset;
#endif

/// @brief 
typedef struct dataBlock {
  bool isUsed;
  offset lastUsedByte;
  char* data[BLOCK_SIZE];
} dataBlock_t;

/// @brief ... 
typedef struct fileIndex {
  bool isUsed;
  void* dataPtr[TOTAL_POINTERS];
  uint32_t usedPtr;
} fileIndex_t;

/// @brief ... 
typedef struct iNode {
  uint32_t user;
  uint32_t groupId;
  std::string path;
  time_t creationTime;
  uint16_t permissions;
  uint32_t size;
  bool isUsed;
  dataBlock_t* directBlocks [TOTAL_POINTERS];
  blockIndex lastUsedBlock;
  fileIndex_t* singleIndirect;
  fileIndex_t* doubleIndirect;
} iNode_t;

/// @brief ... 
typedef struct fileEntry {
  std::string fileName;
  iNodeNum inodeIndex;
  bool isUsed;
} fileEntry_t;

/// @brief ... 
typedef struct directory {
  std::string dirName;
  time_t creationTime;
  fileEntry files[TOTAL_I_NODES];
  iNodeNum usedInodes;
} directory_t;
