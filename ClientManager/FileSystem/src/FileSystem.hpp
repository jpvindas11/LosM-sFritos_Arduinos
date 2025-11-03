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
 * @brief FileSystem indexado funcional
 * 
 * Sin creación de carpetas por el momento
 */

class LogsServer;

class FileSystem {
protected:
    friend class LogsServer;
    // Super bloque que guarda los metadatos del file system
    SuperBlock superBlock;

    // Directorio del file system donde se contienen los archivos
    // Directory directory;

    // Disco del file system
    std::fstream diskFile;

    // Path para el disco en uso
    std::string diskPath;

    // Dicta si el disco está activo
    bool isMounted;
    
    // Bitmaps en memoria

    // Bitmap de inodos
    uint8_t inodeBitmap[INODE_BITMAP_BLOCKS * BLOCK_SIZE];

    // Bitmap de bloques
    uint8_t blockBitmap[BLOCK_BITMAP_BLOCKS * BLOCK_SIZE];

    // Gestión de bitmaps

    /**
     * @brief Revisa la polaridad de un bit en un bitmap
     * @param bitmap El bitmap siendo revisado
     * @param index El índice verificado
     * @return True si el bit está encendido, de lo contrario False
     */
    bool isBitSet(uint8_t* bitmap, uint32_t index);

    /**
     * @brief Enciende un bit en un bitmap
     * @param bitmap El bitmap siendo modificado
     * @param index El índice a encender
     */
    void setBit(uint8_t* bitmap, uint32_t index);

    /**
     * @brief Apaga un bit en un bitmap
     * @param bitmap El bitmap siendo modificado
     * @param index El índice a limpiar
     */
    void clearBit(uint8_t* bitmap, uint32_t index);

    /**
     * @brief Busca el primer bit libre en un bitmap
     * @param bitmap El bitmap siendo revisado
     * @param maxBits Cantidad máxima de entradas en el bitmap
     * @return Si hay un bit libre, retorna su índice, de lo contrario retorna -1
     */ 
    int64_t findFreeBit(uint8_t* bitmap, uint32_t maxBits);
    
    // Lectura/escritura de bloques raw

    /**
     * @brief Lee un bloque del disco
     * @param blockNum El número de bloque que se va a leer
     * @param buffer Buffer de bits donde se guardará el resultado
     * @return True si la lectura fue exitosa
     */
    bool readBlock(uint32_t blockNum, void* buffer);

    /**
     * @brief Escribe un bloque del disco (reemplaza)
     * @param blockNum El número de bloque que se va a escribir
     * @param buffer Buffer de bits que será escrito en disco
     * @return True si la escritura fue exitosa
     */
    bool writeBlock(uint32_t blockNum, const void* buffer);
    
    // Gestión de inodos

    /**
     * @brief Lee un inodo del disco
     * @param inodeNum Número de inodo que será leido
     * @param inode Puntero del inodo que guarda la información
     * @return True si la lectura fue exitosa
     */
    bool readInode(uint16_t inodeNum, iNode* inode);

    /**
     * @brief Escribir un inodo al disco 
     * @param inodeNum Número de inodo que será escrito
     * @param inode Puntero del inodo que contiene la información nueva
     * @return True si la escritura fue exitosa
     */
    bool writeInode(uint16_t inodeNum, const iNode* inode);

    /**
     * @brief Reserva un inodo disponible
     * @return índice del inodo reservado, -1 si la operación no pudo realizarse
     */
    int32_t allocateInode();

    /**
     * @brief Libera un inodo
     * @param inodeNum Número del inodo a liberar
     */
    void freeInode(uint16_t inodeNum);
    
    // Gestión de bloques de datos

    /**
     * @brief Reserva un bloque disponible
     * @return índice del bloque reservado, -1 si la operación no pudo realizarse
     */
    int64_t allocateBlock();

    /**
     * @brief Libera un bloque
     * @param Índice del bloque a liberar
     */
    void freeBlock(uint32_t blockNum);
    
    // Gestión de punteros indirectos

    /**
     * @brief Reserva un bloque para direccionamiento indirecto
     * @param indirectBlockNum índice del bloque indirecto del inodo
     * @param index Índice del bloque que se desea asignar
     * @param allocatedBlock Referencia al bloque reservado
     * @return True si se completó la operación, False en el caso contrario
     */
    bool allocateIndirectBlock(uint32_t indirectBlockNum, uint32_t index, uint32_t& allocatedBlock);
    
