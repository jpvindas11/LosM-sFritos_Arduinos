/// @copyright Los Más Fritos - 2025

#ifndef USER_STRUCT_2025B
#define USER_STRUCT_2025B

#include <limits.h>
#include <cstdint>
#include <string>

#define EMPTY_USER -1
#define SEPARATOR '/'
#define FILLER '*'

// Tipos de usuarios
enum userTypes {
  CONSULTANT = 1,
  USER_ADMIN,
  SOFTWARE_ADMIN,
  HARDWARE_ADMIN
};

// Permisos
#define PERM_AUTH_READ      0x01
#define PERM_AUTH_WRITE     0x02
#define PERM_SENSOR_READ    0x04
#define PERM_SENSOR_WRITE   0x08
#define PERM_LOG_READ       0x10
#define PERM_LOG_WRITE      0x20
#define PERM_USERDATA_READ  0x40
#define PERM_USERDATA_WRITE 0x80

// Permisos por tipo de usuario
#define PERMS_SOFTWARE_ADMIN   (PERM_AUTH_READ | PERM_AUTH_WRITE \
                              | PERM_SENSOR_READ | PERM_SENSOR_WRITE \
                              | PERM_LOG_READ | PERM_LOG_WRITE \
                              | PERM_USERDATA_READ | PERM_USERDATA_WRITE)
#define PERMS_HARDWARE_ADMIN   (PERM_SENSOR_READ | PERM_SENSOR_WRITE)
#define PERMS_USER_ADMIN       (PERM_USERDATA_READ | PERM_USERDATA_WRITE)
#define PERMS_CONSULTANT       (PERM_SENSOR_READ | PERM_LOG_READ)

/// Estructura de usuario
typedef struct User {
  char isUsed;       // 1 byte
  char name[20];     // 20 bytes
  char hash[64];     // 64 bytes
  char salt[32];     // 32 bytes
  char type;         // 1 byte
  char permissions;  // 1 byte
  char day [2];      // 2 bytes
  char month [2];    // 2 bytes
  char hour [2];     // 2 bytes
  char minute [2];   // 2 bytes
  char separator;    // 1 byte
} user_t;            // 128 bytes total

#endif  // USER_STRUCT_2025B
