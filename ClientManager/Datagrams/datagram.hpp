/// @copyright Los Más Fritos - 2025

#ifndef DATAGRAMS_HPP
#define DATAGRAMS_HPP

#include <inttypes.h>
#include "../Users/src/User.hpp"
#include "../FileSystem/src/FileSystemStructures.hpp"

typedef enum MessageID {
    MID_AUTH_REQUEST,
    MID_AUTH_SUCCESS,
    MID_ERROR,

    MID_NODE_CONNECTED_REQUEST,
    MID_NODE_CONNECTED_RESPONSE,
    MID_NODE_INFO_REQUEST,
    MID_NODE_INFO_RESPONSE,

    MID_FILE_METADATA_REQUEST,
    MID_FILE_METADATA_RESPONSE,
    MID_FILE_BLOCKAMOUNT_REQUEST,
    MID_FILE_BLOCKAMOUNT_RESPONSE,
    MID_FILE_BLOCKS_REQUEST,
    MID_FILE_BLOCKS_RESPONSE,
    MID_FILE_WRITE_REQUEST,
    MID_FILE_WRITE_RESPONSE,

    MID_USER_AMOUNT_REQUEST,
    MID_USER_AMOUNT_RESPONSE,
    MID_USER_INFO_REQUEST,
    MID_USER_INFO_RESPONSE,
    MID_USER_MODIFY_REQUEST,
    MID_USER_MODIFY_RESPONSE,
} MessageID;

/**
 * Definir las estructuras según los datagramas
 * Así al copiar la información exacta a un buffer, se enviarán como el diseño
 */

typedef struct midAuthRequest {
    uint8_t mid = MID_AUTH_REQUEST;
    char user[USER_NAME_SIZE];
    char pass[USER_PASSWORD_SIZE_MAX];
} midAuthRequest;

typedef struct midAuthSuccess {
    uint8_t mid = MID_AUTH_SUCCESS;
    char message[32];

    // Mandamos por aqui el token más adelante
} midAuthSuccess;

typedef struct midErrorMessage {
    uint8_t mid = MID_ERROR;
    char message[32];
} midErrorMessage;

#endif