    /**
     * @brief Libera los bloques implicados en la indirección, esto incluye:
     * 
     * Bloques de direcciones
     * 
     * Bloques de datos
     * 
     * @param indirectPointer Bloque que contiene las direcciones
     * @param isDouble Indica si la indirección a liberar es doble
     * @return True si la operación fue exitosa, False en caso contrario 
     */
    bool freeIndirectBlocks(uint32_t indirectPointer, bool isDouble);
    
    // Directorio

    /**
     * @brief Busca un archivo en el directorio
     * @param fileName Nombre del archivo
     * @return Estructura con información del archivo, FileLocation.found = false si no existe
     */
    FileLocation findFileInDirectory(const std::string& fileName);

    /**
     * @brief Agrega un archivo al directorio
     * @param fileName Nombre del archivo
     * @param inodeNum Inodo asociado al archivo
     * @return True si se agregó correctamente, False en caso contrario
     */
    bool addFileToDirectory(const std::string& fileName, uint16_t inodeNum);

    /**
     * @brief Elimina un archivo del directorio
     * @param fileName Nombre del archivo
     * @return True si se borró correctamente, False en caso contrario
     */
    bool removeFileFromDirectory(const std::string& fileName);

    /**
     * @brief Actualiza el estado de los bitmaps en el disco
     * @brief True si se guardaron correctamente, False en caso contrario
     */
    bool saveBitmaps();

    /**
     * @brief Carga el estado de los bitmaps del disco
     * @brief True si se cargaron correctamente, False en caso contrario
     */
    bool loadBitmaps();

    /**
     * @brief Obtiene el número de bloque físico correspondiente a un índice lógico.
     * @param node Referencia al inodo del archivo que contiene los punteros a bloques
     * @param logicalBlockIndex Índice lógico del bloque dentro del archivo (0-4171)
     * @return Número del bloque físico donde se encuentran los datos.
     *         Retorna BLOCK_FREE_SLOT si la operación no se completó
     */
    uint32_t getBlockNumber(iNode& node, uint32_t logicalBlockIndex);

    /**
     * @brief Establece el mapeo entre un índice lógico y un bloque físico.
     * @param node Referencia al inodo del archivo (se modifica si se crean nuevos punteros)
     * @param logicalBlockIndex Índice lógico donde se quiere mapear el bloque (0-4171)
     * @param physicalBlock Número del bloque físico a asignar en esa posición
     * 
     * @return true Si el mapeo se realizó exitosamente, False del caso contrario
     */
    bool setBlockNumber(iNode& node, uint32_t logicalBlockIndex, uint32_t physicalBlock);
    
public:
    // Inicialización y montaje
    FileSystem();

    // Finalizar programa (Llama a unmount())
    ~FileSystem();

    /**  
     * @brief Crear y formatear disco nuevo
     * @param path Dirección en donde se guardará el disco nuevo
     * @return True si se completó el formateo, False en caso contrario
    */
    bool format(const std::string& path);  

    /**  
     * @brief Montar un disco existente
     * @param path Dirección en donde se encuentra el disco
     * @return True si se completó la carga del disco, False en caso contrario
     * @note Llama a format() si no encuentra un disco
    */
    bool mount(const std::string& path);

    /**  
     * @brief Desmontar un disco existente y guardar los cambios
     * @return True si se completó la descarga del disco, False en caso contrario
     * @note Es llamado en el destructor del filesystem
    */
    bool unmount();                        // Desmontar y guardar cambios

    // Operaciones de archivos

    /**  
     * @brief Crear un archivo
     * @param fileName Nombre del nuevo archivo
     * @param permissions Permisos del nuevo archivo, de base son (-rw-r--r--)
     * @return True si el archivo fue creado, False en caso contrario
    */
    bool createFile(const std::string& fileName, uint16_t permissions = 0644);

    /**  
     * @brief Eliminar un archivo
     * @param fileName Nombre del archivo a eliminar
     * @return True si el archivo fue eliminado, False en caso contrario
    */
    bool deleteFile(const std::string& fileName);

    /**  
     * @brief Verifica de la existencia de un archivo en el directorio
     * @param fileName Nombre del archivo a buscar
     * @return True si el archivo existe, False en caso contrario
    */
    bool fileExists(const std::string& fileName);
    
