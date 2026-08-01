#pragma once

#include <stdint.h>

namespace MemoryMap {
    constexpr uint32_t FLASH_START = 0x8000000UL;

    constexpr uint32_t BOOT_START = 0x8000000UL;
    constexpr uint32_t BOOT_SIZE = 16UL * 1024UL;

    constexpr uint32_t APP_START = 0x8004000UL;
    constexpr uint32_t APP_SIZE = FLASH_END - APP_START;

    constexpr uint32_t FLASH_SIZE = 128UL * 1024UL;
    constexpr uint32_t FLASH_END = FLASH_START + FLASH_SIZE;

    constexpr uint32_t SRAM_START = 0x20000000UL;
    constexpr uint32_t SRAM_SIZE = 20UL * 1024UL;
    constexpr uint32_t SRAM_END = SRAM_START + SRAM_SIZE;
}