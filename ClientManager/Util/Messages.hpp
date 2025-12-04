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
#include "User.hpp"

#define MAX_COMMON_MSG_LENGTH 32

enum class ServerType: uint8_t {
    SV_MASTER,
    SV_AUTH,
    SV_LOGS_USER,
    SV_STORAGE,
    SV_SENSOR_PROXY,
};

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
    SEN_RECENT_DATA_REQ,
    SEN_RECENT_DATA_RES,
    ADD_SENSOR,
    DELETE_SENSOR,
    MODIFY_SENSOR,
    AUTH_LOGIN_REQ,
    AUTH_LOGIN_SUCCESS,
    AUTH_LOGOUT,
    AUTH_USER_CREATE,
    AUTH_USER_DELETE,
    AUTH_USER_MODIFY_PASS,
    AUTH_USER_MODIFY_RANK,
    AUTH_USER_REQUEST,
    AUTH_USER_RESPONSE,
    ERR_COMMOM_MSG,
    OK_COMMON_MSG,
    LOG_USER_REQUEST,
    LOG_USER_RESP,
    ADD_USER_LOG,
    DELETE_USER_LOGS,
    SERVER_STATUS_REQ,
    SERVER_STATUS_RES,
    SERVER_DISCOVER_REQ,
    SERVER_DISCOVER_RES,
    RAID_PAIR_REQUEST,
    RAID_PAIR_RESPONSE,
    RAID_SYNC_START,
    RAID_SYNC_FILE,
    RAID_SYNC_COMPLETE,
    RAID_HEARTBEAT,
    RAID_PROMOTE,
};

struct token {
    uint32_t id;
    std::string name;
    uint8_t userType;
    uint8_t hour;
    uint8_t minute;
};

struct sensorRecentData {
  std::string ip;
  std::string sensorType;
  std::string data;
  uint32_t lastModified;
};

struct sensorFileName {
    std::string sensorType;
    uint16_t id;
    uint16_t year;
    uint8_t month;
    uint8_t day;
};

struct forNamesRequest {
  std::string Filename;
};

struct sensorNamesVector {
  std::vector<forNamesRequest> names;
};

struct GenNumReq {
    uint32_t id_token;
};

struct fileNumberResp {
    uint32_t id_token;
    uint32_t totalFiles;
};

struct senFileNamesRes {
    uint32_t id_token;
    uint32_t page;
    uint32_t totalPages;
    sensorNamesVector fileNames;
};

struct genSenFileReq {
    uint32_t id_token;
    forNamesRequest fileName;
};

struct senFileMetDRes {
    uint32_t id_token;
    forNamesRequest fileName;
    uint32_t size;
    uint16_t permissions;
    uint32_t userId;
    uint32_t groupId;
    uint32_t creationTime;
    uint32_t lastModifiedTime;
    uint32_t lastAccessTime;
};

struct senFileBlockNumRes {
    uint32_t id_token;
    forNamesRequest fileName;
    uint32_t blocks;
};

struct senFileBlockRes {
    uint32_t id_token;
    uint32_t page;
    uint32_t totalPages;
    uint8_t usedBlocks;
    forNamesRequest fileName;
    std::string firstBlock;
    std::string secondBlock;
};

struct senAddLog {
    sensorFileName fileName;
    std::string data;
    std::string originIP;
};

struct senRecentDataRes {
    std::vector<sensorRecentData> recentData;
};

struct addSensor {
    // the file should be store as "name_id.txt"
    uint32_t id_token;
    // 3 letters "TEM","DIS","VIB", etc
    std::string name;
    uint16_t id;
    uint8_t state;
    uint16_t addition_year;
    uint8_t addition_moth;
    uint8_t addition_day;
    uint16_t last_send_year;
    uint8_t last_send_moth;
    uint8_t last_send_day;
    std::string added_by;
};

struct deleteSensor {
    uint32_t id_token;
    // 0 = NO, 1 = YES
    uint8_t delete_logs;
    std::string name;

};

struct modifySensorInfp {
    uint32_t id_token;
    // 1 = YES, 0 = NO so we modify the last time that the sensor sent data
    uint8_t modifyState;
    // always set to 0 those values that will not be used
    uint8_t newState;
    uint16_t last_send_year;
    uint8_t last_send_moth;
    uint8_t last_send_day;
    std::string name;
};


struct authLoginReq {
    std::string user;
    std::string pass;
};

struct authLoginSuccess {
    token Token;
};

struct authLogout {
    std::string user;
};

struct authCreateUser {
    std::string newUser;
    std::string pass;
    uint8_t rank;
};

struct authDeleteUser {
    std::string deleteUser;
};

struct authModifyUserPass {
    std::string user;
    std::string newPassword;
};

struct authModifyUserRank {
    std::string user;
    uint8_t rank;
};

struct authRequestUsers {
    std::vector<UserInfo> users;
};

struct errorCommonMsg {
    std::string message;
};

struct okCommonMsg {
    std::string message;
};

