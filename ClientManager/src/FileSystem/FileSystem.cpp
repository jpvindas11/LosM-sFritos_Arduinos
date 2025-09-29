/// @copyright Los Más Fritos - 2025

#include "FileSystem.hpp"

#include <algorithm>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>

FileSystem::FileSystem() {
  // ID de usuario por defecto
  this->userID = 1000;
  this->n = 0;
  this->TUnit = DISK_SIZE;
  this->unit = new char[TUnit];
  memset(this->unit, 0, TUnit);
  this->dir = new directory_t;   // Reserva memoria para el directorio
  this->dir->usedInodes = 0;     // Inicializa inodos usados
  // Guarda la hora en que se crea el directorio
  this->dir->creationTime = time(0);
  this->memoryDisk = "filesystem.bin";
  snprintf(this->dir->dirName, NAME_MAX, "root");
  // Inicializar la tabla FAT (File Allocation Table)
  this->fat = new int[BLOCK_TOTAL];
  for (int i = 0; i < BLOCK_TOTAL; i++) {
    this->fat[i] = FREE_BLOCK;
  }
  // Inicializar los inodos
  this->inodes = new iNode_t[TOTAL_I_NODES];
  for (int i = 0; i < TOTAL_I_NODES; i++) {
    this->inodes[i].isUsed = false;
  }
  // this->loadFromDisk(this->memoryDisk);
}

FileSystem::~FileSystem() {
  // this->saveToDisk(this->memoryDisk);
  delete[] this->unit;
  delete this->dir;
  delete[] this->fat;
  delete[] this->inodes;
}

int FileSystem::createFile(string filename) {
  try {
    // Se asegura que no exista un archivo con el mismo nombre
    if (!exist(filename)) {
      int freeInodeIndex = searchEmptyNode();
      int freeBlockIndex = searchFreeBlock();
      // Crear archivo si hay inodos y bloques disponibles
      iNode_t* inodo = &this->inodes[freeInodeIndex];
      inodo->isUsed = true;
      inodo->creationTime = time(0);
      inodo->user = this->userID;
      inodo->groupId = 1000;
      inodo->permissions = 0644;  // rw- r-- r--
      inodo->size = 0;
      inodo->lastUsedBlock = 0;
      snprintf(inodo->path, PATH_MAX, "/");
      // Bloques directos
      for (int i = 0; i < TOTAL_POINTERS; ++i) {
        inodo->directBlocks[i] = -1;
      }
      inodo->directBlocks[0] = freeBlockIndex;  // Primer bloque
      // Indirección
      // Solo se usa al necesitar más bloques fuera de los directos
      inodo->singleIndirect.isUsed = false;
      inodo->singleIndirect.usedDataPtr = 0;
      inodo->doubleIndirect.isUsed = false;
      inodo->doubleIndirect.usedIndex = 0;
      this->fat[freeBlockIndex] = ERR_OCCUPIED_BLOCK;
      // Agregar entrada al directorio
      fileEntry_t* newEntry = &this->dir->files[this->dir->usedInodes];
      strncpy(newEntry->fileName, filename.c_str(), NAME_MAX - 1);
      newEntry->fileName[NAME_MAX - 1] = '\0';
      newEntry->iNodeIndex = freeInodeIndex;
      newEntry->isUsed = true;
      this->dir->usedInodes++;  // Inodos totales
      cout << "Archivo '" << filename << "' creado correctamente" << endl;
      cout << "Inodo asignado: " << freeInodeIndex << endl;
      cout << "Bloque asignado: " << freeBlockIndex << endl;
      return EXIT_SUCCESS;
    } else {
      throw FileSysError(ERR_OCCUPIED_FILENAME,
        "Ya existe un archivo llamado " + filename);
    }
  } catch (const FileSysError& err) {
    cerr << "Error al crear archivo: " << err.what() << endl;
    return err.code();
  }
}

