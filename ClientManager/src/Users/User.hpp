/// @copyright Los Más Fritos - 2025

#ifndef USER_STRUCT_2025B
#define USER_STRUCT_2025B

#include <limits.h>
#include <cstdint>
#include <string>

#define EMPTY_USER -1
#define SEPARATOR '/'
#define FILLER '*'

/// Estructura de usuario
typedef struct User {
  char isUsed;       // 1 byte
  char name[20];        // 20 bytes
  char hash[64];        // 64 bytes
  char salt[32];        // 32 bytes
  char type;         // 1 byte
  char permissions;  // 1 byte
  char day [2];         // 2 bytes
  char month [2];       // 2 bytes
  char hour [2];        // 2 bytes
  char minute [2];      // 2 bytes
  char separator;       // 1 byte
} user_t;               // 128 bytes total

#endif  // USER_STRUCT_2025B
