#ifndef MASTER_HPP
#define MASTER_HPP
#include <string>
#include <vector>
#include "arduinoSimulation.hpp"

class Master {
 public:
  Master();
  ~Master();

  void addArduino(const ArduinoSimulation& arduino);
  void generateArduinos(int count);
  void removeArduino(const std::string& id);
  ArduinoSimulation* getArduino(const std::string& id);
  std::vector<ArduinoSimulation>& getAllArduinos() { return this->arduinos; }

 private:
  std::vector<ArduinoSimulation> arduinos;
};

#endif  // MASTER_HPP