    // Lectura/escritura

    /**
     * @brief Abre un archivo para permitir cambios
     * @param fileName Nombre del archivo
     * @return El inodo asociado al archivo, -1 si el archivo no se pudo abrir
     * @warning ESTE MÉTODO NO ESTÁ IMPLEMENTADO
     */
    int32_t openFile(const std::string& fileName);

    /**
     * @brief Escribe nuevos datos en un archivo (Con reemplazo)
     * @param fileName Nombre del archivo
     * @param data Buffer de datos a escribir
     * @param size Tamaño del buffer
     * @return True si el archivo fue modificado, False en caso contrario
     */
    bool writeFile(const std::string& fileName, const char* data, uint32_t size);

    /**
     * @brief Lee los datos de un archivo
     * @param fileName Nombre del archivo
     * @param data Buffer de datos a en donde se guardará la información obtenida
     * @param size Tamaño del buffer recibido
     * @return True si el archivo fue leido, False en caso contrario
     */
    bool readFile(const std::string& fileName, char* buffer, uint32_t& size);

    /**
     * @brief Agrega nuevos datos a un archivo existente
     * @param fileName Nombre del archivo
     * @param data Buffer de datos que se agregarán al archivo
     * @param size Tamaño del buffer
     * @return True si la información fue agregada, False en caso contrario
     */
    bool appendFile(const std::string& fileName, const char* data, uint32_t size);
    
    // Información de archivos

    /**
     * @brief Obtiene los metadatos (iNodo) de un archivo
     * @param fileName Nombre del archivo
     * @param info Puntero del inodo a recibir
     * @return True si el inodo fue recibido, False en caso contrario
     */
    bool getFileInfo(const std::string& fileName, iNode* info);

    /**
     * @brief Obtiene el tamaño de un archivo
     * @param fileName Nombre del archivo
     * @return El tamaño en bytes del archivo, 0 si no se completó la operación
     */
    uint32_t getFileSize(const std::string& fileName);

    /**
     * @brief Recibe una lista que contiene todos los archivos en el directorio
     * @return La lista con los archivos del directorio
     */
    std::vector<std::string> listFiles();
    
    // Operaciones avanzadas

    /**
     * @brief Cambia el nombre de un archivo
     * @param oldName Nombre del archivo
     * @param newName Nombre nuevo del archivo
     * @return True si el nombre se cambió, False en caso contrario
     */
    bool renameFile(const std::string& oldName, const std::string& newName);

    /**
     * @brief Cambia los permisos de lectura, escritura y ejecución del archivo
     * @param fileName Nombre del archivo
     * @param permissions Nuevos permisos del archivo
     * @return True si los permisos cambiaron, False en caso contrario
     */
    bool changePermissions(const std::string& fileName, uint16_t permissions);
    
    // Información del sistema

    /**
     * @brief Obtiene cuantos bloques libres hay en el file system
     * @return Cantidad de bloques libres
     */
    uint32_t getFreeBlocks() const { return superBlock.freeBlocks; }

    /**
     * @brief Obtiene cuantos inodos libres hay en el file system
     * @return Cantidad de inodos libres
     */
    uint32_t getFreeInodes() const { return superBlock.freeInodes; }

    // Funciones de inspección del disco

    /**
     * @brief Muestra una vista general del sistema de archivos con 
     * estadísticas de uso, estructura del disco y lista de archivos
     */
    void inspectDisk();

    /**
     * @brief Muestra información detallada de un archivo específico, 
     * incluyendo metadatos, timestamps y estructura de bloques.
     * @param fileName Nombre del archivo
     */
    void printFileDetails(const std::string& fileName);

    /**
     * @brief Lista todos los archivos del sistema en formato
     * tabla con sus principales características.
     */
    void printAllFiles();

    /**
     * @brief Muestra todos los detalles de un inodo específico 
     * (útil para debugging bajo nivel).
     * @param inodeNum Inodo a ser revisado
     */
    void printInodeDetails(uint16_t inodeNum);

    /**
     * @brief Imprime el contenido de un bloque específico, 
     * ya sea como texto o en formato hexadecimal.
     * @param asText Dicta si el resultado es visto en texto o hexadecimal
     */
    void printBlockContents(uint32_t blockNum, bool asText = true);
};

#endif