/// @copyright Los Más Fritos - 2025

#ifndef USER_HPP
#define USER_HPP

#include <cstdint>

#define FILLER '*'
#define USER_NAME_SIZE 20
#define USER_PASSWORD_SIZE_MAX 32
#define USER_HASH_SIZE 64  // Hexadecimal representation (32 bytes = 64 hex chars)
#define USER_SALT_SIZE 32  // Hexadecimal representation (16 bytes = 32 hex chars)

enum USER_TYPES {
    USER_OWNER = 'o',
    USER_HARDWAREMANAGER = 'h',
    USER_SOFTWAREMANAGER = 's',
    USER_USERMANAGER = 'u',
    USER_CONSULTANT = 'c',
};

/**
 * @brief Estructura que representa un usuario en el sistema
 * 
 * Total size: 128 bytes
 * Layout optimizado para almacenamiento en bloques de 256 bytes (2 usuarios por bloque)
 */
typedef struct __attribute__((packed)) user_t {
    char isUsed;                    // '1' = usado, '0' = libre (1 byte)
    char name[USER_NAME_SIZE];      // Nombre de usuario, rellenado con FILLER (20 bytes)
    char hash[USER_HASH_SIZE];      // Hash de contraseña en hexadecimal (64 bytes)
    char salt[USER_SALT_SIZE];      // Salt en hexadecimal (32 bytes)
    char type;                      // Tipo de usuario/rank (1 byte)
    char permissions;               // Permisos (1 byte)
    char day[2];                    // Día de creación (2 bytes)
    char month[2];                  // Mes de creación (2 bytes)
    char hour[2];                   // Hora de creación (2 bytes)
    char minute[2];                 // Minuto de creación (2 bytes)
    char separator;                 // Separador '/' (1 byte)
    // Total: 128 bytes
} user_t;

static_assert(sizeof(user_t) == 128, "user_t must be exactly 128 bytes");

#endif // USER_HPP