int FileSystem::deleteFile(string filename) {
  try {
    if (exist(filename)) {
      fileEntry* target = &this->dir->files[search(filename)];
      for (int i = 0; i < TOTAL_POINTERS; ++i) {
       int block_index = this->inodes[target->iNodeIndex].directBlocks[i];
        if (block_index != -1) {
          dataBlock_t* block =
             reinterpret_cast<dataBlock_t*>(&unit[block_index
                * sizeof(dataBlock_t)]);
          for (int j = 0; j < BLOCK_SIZE; j++) {
            block->data[j] = '\0';
          }
          this->fat[block_index] = FREE_BLOCK;  // LIBERA BLOQUES
        }
      }
      // marca el nodo como libre
      this->inodes[target->iNodeIndex].isUsed = false;
      target->iNodeIndex = FREE_INDEX;
      // marca el espacio del directorio como libre
      target->isUsed = false;
      // Elimina todos los datos del archivo(?)
      // clearFileEntry(target)
      return EXIT_SUCCESS;
    } else {
      throw FileSysError(ERR_OCCUPIED_FILENAME,
        "No existe un archivo llamado " + filename);
    }
  } catch (const FileSysError& err) {
    cerr << "Error al eliminar archivo: " << err.what() << endl;
    return err.code();
  }
}

int FileSystem::search(string filename) {
  // Revisa si el archivo (inodo) está en uso
  // Y si el nombre es igual
  for (size_t i = 0; i < dir->usedInodes; ++i) {
    if (dir->files[i].isUsed &&
        filename == dir->files[i].fileName) {
      return i;
    }
  }
  throw FileSysError(ERR_NO_FILE_FOUND
      , "No se encontró el archivo llamado " + filename);
  return ERR_NO_FILE_FOUND;
}

int FileSystem::read(string file, int cursor, size_t size, char* buffer) {
  try {
    int fileIndex = search(file);
    iNode_t* inode = &this->inodes[fileIndex];
    if (cursor > static_cast<int>(inode->size)) {
      throw FileSysError(ERR_OUT_OF_RANGE
        , "El cursor se encuentra fuera del rango del archivo");
    }
    size_t bytesToRead = min(size, static_cast<size_t>(inode->size - cursor));
    size_t bytesRead = 0;

      if (open(file) == EXIT_SUCCESS) {
        const size_t maxBlocks = TUnit/sizeof(dataBlock_t);
        int processedBlocks = -1;
        int previousBlock = -1;
        while (bytesRead < bytesToRead) {
          int blockIndex = (cursor + bytesRead) / BLOCK_SIZE;
          int blockOffset = (cursor + bytesRead) % BLOCK_SIZE;
          block_size_t dataBlockNum = 0;
          if (previousBlock != blockIndex) {
            previousBlock = blockIndex;
            processedBlocks++;
          }
          if (static_cast<size_t>(blockIndex) >= maxBlocks) {
            cerr << "Error: El archivo es demasiado grande para leer"
                 << endl;
            break;
          }
          if (processedBlocks < TOTAL_POINTERS) {
            dataBlockNum = inode->directBlocks[processedBlocks];
          } else if (processedBlocks < (TOTAL_POINTERS
              + TOTAL_POINTERS)) {
            dataBlockNum = inode->singleIndirect.dataPtr[processedBlocks
                - TOTAL_POINTERS];
          } else {
            dataBlockNum = inode->doubleIndirect.dataIndex[processedBlocks
                - TOTAL_POINTERS - TOTAL_POINTERS];
          }
          if (dataBlockNum == FREE_BLOCK) {
            break;
          }
          dataBlock_t* block =  reinterpret_cast<dataBlock_t*>
              (&unit[dataBlockNum * sizeof(dataBlock_t)]);
          size_t canRead = min(bytesToRead - bytesRead
              , static_cast<size_t>(BLOCK_SIZE - blockOffset));
          memcpy(buffer + bytesRead, block->data + blockOffset, canRead);
          bytesRead += canRead;
        }
        if (bytesRead < size) {
          buffer[bytesRead] = '\0';
        } else if (size > 0) {
          buffer[size - 1] = '\0';
        }
        /*
        cout << "Datos leídos: "
             << buffer
             << endl;
        */
      }
      close(file);
      return bytesRead;
  } catch (const FileSysError& err) {
    cerr << "Error al leer archivo: " << err.what() << endl;
    return err.code();
  }
}

