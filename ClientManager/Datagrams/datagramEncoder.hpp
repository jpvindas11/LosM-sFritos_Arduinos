/// @copyright Los Más Fritos - 2025

#ifndef DATAGRAMSENCODER_HPP
#define DATAGRAMSENCODER_HPP

#include "datagram.hpp"
#include <cstring>

/**
 * @brief Codifica una solicitud de autenticación en un buffer
 * @param mid Estructura midAuthRequest a codificar
 * @param buffer Buffer donde se escribirá el mensaje codificado
 * @param bufferSize Tamaño del buffer
 * @return Número de bytes escritos, o -1 en caso de error
 */
inline int encodeAuthRequest(const midAuthRequest* mid, char* buffer, size_t bufferSize) {
    if (bufferSize < sizeof(midAuthRequest)) {
        return -1;
    }

    memcpy(buffer, mid, sizeof(midAuthRequest));
    return sizeof(midAuthRequest);
}

/**
 * @brief Decodifica un buffer en una solicitud de autenticación
 * @param buffer Buffer con el mensaje
 * @param bufferSize Tamaño del buffer
 * @param mid Estructura donde se almacenará el resultado
 * @return true si la decodificación fue exitosa, false en caso contrario
 */
inline bool decodeAuthRequest(const char* buffer, size_t bufferSize, midAuthRequest* mid) {
    if (bufferSize < sizeof(midAuthRequest)) {
        return false; // Buffer muy pequeño
    }
    
    // Copiar del buffer a la estructura
    memcpy(mid, buffer, sizeof(midAuthRequest));
    
    // Verificar que el MID sea correcto
    if (mid->mid != MID_AUTH_REQUEST) {
        return false;
    }
    
    return true;
}

/**
 * @brief Codifica una respuesta de autenticación exitosa
 * @param mid Estructura midAuthSuccess a codificar
 * @param buffer Buffer donde se escribirá el mensaje
 * @param bufferSize Tamaño del buffer
 * @return Número de bytes escritos, o -1 en caso de error
 */
inline int encodeAuthSuccess(const midAuthSuccess* mid, char* buffer, size_t bufferSize) {
    if (bufferSize < sizeof(midAuthSuccess)) {
        return -1;
    }
    
    memcpy(buffer, mid, sizeof(midAuthSuccess));
    return sizeof(midAuthSuccess);
}

/**
 * @brief Decodifica una respuesta de autenticación exitosa
 * @param buffer Buffer con el mensaje
 * @param bufferSize Tamaño del buffer
 * @param mid Estructura donde se almacenará el resultado
 * @return true si la decodificación fue exitosa, false en caso contrario
 */
inline bool decodeAuthSuccess(const char* buffer, size_t bufferSize, midAuthSuccess* mid) {
    if (bufferSize < sizeof(midAuthSuccess)) {
        return false;
    }
    
    memcpy(mid, buffer, sizeof(midAuthSuccess));
    
    if (mid->mid != MID_AUTH_SUCCESS) {
        return false;
    }
    
    return true;
}

/**
 * @brief Codifica un mensaje de error
 * @param mid Estructura midErrorMessage a codificar
 * @param buffer Buffer donde se escribirá el mensaje
 * @param bufferSize Tamaño del buffer
 * @return Número de bytes escritos, o -1 en caso de error
 */
inline int encodeErrorMessage(const midErrorMessage* mid, char* buffer, size_t bufferSize) {
    if (bufferSize < sizeof(midErrorMessage)) {
        return -1;
    }
    
    memcpy(buffer, mid, sizeof(midErrorMessage));
    return sizeof(midErrorMessage);
}

/**
 * @brief Decodifica un mensaje de error
 * @param buffer Buffer con el mensaje
 * @param bufferSize Tamaño del buffer
 * @param mid Estructura donde se almacenará el resultado
 * @return true si la decodificación fue exitosa, false en caso contrario
 */
inline bool decodeErrorMessage(const char* buffer, size_t bufferSize, midErrorMessage* mid) {
    if (bufferSize < sizeof(midErrorMessage)) {
        return false;
    }
    
    memcpy(mid, buffer, sizeof(midErrorMessage));
    
    if (mid->mid != MID_ERROR) {
        return false;
    }
    
    return true;
}

/**
 * @brief Obtiene el (MID) del mensaje
 * @param buffer Buffer con el mensaje
 * @param bufferSize Tamaño del buffer
 * @return El MessageID, o -1 si el buffer es inválido
 */
inline int getMessageID(const char* buffer, size_t bufferSize) {
    if (bufferSize < 1) {
        return -1;
    }
    
    return static_cast<int>(static_cast<uint8_t>(buffer[0]));
}

#endif