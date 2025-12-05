/// @copyright Los Más Fritos - 2025

#ifndef FILE_SYSTEM_2025B
#define FILE_SYSTEM_2025B

#include <array>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <vector>
#include "FileSystemStructures.hpp"

/**
 * @brief FileSystem indexado funcional con RAID 1
 * 
 * Implementa mirroring para redundancia de datos
 */

class LogsServer;
class SensorServer;

class FileSystem {
protected:
    friend class LogsServer;
    friend class SensorServer;
    
    // Super bloque que guarda los metadatos del file system
    SuperBlock superBlock;

    // RAID 1: Dos discos para mirroring
    std::fstream diskFile;       // Disco primario
    std::fstream diskFile2;      // Disco espejo
    
    std::string diskPath;
    std::string diskPath2;
    
    bool isMounted;
    
    // Estado de los discos
    bool disk1Active;
    bool disk2Active;
    
    // Bitmaps en memoria
    uint8_t inodeBitmap[INODE_BITMAP_BLOCKS * BLOCK_SIZE];
    uint8_t blockBitmap[BLOCK_BITMAP_BLOCKS * BLOCK_SIZE];

    // Gestión de bitmaps
    bool isBitSet(uint8_t* bitmap, uint32_t index);
    void setBit(uint8_t* bitmap, uint32_t index);
    void clearBit(uint8_t* bitmap, uint32_t index);
    int64_t findFreeBit(uint8_t* bitmap, uint32_t maxBits);
    
    // Lectura/escritura de bloques raw (ahora con RAID 1)
    bool readBlock(uint32_t blockNum, void* buffer);
    bool writeBlock(uint32_t blockNum, const void* buffer);
    
    bool writeBlockToDisk(std::fstream& disk, uint32_t blockNum, const void* buffer);
    bool readBlockFromDisk(std::fstream& disk, uint32_t blockNum, void* buffer);
    
    // Gestión de inodos
    bool readInode(uint16_t inodeNum, iNode* inode);
    bool writeInode(uint16_t inodeNum, const iNode* inode);
    int32_t allocateInode();
    void freeInode(uint16_t inodeNum);
    
    // Gestión de bloques de datos
    int64_t allocateBlock();
    void freeBlock(uint32_t blockNum);
    
    // Gestión de punteros indirectos
    bool allocateIndirectBlock(uint32_t indirectBlockNum, uint32_t index, uint32_t& allocatedBlock);
    bool freeIndirectBlocks(uint32_t indirectPointer, bool isDouble);
    
    // Directorio
    FileLocation findFileInDirectory(const std::string& fileName);
    bool addFileToDirectory(const std::string& fileName, uint16_t inodeNum);
    bool removeFileFromDirectory(const std::string& fileName);
    bool saveBitmaps();
    bool loadBitmaps();
    
    uint32_t getBlockNumber(iNode& node, uint32_t logicalBlockIndex);
    bool setBlockNumber(iNode& node, uint32_t logicalBlockIndex, uint32_t physicalBlock);
    
public:
    FileSystem();
    ~FileSystem();

    // Funciones principales (interfaz sin cambios)
    bool format(const std::string& path);
    bool mount(const std::string& path);
    bool unmount();

    // Operaciones de archivos
    bool createFile(const std::string& fileName, uint16_t permissions = 0644);
    bool deleteFile(const std::string& fileName);
    bool fileExists(const std::string& fileName);
    
    // Lectura/escritura
    int32_t openFile(const std::string& fileName);
    bool writeFile(const std::string& fileName, const char* data, uint32_t size);
    bool readFile(const std::string& fileName, char* buffer, uint32_t& size);
    bool readFile(const std::string& fileName, uint32_t cursor, char* buffer, uint32_t& size);
    bool appendFile(const std::string& fileName, const char* data, uint32_t size);

    // Información de archivos
    bool getFileInfo(const std::string& fileName, iNode* info);
    uint32_t getFileSize(const std::string& fileName);
    std::vector<std::string> listFiles();
    
    // Operaciones avanzadas
    bool renameFile(const std::string& oldName, const std::string& newName);
    bool changePermissions(const std::string& fileName, uint16_t permissions);
    
    // Información del sistema
    uint32_t getFreeBlocks() const { return superBlock.freeBlocks; }
    uint32_t getFreeInodes() const { return superBlock.freeInodes; }
    
    // Funciones RAID 1 específicas
    bool isDisk1Active() const { return disk1Active; }
    bool isDisk2Active() const { return disk2Active; }
    bool isRAIDHealthy() const { return disk1Active && disk2Active; }
    
    /**
     * @brief Sincroniza el disco espejo desde el primario
     * Útil si un disco falló y se reemplazó
     */
    bool rebuildMirror();

    // Funciones de inspección del disco
    void inspectDisk();
    void printFileDetails(const std::string& fileName);
    void printAllFiles();
    void printInodeDetails(uint16_t inodeNum);
    void printBlockContents(uint32_t blockNum, bool asText = true);
};

#endif