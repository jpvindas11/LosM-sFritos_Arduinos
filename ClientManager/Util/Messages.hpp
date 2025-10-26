#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>
#include <bitsery/traits/array.h>
#include <bitsery/brief_syntax/bitset.h>

#include <stdexcept>
#include <variant>
#include <vector>

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
    SEN_FILE_BLOCK_RESP
};

struct token {
    uint32_t id;
    char name[20];
    uint8_t userType;
    uint8_t hour;
    uint8_t minute;
};

struct sensorFileName {
    char sensorType [3];
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
  uint_fast32_t blocks;
};


struct senFileBlockRes {
  uint32_t page;
  uint32_t totalPages;
  uint8_t usedBlocks;
  sensorFileName fileName;
  char firstBlock [1024];
  char secondBlock[1024];
};

struct genMessage {
  MessageType MID;
  std::variant< GenNumReq, fileNumberResp, senFileNamesRes,
                genSenFileReq, senFileMetDRes, senFileBlockNumRes,
                senFileBlockRes
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
      // 1023 bytes. 93 sensorFile names posible
      // (each filename has a 11 bytes size)
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
  void serialize(S& s, genMessage& m) {
      s.value1b(static_cast<uint8_t>(m.MID));
      s.variant(m.content);
  }

}

template <typename T>
T getMessageContent (const genMessage& message) {
  try {
    return std::get<T>(message.content);
  } catch (const std::bad_variant_access&) {
    throw std::runtime_error("ERROR: Could not extract message");
  }
}

#endif // MESSAGES_HPP