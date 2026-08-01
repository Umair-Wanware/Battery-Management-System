#pragma once

#include <stdint.h>

namespace Protocol {
    constexpr uint16_t HEADER = 0x55AA;
    constexpr uint16_t MAX_PAYLOAD = 256;
}

enum class Command : uint8_t {
    HELLO         = 0x01,
    START_UPDATE  = 0x02,
    DATA          = 0x03,
    END_UPDATE    = 0x04,

    ACK           = 0x80,
    NACK          = 0x81,
    ABORT         = 0x82
};

struct Packet {
    uint16_t header;
    Command command;
    uint16_t sequence;
    uint16_t length;
    uint8_t payload[Protocol::MAX_PAYLOAD];
    uint32_t crc32;
};