#include "VirtualMemoryManager.hpp"
#include <bitset>


VirtualMemoryManager::VirtualMemoryManager() {
  // cargar el backingStorage
  this->disk = new FileSystem();
  if (disk->mount("src/FileSystem/disk.bin")) {
    this->initializeManager();
  }
}

VirtualMemoryManager::~VirtualMemoryManager() {
  // hacer el flush y liberar la memoria utilizada para el disco
  if (this->disk->isMounted) {
    delete this->disk;
  }
  // liberar los recursos de la tabla de correspondencia
  this->correspondanceTable.clear();
}

void VirtualMemoryManager::initializeManager() {
  // inicializar cada celda del arreglo en -1 (disponible)
  for (size_t index = 0; index < FRAME_COUNT; index++) {
    this->pageTable[index] = -1;
    this->timeTable[index] = -1;
  }
  // establecer contadores en 0
  this->timeCounter = 0;
  this->pageFaults = 0;
  this->tableHits = 0;
  // establecer el tamaño de la memoria principal que guarda el contenido de las páginas
  this->framesContent.resize(FRAME_COUNT);
  uint32_t file_size = this->disk->getFileSize("correspondenceTable.bin") + 1;
  // buffer para leer la tabla de correspondencia con el tamaño del archivo en sí
  char tableContent[file_size];
  this->disk->readFile("correspondenceTable.bin", tableContent, file_size);
  std::vector<std::string> pages;
  // obtener los pares de direcciones leídas en la tabla
  this->proccessFileCorrespondenceTable(tableContent, pages);
  // procesar los pares y rellenar el mapa de correspondencia
  this->fillCorrespondanceTable(pages);
  // inicializar la tabla de protección
  this->initializeProtectionTable();
} 

void VirtualMemoryManager::proccessFileCorrespondenceTable(char* tableContent, 
                                        std::vector<std::string>& directions) {
  // colocar el contenido del buffer en un contenedor string
  std::string content(tableContent);
  // transformar en un stringstream
  std::stringstream ss(content);
  // string que contendra cada par: dirección lógica,bloque
  std::string correspondePair;

  // obtener los pares al separar cada parte de la tabla de correspondencia mediante los ":"
  while (std::getline(ss, correspondePair, ':')) {
    if (!correspondePair.empty()) {
      // guardar el par en el vetor de direcciones
      directions.push_back(correspondePair);
    }
  }
}

void VirtualMemoryManager::fillCorrespondanceTable(
                                              std::vector<std::string>& pages) {                                             
    // separar cada uno de los pares: direccion logica,bloque a partir de la coma 
  for (const auto& pair: pages) {
   // buscar la posción de la coma
   size_t commaPostion = pair.find(',');
   // en caso de hallarla 
   if (commaPostion != std::string::npos) {
      // crear un substring que contenga la página lógica (antes de la coma)
      std::string logicalPage = pair.substr(0, commaPostion);
      // crear un substring que contenga el bloque (después de la coma)
      std::string physicalPage = pair.substr(commaPostion+1);
      if (!logicalPage.empty() && !physicalPage.empty()) {
        // guardar el mapa: la página logica es la clave, el valor será el bloque
        this->correspondanceTable[logicalPage] = physicalPage;
      }
    }
  }
}

int64_t VirtualMemoryManager::getFreePage() {
  for (int64_t index = 0; index < FRAME_COUNT; index++){
    // si una celda contiene -1 es porque el espacio para la página esta disponible
    if (this->pageTable[index] == -1) {
      // se retorna ese indice
      return index;
    }
  }
  return -1;
}

int64_t VirtualMemoryManager::checkPageInTable(int64_t page) {
  for (int64_t index = 0; index < FRAME_COUNT; index++){
    if (this->pageTable[index] == page) {
      // se retorna el indice si se encuentra la página
      return index;
    }
  }
  return -1;
}

