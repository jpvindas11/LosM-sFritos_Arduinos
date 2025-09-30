#include "arduinoSimulation.hpp"

void ArduinoSimulation::connect() {
  this->isConnected = true;
  std::cout << "Arduino " << this->arduinoID << " conectado." << std::endl;
}

void ArduinoSimulation::disconnect() {
  this->isConnected = false;
  std::cout << "Arduino " << this->arduinoID << " desconectado." << std::endl;
}

bool ArduinoSimulation::getConnectionStatus() {
  return this->isConnected;
}

std::string ArduinoSimulation::getID() {
  return this->arduinoID;
}

std::string ArduinoSimulation::arduinoType() {
  switch (this->type) {
    case AT_TILT:
      return "Tilt";
    case AT_BUZZER:
      return "Bruzzer";
    case AT_ULTRASONIC:
      return "Ultrasonico";
    default:
      return "Led";
  }
}

void ArduinoSimulation::sendData(const std::string& data) {
  if (this->isConnected) {
    std::string specificData;
    if (this->arduinoType() == "Tilt") {
      specificData = "Estado del sensor Tilt: Inclinación detectada";
    } else if (this->arduinoType() == "Buzzer") {
      specificData = "Estado del Buzzer: Activado";
    } else if (this->arduinoType() == "Ultrasonico") {
      specificData = "Distancia medida: 45 cm";
    } else if (this->arduinoType() == "Led") {
      specificData = "Estado de los LEDs: ON";
    } else {
      specificData = data;
    }
    std::cout << "Arduino " << this->arduinoID << " enviando datos: " << specificData << std::endl;
  } else {
    std::cout << "Error: Arduino " << this->arduinoID << " no está conectado. No se pueden enviar datos." << std::endl;
  }
}
