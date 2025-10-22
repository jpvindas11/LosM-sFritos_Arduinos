#include "VirtualMemoryManager.hpp"


VirtualMemoryManager::VirtualMemoryManager() {
  // cargar el backingStorage
  this->disk = new FileSystem();
  if (disk->mount("src/FileSystem/disk.bin")) {
    this->initializeManager();
  }
}

VirtualMemoryManager::~VirtualMemoryManager() {
  // hacer el flush y lberar la memoria utilizada para el disco
  if (this->disk->isMounted) {
    delete this->disk;
  }
  // liberar los recursos de la tabla de correspondencia
  this->correspondanceTable.clear();
}

void VirtualMemoryManager::initializeManager() {
  // incializar cada celda del arreglo en -1 (disponible)
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
      // se retorna el valor númerico del bloque en el backingStorage
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
    // obtener el valor númerico de la página solicitada
    int64_t numericPage = std::stoll(requestedPage);
    // almacenar el número de página en la pageTable según el índice dado
    this->pageTable[storingFrameIndex] = numericPage;
    // establecer el tiempo de uso con el contador actual
    this->timeTable[storingFrameIndex] = this->timeCounter;
    // extraer el bloque en el backingStorage para la página solicitada
    // y alamacentar en la memoria principal
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