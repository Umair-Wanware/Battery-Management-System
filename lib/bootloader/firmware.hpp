#pragma once

#include <stdint.h>

#include "memory_map.hpp"

struct FirmwareHeader {
    uint32_t magic, version, imageSize, crc32;
};

namespace Firmware {
    constexpr uint32_t MAGIC = 0x424D5331UL;
    constexpr uint32_t HEADER_ADDRESS = MemoryMap::APP_START;
}