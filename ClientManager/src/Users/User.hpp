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
  uint8_t isUsed;       // 1 byte
  char name[20];        // 20 bytes
  char hash[32];        // 32 bytes
  char salt[16];        // 16 bytes
  uint8_t type;         // 1 byte
  uint8_t permissions;  // 1 byte
  uint16_t day;         // 2 bytes
  uint16_t month;       // 2 bytes
  uint32_t year;        // 4 bytes
  uint16_t hour;        // 2 bytes
  uint16_t minute;      // 2 bytes
  char separator;       // 1 byte
} user_t;               // 84 bytes total

#endif  // USER_STRUCT_2025B
