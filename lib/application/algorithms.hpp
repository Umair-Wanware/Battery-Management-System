#pragma once

#include <stdint.h>
#include "sensor_packet.hpp"

class Algorithms {
    public:
    uint16_t calculateCRC16(const uint8_t* data, uint16_t length);
    uint8_t checkFault(const SensorPacket& packet);
    uint8_t calculateSOC(float voltage);
    float ADC_ToCurr(uint16_t adc);
    float ADC_ToVolt(uint16_t adc);
    float ADC_ToTemp(uint16_t adc);
};