#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include "../FileSystem/src/FileSystem.hpp"

// Chose the disk depending on the compile flag
#if defined(DISK_A)
    #define DISK_PATH "../storage/sensorStorage.bin"
#elif defined(DISK_B)
    #define DISK_PATH "../UserLogs/userLogsStorage.bin"
#else
    #define DISK_PATH ""
#endif


class Visualizer{
 private:
  FileSystem StorageDisk;
  bool startMenu;
 public:
  Visualizer();
  ~Visualizer();
  void startInteraction();
 private:
  std::string userMenu();
  bool exportFile(const std::string& FileName,
                  const std::string& hostOutPath);
  void waitForEnter();
  void refreshDisk();
      
};

#endif //VISUALIZER_HPP