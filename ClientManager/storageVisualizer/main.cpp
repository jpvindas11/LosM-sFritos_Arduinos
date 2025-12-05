#include "Visualizer.hpp"

int main() {
  try{
    Visualizer visualizer;
    visualizer.startInteraction();
    return 0;
  } catch (const std::runtime_error& e) {
    std::cerr << "Error iniciando el visualizador: " << e.what() << std::endl;
    return 1;
  }
}