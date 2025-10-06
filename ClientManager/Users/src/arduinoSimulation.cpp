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

std::string ArduinoSimulation::sendData() {
  if (this->isConnected) {
    std::string specificData;
    if (this->arduinoType() == "Tilt") {
      specificData = "Estado del sensor Tilt: Inclinación detectada";
    } else if (this->arduinoType() == "Bruzzer") {
      specificData = "Estado del Buzzer: Activado";
    } else if (this->arduinoType() == "Ultrasonico") {
      specificData = "Distancia medida: 45 cm";
    } else if (this->arduinoType() == "Led") {
      specificData = "Estado de los LEDs: ON";
    }
    std::cout << "Arduino " << this->arduinoID << " enviando datos: " << specificData << std::endl;
    return specificData;
  } else {
    std::cout << "Error: Arduino " << this->arduinoID << " no está conectado. No se pueden enviar datos." << std::endl;
    return "Error: No conectado";
  }
}
