#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include "../../common/bitsery/include/bitsery/bitsery.h"
#include <../../common/bitsery/include/bitsery/adapter/buffer.h>
#include <../../common/bitsery/include/bitsery/traits/string.h>
#include <../../common/bitsery/include/bitsery/traits/vector.h>
#include <../../common/bitsery/include/bitsery/traits/array.h>
#include <../../common/bitsery/include/bitsery/brief_syntax/bitset.h>
#include <../../common/bitsery/include/bitsery/ext/std_variant.h>

#include <stdexcept>
#include <variant>
#include <vector>

#define MAX_ERR_MSG_LENGTH 32

enum class MessageType : uint8_t {
    FILE_NUMBER_REQ,
    FILE_NUMBER_REP,
    SEN_FILE_NAMES_REQ,
    SEN_FILE_NAMES_RES,
    SEN_FILE_METD_REQ,
    SEN_FILE_METD_RES,
    SEN_FILE_BLOCKNUM_REQ,
    SEN_FILE_BLOCKNUM_RES,
    SEN_FILE_BLOCK_REQ,
    SEN_FILE_BLOCK_RESP,
    SEN_ADD_LOG,
    AUTH_LOGIN_REQ,
    AUTH_LOGIN_SUCCESS,
    ERR_COMMOM_MSG,
};

struct token {
    uint32_t id;
    std::string name;
    uint8_t userType;
    uint8_t hour;
    uint8_t minute;
};

struct sensorFileName {
    std::string sensorType;
    uint16_t id;
    uint16_t year;
    uint8_t month;
    uint8_t day;
};

struct sensorNamesVector {
    std::vector<sensorFileName> names;
};

struct GenNumReq {
    uint32_t id_token;
};

struct fileNumberResp {
    uint32_t totalFiles;
};

struct senFileNamesRes {
    uint32_t page;
    uint32_t totalPages;
    sensorNamesVector fileNames;
};

struct genSenFileReq {
    uint32_t id_token;
    sensorFileName fileName;
};

struct senFileMetDRes {
    sensorFileName fileName;
    uint32_t size;
    uint16_t permissions;
    uint32_t userId;
    uint32_t groupId;
    uint32_t creationTime;
    uint32_t lastModifiedTime;
    uint32_t lastAccessTime;
};

struct senFileBlockNumRes {
    sensorFileName fileName;
    uint32_t blocks;
};

struct senFileBlockRes {
    uint32_t page;
    uint32_t totalPages;
    uint8_t usedBlocks;
    sensorFileName fileName;
    std::string firstBlock;
    std::string secondBlock;
};

struct senAddLog {
    sensorFileName fileName;
    std::string data;
};

struct authLoginReq {
    std::string user;
    std::string pass;
};

struct authLoginSuccess {
    token Token;
};

struct errorCommonMsg {
    std::string message;
};

struct genMessage {
    uint8_t MID;
    std::variant<
        GenNumReq,
        fileNumberResp,
        senFileNamesRes,
        genSenFileReq,
        senFileMetDRes,
        senFileBlockNumRes,
        senAddLog,
        senFileBlockRes,
        authLoginReq,
        authLoginSuccess,
        errorCommonMsg
    > content;
};

namespace bitsery {

    template <typename S>
    void serialize(S& s, token& t) {
        s.value4b(t.id);
        s.text1b(t.name, 20);
        s.value1b(t.userType);
        s.value1b(t.hour);
        s.value1b(t.minute);
    }

    template <typename S>
    void serialize(S& s, sensorFileName& sf) {
        s.text1b(sf.sensorType, 3);
        s.value2b(sf.id);
        s.value2b(sf.year);
        s.value1b(sf.month);
        s.value1b(sf.day);
    }

    template <typename S>
    void serialize(S& s, sensorNamesVector& snv) {
        s.container(snv.names, 93);
    }

    template <typename S>
    void serialize(S& s, GenNumReq& m) {
        s.value4b(m.id_token);
    }

    template <typename S>
    void serialize(S& s, fileNumberResp& m) {
        s.value4b(m.totalFiles);
    }

    template <typename S>
    void serialize(S& s, senFileNamesRes& m) {
        s.value4b(m.page);
        s.value4b(m.totalPages);
        s.object(m.fileNames);
    }

    template <typename S>
    void serialize(S& s, genSenFileReq& m) {
        s.value4b(m.id_token);
        s.object(m.fileName);
    }

    template <typename S>
    void serialize(S& s, senFileMetDRes& m) {
        s.object(m.fileName);
        s.value4b(m.size);
        s.value2b(m.permissions);
        s.value4b(m.userId);
        s.value4b(m.groupId);
        s.value4b(m.creationTime);
        s.value4b(m.lastModifiedTime);
        s.value4b(m.lastAccessTime);
    }

    template <typename S>
    void serialize(S& s, senFileBlockNumRes& m) {
        s.object(m.fileName);
        s.value4b(m.blocks);
    }

    template <typename S>
    void serialize(S& s, senFileBlockRes& m) {
        s.value4b(m.page);
        s.value4b(m.totalPages);
        s.value1b(m.usedBlocks);
        s.object(m.fileName);
        s.text1b(m.firstBlock, 1024);
        s.text1b(m.secondBlock, 1024);
    }

    template <typename S>
    void serialize(S& s, senAddLog& m) {
        s.object(m.fileName);
        s.text1b(m.data, 256);
    }

    template <typename S>
    void serialize(S& s, authLoginReq& m) {
        s.text1b(m.user, 28);
        s.text1b(m.pass, 28);
    }

    template <typename S>
    void serialize(S& s, authLoginSuccess& m) {
        s.object(m.Token);
    }

    template <typename S>
    void serialize(S& s, errorCommonMsg& m) {
        s.text1b(m.message, MAX_ERR_MSG_LENGTH);
    }

    template <typename S>
    void serialize(S& s, genMessage& m) {
        auto& mid = m.MID;
        s.value1b(mid);
        s.ext(m.content, bitsery::ext::StdVariant{});
    }

}

template <typename T>
T getMessageContent(const genMessage& message) {
    try {
        return std::get<T>(message.content);
    } catch (const std::bad_variant_access&) {
        throw std::runtime_error("ERROR: Could not extract message");
    }
}

#endif // MESSAGES_HPP
