#pragma once

#include <stdint.h>
#include <stddef.h>

class CRC32 {
    public:
    static uint32_t Calculate(const void* data, size_t length);

    private:
    static constexpr uint32_t Polynomial = 0xEDB88320UL;
};