int FileSystem::write(string file, int cursor
                    , size_t size, const char* buffer) {
  try {
    int fileIndex = search(file);
    iNode_t* inode = &this->inodes[this->dir->files[fileIndex].iNodeIndex];
    if (cursor > static_cast<int>(inode->size)) {
      throw FileSysError(ERR_OUT_OF_RANGE
        , "El cursor se encuentra fuera del rango del archivo");
    }
    size_t bytesWritten = 0;
    size_t bytesToWrite = size;

    if (open(file) == EXIT_SUCCESS) {
      const size_t maxBlocks = TUnit/sizeof(dataBlock_t);
      int previousBlock = cursor/ BLOCK_SIZE;
      while (bytesWritten < bytesToWrite) {
        int blockIndex = (cursor + bytesWritten) / BLOCK_SIZE;
        int blockOffset = (cursor + bytesWritten) % BLOCK_SIZE;
        if (blockIndex >= static_cast<int>(maxBlocks)) {
          cerr << "Error: El archivo es demasiado grande para escribir"
               << endl;
          break;
        }
        block_size_t dataBlockNum = FREE_BLOCK;
        if (inode->lastUsedBlock < TOTAL_POINTERS) {
          if (blockIndex != previousBlock) {
            previousBlock = blockIndex;
            inode->lastUsedBlock++;
            if (inode->lastUsedBlock < TOTAL_POINTERS) {
              inode->directBlocks[inode->lastUsedBlock] = searchFreeBlock();
              this->fat[inode->directBlocks[inode->lastUsedBlock]]
                  = ERR_OCCUPIED_BLOCK;
            } else {
              continue;
            }
          }
          dataBlockNum = inode->directBlocks[inode->lastUsedBlock];
        } else if (inode->singleIndirect.usedDataPtr < TOTAL_POINTERS) {
          if (blockIndex != previousBlock && inode->singleIndirect.isUsed) {
              previousBlock = blockIndex;
              inode->singleIndirect.usedDataPtr++;
              if (inode->singleIndirect.usedDataPtr < TOTAL_POINTERS) {
                inode->singleIndirect.dataPtr[
                    inode->singleIndirect.usedDataPtr]= searchFreeBlock();
                this->fat[inode->singleIndirect.dataPtr[
                    inode->singleIndirect.usedDataPtr]] = ERR_OCCUPIED_BLOCK;
              } else {
                continue;
              }
            }
            if (!inode->singleIndirect.isUsed) {
                inode->singleIndirect.isUsed = true;
                inode->singleIndirect.usedDataPtr = 0;
                for (int i = 0; i < TOTAL_POINTERS; i++) {
                  inode->singleIndirect.dataPtr[i] = -1;
                }
                inode->singleIndirect.dataPtr[0] = searchFreeBlock();
                this->fat[inode->singleIndirect.dataPtr[0]]
                    = ERR_OCCUPIED_BLOCK;
                previousBlock = blockIndex;
            }
            dataBlockNum = inode->singleIndirect.dataPtr[
                  inode->singleIndirect.usedDataPtr];
        } else {
           if (blockIndex != previousBlock && inode->doubleIndirect.isUsed) {
              previousBlock = blockIndex;
              inode->doubleIndirect.usedIndex++;
              if (inode->doubleIndirect.usedIndex <
                  TOTAL_POINTERS*TOTAL_POINTERS) {
                inode->doubleIndirect.dataIndex[
                    inode->doubleIndirect.usedIndex]= searchFreeBlock();
                this->fat[inode->doubleIndirect.dataIndex[
                    inode->doubleIndirect.usedIndex]] = ERR_OCCUPIED_BLOCK;
              } else {
                continue;
              }
            }
            if (!inode->doubleIndirect.isUsed) {
                inode->doubleIndirect.isUsed = true;
                inode->doubleIndirect.usedIndex = 0;
                for (int i = 0; i < TOTAL_POINTERS*TOTAL_POINTERS; i++) {
                  inode->doubleIndirect.dataIndex[i] = -1;
                }
                inode->doubleIndirect.dataIndex[0] = searchFreeBlock();
                this->fat[inode->doubleIndirect.dataIndex[0]]
                    = ERR_OCCUPIED_BLOCK;
                previousBlock = blockIndex;
            }
            dataBlockNum = inode->doubleIndirect.dataIndex[
                  inode->doubleIndirect.usedIndex];
         }
        dataBlock_t* block = reinterpret_cast<dataBlock_t*>(&unit[dataBlockNum
            * sizeof(dataBlock_t)]);
        size_t canWrite = min(bytesToWrite - bytesWritten
            , static_cast<size_t>((BLOCK_SIZE - blockOffset)));
        memcpy(block->data + blockOffset, buffer + bytesWritten, canWrite);
        bytesWritten += canWrite;
        block->offset = blockOffset + canWrite;
        // Actualizar tamaño del archivo si creció
        size_t newEnd = static_cast<size_t>(cursor + bytesWritten);
        if (newEnd > inode->size) {
          inode->size = static_cast<uint32_t>(newEnd);
        }
      }
    }
    close(file);
    return bytesWritten;
  } catch (const FileSysError& err) {
    cerr << "Error al escribir el archivo: "
         << err.what()
         << endl;
    return err.code();
  }
}

