#include "Master.hpp"
#include <algorithm>
#include <iostream>

Master::Master() {}

Master::~Master() {
}

void Master::addArduino(const ArduinoSimulation& arduino) {
  this->arduinos.push_back(arduino);
}

void Master::removeArduino(const std::string& id) {
  this->arduinos.erase(std::remove_if(this->arduinos.begin(), this->arduinos.end(), [&id](ArduinoSimulation& arduino) {
    return arduino.getID() == id;}), this->arduinos.end());
}

ArduinoSimulation* Master::getArduino(const std::string& id) {
  for (auto& arduino : this->arduinos) {
    if (arduino.getID() == id) {
      return &arduino;
    }
  }
  return nullptr;
}

void Master::generateArduinos(int count) {
  for (int i = 0; i < count; ++i) {
    std::string id = "Arduino_" + std::to_string(i + 1);
    ArduinoSimulation arduino(id, (i % 4) + 1);
    arduino.connect();
    this->addArduino(arduino);
  }
}