int64_t VirtualMemoryManager::getPhysicalPage(std::string& logicalPage) {
  for (const auto& pair: this->correspondanceTable) {
    // si la clave se corresponde con la página solicitada
    if (pair.first == logicalPage) {
      // se retorna el valor numérico del bloque en el backingStorage
      return std::stoll(pair.second);
    }
  }
  return -1;
}

int VirtualMemoryManager::allocatePage(std::string& requestedPage,
                                                    int64_t storingFrameIndex) {
  int64_t physicalPage = this->getPhysicalPage(requestedPage);
  // se revisa si la página existe en el mapa de correspondencia
  if (physicalPage == -1) {
    std::cerr<<"ERROR: La página "<< requestedPage<<" no existe"<<std::endl;
    return EXIT_FAILURE;
  }
  try {
    // obtener el valor numérico de la página solicitada
    int64_t numericPage = std::stoll(requestedPage);
    // almacenar el número de página en la pageTable según el índice dado
    this->pageTable[storingFrameIndex] = numericPage;
    // establecer el tiempo de uso con el contador actual
    this->timeTable[storingFrameIndex] = this->timeCounter;
    // extraer el bloque en el backingStorage para la página solicitada
    // y almacenar en la memoria principal
    this->disk->readBlock(
                          (uint64_t)physicalPage, 
                          static_cast<void*>
                          (this->framesContent[storingFrameIndex].data));
  } catch (const std::exception& e) {
    std::cerr<<"ERROR: Página no númerica"<<std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void VirtualMemoryManager:: getReferencedPages(std::string& referenceString){
  
  std::stringstream ss(referenceString);
  std::string logicalPage;
  // separar la cadena de referencias a partir de la coma
  while (std::getline(ss, logicalPage, ',')) {
    if (!logicalPage.empty()) {
      // revisar si la página se encuentra en la memoria principal
      if (this->checkPageInTable(std::stoll(logicalPage))==-1) {
        // aumentar la cantidad de pageFaults si no es el caso
        this->pageFaults++;
        // proceder a carga la página
        this->LRUBringPage(logicalPage);
      } else {
          // indicar que la página ya se encuentra en memoria
          // aumentar los tableHits
          this->tableHits++;
          std::cout<<"La página "<<logicalPage<<" ya fue añadida a la tabla"
                   << std::endl;
          // actualizar el contador para la página a la que se accedió
          this->timeTable[this->checkPageInTable(std::stoll(logicalPage))]
                                                            = this->timeCounter; 
      }
      // aumentar el contador tras cada procesamiento de la cadena de referencias
      this->timeCounter++;
    }
  } 
}

int VirtualMemoryManager::LRUBringPage(std::string& requestedPage) {
  // obtener una página libre de la pageTable
  int64_t emptyPage = this->getFreePage();
  if (emptyPage == -1) {
    // si no se obtuvo entonces desplazar a la página usada menos recientemente
    // y usar su indice para cargar la página solicitada
    emptyPage = this->LRUSelectVictimFrame();
  }
  // cargar la página en el índice que corresponda
  this->allocatePage(requestedPage, emptyPage);
  std::cout<<"Página: "<<requestedPage<<" cargada en "<<emptyPage<<std::endl;
  //std::cout<<this->framesContent[emptyPage].data<<std::endl;
  return EXIT_SUCCESS;
}

int64_t VirtualMemoryManager:: LRUSelectVictimFrame() {
  // buscar la página usada menos recientemente
  int64_t frameIndex = this->getOldestFrame();
  // limpiar el contenido relativo a dicha página en la memoria principal
  this->cleanFrame(frameIndex);
  // retornar el índice de la página disponible
  return frameIndex;
}

int64_t VirtualMemoryManager::getOldestFrame(){
  int64_t oldestTime = this->timeTable[0];
  int64_t frameIndex = 0;
  for (int64_t index = 0; index < FRAME_COUNT; index++){
    // retornar el índice del indice cuyo tiempo es el menor
    if (this->timeTable[index] < oldestTime) {
      oldestTime = this->timeTable[index];
      frameIndex = index; 
    }
  }
  return frameIndex;
}

void VirtualMemoryManager::cleanFrame(int64_t frameNumber) {
  // colocar el espacio de esta página como disponible en ambas tablas
  this->pageTable[frameNumber] = -1;
  this->timeTable[frameNumber] = -1;
  // limpiar el buffer que contenía el bloque de la página
  memset(this->framesContent[frameNumber].data,'\0', 
                                 sizeof(this->framesContent[frameNumber].data));
}

void VirtualMemoryManager::printStatics() {
  double pageFaultPercentage = 0;
  double tableHitsPercentage = 0;
  if (this->timeCounter != 0) {
    // porcentaje de pageFaults = (pageFaults / totalDeSolicitudes) * 100
    pageFaultPercentage = 
                       ((double)this->pageFaults/(double)this->timeCounter)*100;
    // porcentaje de tableHits = (tableHits / totalDeSolicitudes) * 100
    tableHitsPercentage = 
                       ((double)this->tableHits/(double)this->timeCounter)*100;
  }
  std::cout<<"Cantidad de page faults: "<<this->pageFaults
           <<". "<<pageFaultPercentage<<"\% de las solicitudes realizadas"
           <<std::endl;
  std::cout<<"Cantidad de table hits: "<<this->tableHits
           <<". "<<tableHitsPercentage<<"\% de las solicitudes realizadas"
           <<std::endl;
}

void VirtualMemoryManager::initializeProtectionTable() {
  // Inicializar cada entrada de la tabla de protección
  for (int64_t index = 0; index < FRAME_COUNT; index++) {
    // Por defecto, sin permisos hasta que se asigne una página
    this->protectionTable[index].permissions = static_cast<uint8_t>(PagePermissions::NONE);
    // No tiene propietario inicialmente
    this->protectionTable[index].ownerProcessId = 0;
    // La página no está bloqueada
    this->protectionTable[index].isLocked = false;
    // Contador de accesos en cero
    this->protectionTable[index].accessCount = 0;
    // Límite de página: por defecto FRAME_SIZE
    this->pageBoundaries[index] = FRAME_SIZE;
  }
}

void VirtualMemoryManager::setPagePermissions(int64_t frameIndex, uint8_t permissions, uint32_t ownerProcessId) {
  // Validar el índice del frame
  if (frameIndex < 0 || frameIndex >= FRAME_COUNT) {
    std::cerr << "ERROR: Índice de frame inválido: " << frameIndex << std::endl;
    return;
  }
  
  // Asignar permisos a la página
  this->protectionTable[frameIndex].permissions = permissions;
  // Asignar el propietario
  this->protectionTable[frameIndex].ownerProcessId = ownerProcessId;
  // Reiniciar el contador de accesos
  this->protectionTable[frameIndex].accessCount = 0;
  
  std::cout << "Permisos asignados a frame " << frameIndex 
            << " para proceso " << ownerProcessId << std::endl;
}

bool VirtualMemoryManager::validateAccess(int64_t frameIndex, PagePermissions requestedPermission, uint32_t processId) {
  // Validar el índice del frame
  if (frameIndex < 0 || frameIndex >= FRAME_COUNT) {
    std::cerr << "ERROR: Índice de frame inválido: " << frameIndex << std::endl;
    return false;
  }
  
  // Si la página está bloqueada (I/O en progreso), denegar acceso
  if (this->protectionTable[frameIndex].isLocked) {
    std::cerr << "ERROR: Página " << frameIndex << " está bloqueada (I/O en progreso)" << std::endl;
    return false;
  }
  
  // Si el proceso no es el propietario, denegar acceso
  if (this->protectionTable[frameIndex].ownerProcessId != processId && 
      this->protectionTable[frameIndex].ownerProcessId != 0) {
    std::cerr << "ERROR: Proceso " << processId << " no es propietario de página " 
              << frameIndex << std::endl;
    return false;
  }
  
  // Obtener los permisos actuales
  uint8_t currentPermissions = this->protectionTable[frameIndex].permissions;
  
  // Verificar si el permiso solicitado está disponible
  if ((currentPermissions & static_cast<uint8_t>(requestedPermission)) == 0) {
    std::cerr << "ERROR: Proceso " << processId << " no tiene permiso " 
              << static_cast<int>(requestedPermission) << " en página " 
              << frameIndex << std::endl;
    return false;
  }
  
  // Incrementar contador de accesos
  this->protectionTable[frameIndex].accessCount++;
  
  // Acceso permitido
  return true;
}

void VirtualMemoryManager::setPageBoundary(int64_t frameIndex, uint32_t size) {
  // Validar el índice del frame
  if (frameIndex < 0 || frameIndex >= FRAME_COUNT) {
    std::cerr << "ERROR: Índice de frame inválido: " << frameIndex << std::endl;
    return;
  }
  
  // Validar el tamaño (no puede exceder FRAME_SIZE)
  if (size > FRAME_SIZE) {
    std::cerr << "ADVERTENCIA: Tamaño " << size << " excede FRAME_SIZE (" 
              << FRAME_SIZE << "). Ajustando a FRAME_SIZE." << std::endl;
    size = FRAME_SIZE;
  }
  
  // Establecer el límite
  this->pageBoundaries[frameIndex] = size;
  
  std::cout << "Límite de página " << frameIndex << " establecido a " << size 
            << " bytes" << std::endl;
}

bool VirtualMemoryManager::validateBoundary(int64_t frameIndex, uint32_t offset) {
  // Validar el índice del frame
  if (frameIndex < 0 || frameIndex >= FRAME_COUNT) {
    std::cerr << "ERROR: Índice de frame inválido: " << frameIndex << std::endl;
    return false;
  }
  
  // Verificar si el offset está dentro de los límites
  if (offset >= this->pageBoundaries[frameIndex]) {
    std::cerr << "ERROR: Buffer overflow detectado - offset " << offset 
              << " excede límite " << this->pageBoundaries[frameIndex] << std::endl;
    return false;
  }
  
  return true;
}

void VirtualMemoryManager::printPageProtection(int64_t frameIndex) {
  // Validar el índice del frame
  if (frameIndex < 0 || frameIndex >= FRAME_COUNT) {
    std::cerr << "ERROR: Índice de frame inválido: " << frameIndex << std::endl;
    return;
  }
  
  PageProtection& prot = this->protectionTable[frameIndex];
  
  std::cout << "\n=== Protección de Página " << frameIndex << " ===" << std::endl;
  std::cout << "Permisos: ";
  
  // Imprimir permisos de forma legible
  if (prot.permissions & static_cast<uint8_t>(PagePermissions::READ)) {
    std::cout << "R";
  }
  if (prot.permissions & static_cast<uint8_t>(PagePermissions::WRITE)) {
    std::cout << "W";
  }
  if (prot.permissions & static_cast<uint8_t>(PagePermissions::EXEC)) {
    std::cout << "X";
  }
  if (prot.permissions == static_cast<uint8_t>(PagePermissions::NONE)) {
    std::cout << "NONE";
  }
  
  std::cout << " (0b" << std::bitset<8>(prot.permissions) << ")" << std::endl;
  std::cout << "Propietario (PID): " << prot.ownerProcessId << std::endl;
  std::cout << "Bloqueada: " << (prot.isLocked ? "SÍ" : "NO") << std::endl;
  std::cout << "Accesos: " << prot.accessCount << std::endl;
  std::cout << "Límite: " << this->pageBoundaries[frameIndex] << " bytes" << std::endl;
}

void VirtualMemoryManager::printAllProtections() {
  std::cout << "\n========== TABLA DE PROTECCIÓN COMPLETA ==========" << std::endl;
  for (int64_t i = 0; i < FRAME_COUNT; i++) {
    // Solo mostrar frames que tienen contenido (pageTable[i] != -1)
    if (this->pageTable[i] != -1) {
      this->printPageProtection(i);
    } else {
      std::cout << "Frame " << i << ": LIBRE" << std::endl;
    }
  }
  std::cout << "=================================================" << std::endl;
}
