#pragma once

#include <stdint.h>

struct FirmwareHeader {
    uint32_t magic, version, imageSize, crc32;
};

namespace Firmware {
    constexpr uint32_t MAGIC = 0x424D5331;
    constexpr uint32_t HEADER_ADDRESS = 0x8004000UL;
}