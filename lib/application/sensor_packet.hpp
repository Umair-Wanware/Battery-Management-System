#pragma once

#include <stdint.h>

struct SensorPacket {
    uint16_t header, crc;
    float temperature, voltage, current;
    uint8_t fault, soc;
};