// for LOG_USER_REQUEST and DELETE_USER_LOGS
struct userLogRequestCommon {
    uint32_t id_token;
    std::string userName;
};

struct userLogResp {
    uint32_t id_token;
    std::string userName;
    uint32_t page;
    uint32_t totalPages;
    std::string firstBlock;
    std::string secondBlock;
};

struct addUserLog {
    uint32_t id_token;
    std::string userName;
    std::string logInfo;
};

struct serverStatus {
    std::string serverName;
    std::string serverIP;
    uint16_t serverPort;
    uint8_t isConnected;
    uint32_t lastCheck;
};

struct serverStatusReq {
    uint32_t id_token;
};

struct serverStatusRes {
    uint32_t id_token;
    std::vector<serverStatus> servers;
};

struct serverDiscoverReq {
    std::string clientName;
};

struct serverDiscoverRes {
    std::string serverName;
    std::string serverIP;
    uint8_t serverType;
    uint8_t raidMode;
};

struct raidPairRequest {
    std::string serverName;
    std::string serverIP;
    uint8_t serverType;
};

struct raidPairResponse {
    std::string serverName;
    std::string serverIP;
    uint8_t hasPair;
};

struct raidSyncFile {
    std::string fileName;
    uint32_t fileSize;
    std::string fileData;
    uint32_t chunkNumber;
    uint32_t totalChunks;
};

struct raidSyncComplete {
    uint32_t totalFiles;
    uint32_t totalBytes;
};

struct raidHeartbeat {
    uint64_t timestamp;
    uint8_t isPrimary;
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
        senRecentDataRes,
        senFileBlockRes,
        addSensor,
        deleteSensor,
        modifySensorInfp,
        authLoginReq,
        authLoginSuccess,
        authLogout,
        authCreateUser,
        authDeleteUser,
        authModifyUserPass,
        authModifyUserRank,
        authRequestUsers,
        errorCommonMsg,
        okCommonMsg,
        userLogRequestCommon,
        userLogResp,
        addUserLog,
        serverStatusReq,
        serverStatusRes,
        serverDiscoverReq,
        serverDiscoverRes,
        raidPairRequest,
        raidPairResponse,
        raidSyncFile,
        raidSyncComplete,
        raidHeartbeat
    > content;
};

namespace bitsery {

    template <typename S>
    void serialize(S& s, token& t) {
        s.value4b(t.id);
        s.text1b(t.name, USER_NAME_SIZE);
        s.value1b(t.userType);
        s.value1b(t.hour);
        s.value1b(t.minute);
    }

    template <typename S>
    void serialize(S& s, UserInfo& ui) {
        s.text1b(ui.user, USER_NAME_SIZE);
        s.value1b(ui.rank);
        s.value1b(ui.isConnected);
    }

    template <typename S>
    void serialize(S& s, sensorRecentData& ui) {
        s.text1b(ui.ip, 16);
        s.text1b(ui.sensorType, 16);
        s.container1b(ui.data, 256);
        s.value4b(ui.lastModified);
    }

    template <typename S>
    void serialize(S& s, sensorFileName& sf) {
        s.text1b(sf.sensorType, 16);
        s.value2b(sf.id);
        s.value2b(sf.year);
        s.value1b(sf.month);
        s.value1b(sf.day);
    }

    template <typename S>
    void serialize(S& s, forNamesRequest& fnr) {
        s.text1b(fnr.Filename, 25);
    }

    template <typename S>
    void serialize(S& s, sensorNamesVector& snv) {
        s.container(snv.names, 50);
    }

    template <typename S>
    void serialize(S& s, GenNumReq& m) {
        s.value4b(m.id_token);
    }

    template <typename S>
    void serialize(S& s, fileNumberResp& m) {
        s.value4b(m.id_token);
        s.value4b(m.totalFiles);
    }

    template <typename S>
    void serialize(S& s, senFileNamesRes& m) {
        s.value4b(m.id_token);
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
        s.value4b(m.id_token);
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
        s.value4b(m.id_token);
        s.object(m.fileName);
        s.value4b(m.blocks);
    }

    template <typename S>
    void serialize(S& s, senFileBlockRes& m) {
        s.value4b(m.id_token);
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
        s.text1b(m.originIP, 16);
    }

    template <typename S>
    void serialize(S& s, senRecentDataRes& m) {
        s.container(m.recentData, 20);
    }

    template <typename S>
    void serialize(S& s, addSensor& m) {
        s.value4b(m.id_token);
        s.text1b(m.name, 3);
        s.value2b(m.id);
        s.value1b(m.state);
        s.value2b(m.addition_year);
        s.value1b(m.addition_moth);
        s.value1b(m.addition_day);
        s.value2b(m.last_send_year);
        s.value1b(m.last_send_moth);
        s.value1b(m.last_send_day);
        s.text1b(m.added_by, 20);
    }

    template <typename S>
    void serialize(S& s, deleteSensor& m) {
        s.value4b(m.id_token);
        s.value1b(m.delete_logs);
        s.text1b(m.name, 15);
    }

