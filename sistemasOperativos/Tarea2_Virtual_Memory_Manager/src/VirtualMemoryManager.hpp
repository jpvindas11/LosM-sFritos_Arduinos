#ifndef VIRTUALMEMORYMANGER
#define VIRTUALMEMORYMANGER

#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unordered_map>
#include <iostream>
#include "FileSystem.hpp"

// se asumirá que la cantidad de páginas cargadas en memoria de forma simúltanea será 8
#define FRAME_COUNT 8
// cada página/frame tendrá un tamaño de 1024 bytes
#define FRAME_SIZE 1024

// estructura para representar el frame
typedef struct frame {
  char data [1024];
} frame;

class VirtualMemoryManager {
 private:

  // Nota: Se modelará una correspondencia uno a uno entre los arreglos pageTable,
  //       timetable y framesContent, de forma que para una página, su indice es
  //       el mismo en las tres estructuras

  // este arreglo contiene el número de cada página cargada actualmente en memoria 
  int64_t pageTable[FRAME_COUNT];
  // arreglo con el "tiempo" de último uso para la página correspondiente
  int64_t timeTable[FRAME_COUNT];
  // vector que posee el contenido de la página
  std::vector<frame> framesContent;
  // estructura que mapea la correspondencia entre una página lógica y el bloque
  // en el backingStorage. Esta tabla se encuentra almacenada en el backingStorage
  std::unordered_map<std::string,std::string> correspondanceTable;
  // disco para emular el backingStorage
  FileSystem* disk;
  // contador de ejecuciones realizadas por el Virtual Manager sobre el conjunto de páginas
  uint64_t timeCounter;
  // número de pageFaults registradas
  uint64_t pageFaults;
  // número de pageHits registrados
  uint64_t tableHits;

 public:
   // constructor 
   VirtualMemoryManager();
   // destructor
   ~VirtualMemoryManager();
  /**
     * @brief Obtiene y procesa la cadena de páginas solicitadas
     * @param referenceString cadena de referencias
  */
   void getReferencedPages(std::string& referenceString);
  /**
     * @brief Imprime las estádisticas de pageFaults y pageHits al finalizar el programa
  */
   void printStatics();


 private:
  /**
     * @brief Establece todos los contadores en cero, rellena los arreglos con -1 (disponible),
     *        procesa la tabla de correspondencia y la guarda en el mapa.
  */
  void initializeManager();
  /**
     * @brief Obtiene del mapa de correspondencia el número de bloque en el backingStorage  
     *        que le corresponde a una página lógica
     * @param logicalPage Número de página lógica
     * @return int64_t Número de bloque en backingStorage
  */
  int64_t getPhysicalPage(std::string& logicalPage);
  /**
   * @brief Procesa el contenido de la tabla de correspondencia para obtener
   *        el número de página lógica y su correspondiente bloque en backingStorage
   *        los guarda en el vector de direcciones dado
   * @param tableContent buffer con el contenido de la tabal de correspondencia
   * @param directions vector que contiene cada par: página lógica,bloque
   */
  void proccessFileCorrespondenceTable(char* tableContent, 
                                          std::vector<std::string>& directions);
  /**
   * @brief Se separa cada par: página logica,bloque y se guarda en el mapa de correspondencia.
   *                            La página se determina como key y el bloque será el value asociado a la misma
   * @param pages Vector que contiene cada par: página logica,bloque 
   */
  void fillCorrespondanceTable(std::vector<std::string>& pages);
  /**
   * @brief Obtiene el índice en la pageTable disponible para cargar un bloque del backingStorage 
   * 
   * @return int64_t índice de la pageTable
   */
  int64_t getFreePage();
  /**
   * @brief Revisa si la página solicitada ya se encuentra cargada en memoria
   * 
   * @param page Número de página lógica solicitada
   * @return true Si la página se encuentra en memoria
   * @return false En caso contrario
   */
  bool checkPageInTable(int64_t page);
  /**
   * @brief Carga el bloque del backingStorage correspondiente a la página solicitada en la memoria principal
   * 
   * @param requestedPage Página solicitada
   * @param storingFrameIndex índice de la pageTable donde se colocará la página
   * @return EXIT_FAILURE si la página solicitada no existe
   * @return EXIT_SUCCES en cas contrario
   */
  int allocatePage(std::string& requestedPage, int64_t storingFrameIndex);
  /**
   * @brief Analiza si existe espacio en la pageTable para cargar la página solicitada
   *        si sí, se carga la página, si no se procede a hacer espacio en la pageTable
   *        para cargarla
   * @param requestedPage Página solicitada
   * @return EXIT_SUCCESS
   */
  int LRUBringPage(std::string& requestedPage);
  /**
   * @brief Se determina el índice en la pageTable cuya página tiene el tiempo de uso menos actualizado
   *        por lo que se considera que esta en desuso y se procede a reemplazarla con una nueva página
   *        que sí se requiere
   * 
   * @return int64_t índice de página desuso de la pageTable
   */
  int64_t LRUSelectVictimFrame();
  /**
   * @brief Limpia la información en la pageTable, timeTable y frameContents dado
   *        el índice del frame
   * 
   * @param frameNumber 
   */
  void cleanFrame(int64_t frameNumber);
  /**
   * @brief Obtiene el índice de página en la pageTable cuyo tiempo de uso es el menos reciente
   * 
   * @return int64_t índice del frame utilizado menos recientemente
   */
  int64_t getOldestFrame();


  
};

/// Método para traducir direcciones lógicas, no forma parte de la clase
inline std::string translateAddress(const std::vector<int>& addresses) {
  std::ostringstream oss;
  bool first = true;
  for (int address : addresses) {
    int page_number = (address >> 12) & 0xFF;
    if (!first) oss << ",";
    oss << page_number;
    first = false;
  }
  return oss.str();
}

#endif // VIRTUALMEMORYMANGER