int FileSystem::append(string filename, int cursor, size_t size, const char* buffer) {
  // crea un buffer para leer los datos ya escritos
  size_t oldSize = getFileSize(filename);
  char* oldData = new char[oldSize + 1]();
  this->read(filename, cursor, oldSize + 1, oldData);
  // Crea un nuevo buffer
  size_t newSize = oldSize + size;
  char* newBuffer = new char[newSize + 1]();
  // Une los datos nuevos y viejos en el nuevo buffer
  strcpy(newBuffer, buffer);
  strcat(newBuffer, oldData);
  // escribe en el archivo
  return this->write(filename, cursor, newSize, newBuffer);
}

int FileSystem::getFileSize(const std::string& filename) {
  try {
    int fileIndex = search(filename);
    iNode_t* inode = &this->inodes[this->dir->files[fileIndex].iNodeIndex];
    return static_cast<int>(inode->size);
  } catch (const FileSysError& err) {
    cerr << "Error al obtener tamaño de archivo: "
         << err.what()
         << endl;
    return err.code();
  }
}

int FileSystem::rename(string filename, string newname) {
  try {
    if (exist(filename) && !exist(newname)) {
      fileEntry* target = &this->dir->files[search(filename)];
      strncpy(target->fileName, newname.c_str(), NAME_MAX - 1);
      target->fileName[NAME_MAX - 1] = '\0';
      return EXIT_SUCCESS;
    }
  } catch (const FileSysError& err) {
    cerr << "Error al renombrar archivo: "<< err.what() << endl;
    return err.code();
  }
  return EXIT_FAILURE;
}

void FileSystem::printDirectory() {
  for (size_t index = 0; index < TOTAL_I_NODES; ++index) {
    if (this->dir->files[index].isUsed) {
      cout << this->dir->files[index].iNodeIndex << ")  "
           << this->dir->files[index].fileName   << endl;
    }
  }
}

void FileSystem::printUnidad() {
  for (size_t index = 0; index < 512; ++index) {
    if (this->fat[index] == ERR_OCCUPIED_BLOCK) {
      // hallar el bloque en la unidad
      dataBlock_t* block = reinterpret_cast<dataBlock_t*>
          (&unit[index * sizeof(dataBlock_t)]);
      cout << "Leyendo bloque: " << index << endl;
      // imprimir la sección de datos del bloque
      cout << block->data << endl;
    }
  }
}

void FileSystem::changeUserID(int newID) {
  this->userID = newID;
}

// Private

int FileSystem::open(string filename) {
  try {
    int index = search(filename);
    fileEntry_t* file = &dir->files[index];
    if (file->isOpen) {
      return ERR_FILE_ALREADY_OPEN;
    } else {
      file->isOpen = true;
    }
    return EXIT_SUCCESS;
  } catch (const FileSysError& err) {
    cerr << "Error al abrir el archivo: " << err.what() << endl;
    return err.code();
  }
}

int FileSystem::close(string filename) {
  try {
    int index = search(filename);
    fileEntry_t* file = &dir->files[index];
    if (!file->isOpen) {
      return ERR_FILE_ALREADY_CLOSE;
    } else {
      file->isOpen = false;
    }
    return EXIT_SUCCESS;
  } catch (const FileSysError& err) {
    cerr << "Error al cerrar el archivo: " << err.what() << endl;
    return err.code();
  }
}

