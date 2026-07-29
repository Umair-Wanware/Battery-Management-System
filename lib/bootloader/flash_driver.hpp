#pragma once

#include "stm32f1xx_hal.h"
#include <cstddef>
#include <cstdint>

class FlashDriver {
    public:
    static bool EraseApplication();
    static bool Write(uint32_t address, const uint8_t *data, size_t length);
    static bool Verify(uint32_t address, const uint8_t *data, size_t length);
};