#include "crc32.hpp"

uint32_t CRC32::Calculate(const void* data, size_t length){
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    uint32_t crc = 0xFFFFFFFFUL;

    while(length--){
        crc ^= *bytes++;
        for(uint8_t i = 0; i < 8; i++){
            if(crc & 1U){
                crc = (crc >> 1U) ^ Polynomial;
            } else {
                crc >>= 1U;
            }
        }
    }
    return ~crc;
}