bool FileSystem::exist(string filename) {
  for (size_t i = 0; i < dir->usedInodes; ++i) {
    if (dir->files[i].isUsed &&
        filename == dir->files[i].fileName) {
      return true;
    }
  }
  return false;
}

int FileSystem::searchEmptyNode() {
  for (size_t index = 0; index < TOTAL_I_NODES; ++index) {
    if (!this->inodes[index].isUsed) {
      return index;
    }
  }
  throw FileSysError(ERR_NO_FREE_INODES, "No hay inodos libres disponibles");
}

int FileSystem::searchFreeBlock() {
  for (size_t index = 0; index < BLOCK_TOTAL; ++index) {
    if (this->fat[index] == FREE_BLOCK) {
      return index;
    }
  }
  throw FileSysError(ERR_NO_FREE_BLOCKS, "No hay bloques libres disponibles");
}

// Implementación de la serialización
int FileSystem::saveToDisk(const string& filename) {
  ofstream file(filename, ios::binary);
  try {
    if (!file.is_open()) {
      throw FileSysError(ERR_NO_FILE_FOUND
          , "No se pudo abrir el archivo " + filename);
    }
    fsHeader_t header;
    header.magic = 42;
    header.version = 1;
    header.totalBlocks = BLOCK_TOTAL;
    header.totalInodes = TOTAL_I_NODES;
    header.blockSize = BLOCK_SIZE;
    header.usedBlocks = calculateUsedBlocks();
    header.usedInodes = this->dir->usedInodes;
    header.lastModified = time(0);
    file.write(reinterpret_cast<const char*>(&header)
        , sizeof(fsHeader_t));
    file.write(reinterpret_cast<const char*>(this->dir)
        , sizeof(directory_t));
    file.write(reinterpret_cast<const char*>(this->fat)
        , sizeof(int) * BLOCK_TOTAL);
    file.write(reinterpret_cast<const char*>(this->inodes)
        , sizeof(iNode_t) * TOTAL_I_NODES);
    file.write(this->unit, DISK_SIZE);
    file.close();
    cout << "Sistema de archivos guardado en "
         << filename
         << endl;
    return EXIT_SUCCESS;
  } catch (const FileSysError& err) {
    cerr << "Error al guardar: " << err.what() << endl;
    file.close();
    return err.code();
  }
}

int FileSystem::loadFromDisk(const string& filename) {
  ifstream file(filename, ios::binary);
  try {
    if (!file.is_open()) {
      throw FileSysError(ERR_NO_FILE_FOUND
          , "No se pudo abrir el archivo " + filename);
    }
    fsHeader_t header;
    file.read(reinterpret_cast<char*>(&header), sizeof(fsHeader_t));
    if (header.magic != 42) {
      throw FileSysError(ERR_WRONG_DATA, "Magic number invalido");
    }
    if (header.totalBlocks != BLOCK_TOTAL
        || header.totalInodes != TOTAL_I_NODES) {
      throw FileSysError(ERR_WRONG_DATA
          , "Incompatibilidad de configuración del sistema");
    }
    file.read(reinterpret_cast<char*>(this->dir)
        , sizeof(directory_t));
    file.read(reinterpret_cast<char*>(this->fat)
        , sizeof(int) * BLOCK_TOTAL);
    file.read(reinterpret_cast<char*>(this->inodes)
        , sizeof(iNode_t) * TOTAL_I_NODES);
    file.read(this->unit, DISK_SIZE);
    file.close();
    cout << "Sistema de archivos cargado desde "
         << filename
         << endl;
    return EXIT_SUCCESS;
  } catch (const FileSysError& err) {
    cerr << "Error al cargar: " << err.what() << endl;
    file.close();
    return err.code();
  }
}

uint32_t FileSystem::calculateUsedBlocks() {
  uint32_t count = 0;
  for (int i = 0; i < BLOCK_TOTAL; i++) {
    if (this->fat[i] == ERR_OCCUPIED_BLOCK) {
      count++;
    }
  }
  return count;
}
