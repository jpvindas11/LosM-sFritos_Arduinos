#ifndef ARDUINO_SIMULATION_HPP
#define ARDUINO_SIMULATION_HPP
#include <iostream>
#include <string>

enum ARDUINO_TYPE {
    AT_TILT = 1,
    AT_BUZZER,
    AT_ULTRASONIC,
};

class ArduinoSimulation {
 public:
  ArduinoSimulation(const std::string& id, int type) : arduinoID(id), isConnected(false), type(type) {}
  ~ArduinoSimulation() = default;

  void connect();

  void disconnect();

  bool getConnectionStatus();

  void sendData(const std::string& data);

  std::string getID();

  std::string arduinoType();

 private:
  std::string arduinoID;
  bool isConnected;
  int type;  // 1: Tilt, 2: Bruzzer, 3: Ultrasonico, 4: Led
};


#endif  // ARDUINO_SIMULATION_HPP