    template <typename S>
    void serialize(S& s, modifySensorInfp& m) {
        s.value4b(m.id_token);
        s.value1b(m.modifyState);
        s.value1b(m.newState);
        s.value2b(m.last_send_year);
        s.value1b(m.last_send_moth);
        s.value1b(m.last_send_day);
    }

    template <typename S>
    void serialize(S& s, authLoginReq& m) {
        s.text1b(m.user, USER_NAME_SIZE);
        s.text1b(m.pass, USER_PASSWORD_SIZE_MAX);
    }

    template <typename S>
    void serialize(S& s, authLoginSuccess& m) {
        s.object(m.Token);
    }

    template <typename S>
    void serialize(S& s, authLogout& m) {
        s.text1b(m.user, USER_NAME_SIZE);
    }

    template <typename S>
    void serialize(S& s, authCreateUser& m) {
        s.text1b(m.newUser, USER_NAME_SIZE);
        s.text1b(m.pass, USER_PASSWORD_SIZE_MAX);
        s.value1b(m.rank);
    }

    template <typename S>
    void serialize(S& s, authDeleteUser& m) {
        s.text1b(m.deleteUser, USER_NAME_SIZE);
    }

    template <typename S>
    void serialize(S& s, authModifyUserPass& m) {
        s.text1b(m.user, USER_NAME_SIZE);
        s.text1b(m.newPassword, USER_PASSWORD_SIZE_MAX);
    }

    template <typename S>
    void serialize(S& s, authModifyUserRank& m) {
        s.text1b(m.user, USER_NAME_SIZE);
        s.value1b(m.rank);
    }

    template <typename S>
    void serialize(S& s, authRequestUsers& m) {
        s.container(m.users, 100);
    }

    template <typename S>
    void serialize(S& s, errorCommonMsg& m) {
        s.text1b(m.message, MAX_COMMON_MSG_LENGTH);
    }

    template <typename S>
    void serialize(S& s, okCommonMsg& m) {
        s.text1b(m.message, MAX_COMMON_MSG_LENGTH);
    }

    template <typename S>
    void serialize(S& s, userLogRequestCommon& m) {
        s.value4b(m.id_token);
        s.text1b(m.userName, 25);
    }

    template <typename S>
    void serialize(S& s, userLogResp& m) {
        s.value4b(m.id_token);
        s.text1b(m.userName, 25);
        s.value4b(m.page);
        s.value4b(m.totalPages);
        s.text1b(m.firstBlock, 1024);
        s.text1b(m.secondBlock, 1024);
    }

    template <typename S>
    void serialize(S& s, addUserLog& m) {
        s.value4b(m.id_token);
        s.text1b(m.userName, 25);
        s.text1b(m.logInfo, 1024);
    }

    template <typename S>
    void serialize(S& s, genMessage& m) {
        auto& mid = m.MID;
        s.value1b(mid);
        s.ext(m.content, bitsery::ext::StdVariant{});
    }

    template <typename S>
    void serialize(S& s, serverStatus& ss) {
        s.text1b(ss.serverName, 20);
        s.text1b(ss.serverIP, 16);
        s.value2b(ss.serverPort);
        s.value1b(ss.isConnected);
        s.value4b(ss.lastCheck);
    }

    template <typename S>
    void serialize(S& s, serverStatusReq& m) {
        s.value4b(m.id_token);
    }

    template <typename S>
    void serialize(S& s, serverStatusRes& m) {
        s.value4b(m.id_token);
        s.container(m.servers, 10);
    }

    template <typename S>
    void serialize(S& s, serverDiscoverReq& m) {
        s.text1b(m.clientName, 20);
    }

    template <typename S>
    void serialize(S& s, serverDiscoverRes& m) {
        s.text1b(m.serverName, 20);
        s.text1b(m.serverIP, 16);
        s.value1b(m.serverType);
        s.value1b(m.raidMode); 
    }

    template <typename S>
    void serialize(S& s, raidPairRequest& m) {
        s.text1b(m.serverName, 20);
        s.text1b(m.serverIP, 16);
        s.value1b(m.serverType);
    }

    template <typename S>
    void serialize(S& s, raidPairResponse& m) {
        s.text1b(m.serverName, 20);
        s.text1b(m.serverIP, 16);
        s.value1b(m.hasPair);
    }

    template <typename S>
    void serialize(S& s, raidSyncFile& m) {
        s.text1b(m.fileName, 50);
        s.value4b(m.fileSize);
        s.text1b(m.fileData, 1024);
        s.value4b(m.chunkNumber);
        s.value4b(m.totalChunks);
    }

    template <typename S>
    void serialize(S& s, raidSyncComplete& m) {
        s.value4b(m.totalFiles);
        s.value4b(m.totalBytes);
    }

    template <typename S>
    void serialize(S& s, raidHeartbeat& m) {
        s.value8b(m.timestamp);
        s.value1b(m.isPrimary);
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
