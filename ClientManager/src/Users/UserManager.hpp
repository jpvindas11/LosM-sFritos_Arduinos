/// @copyright Los Más Fritos - 2025

#ifndef USER_MANAGER_2025
#define USER_MANAGER_2025

#include <string>

#include "common.hpp"
#include "FileSystem.hpp"

#define PASSWORD_MAX 127

typedef struct user {
  char name[NAME_MAX];
  char password[PASSWORD_MAX];
} user_t;

class UserManager {
  DISABLE_COPY(UserManager);
 
 protected:
  /// File system para almacenar usuarios
  FileSystem& fs;
  /// Disco de memoria;
  std::string disk;

 public:
  /// Constructor
  explicit UserManager(FileSystem& fs);
  /// Destructor
  ~UserManager();
  ///
  int addUser();
  ///
  int deleteUser();
  /// 
  void printUser();
  ///
  int changeName(char oldName, char newName);
  ///
  int changePassword(char name, char newPassword);

};

#endif  // USER_MANAGER_2025
