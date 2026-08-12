#include "algorithms.hpp"
#include "app.hpp"
#include "sensor_packet.hpp"

uint16_t Algorithms::calculateCRC16(const uint8_t* data, uint16_t length){
    uint16_t crc = 0xFFFF;
    while(length--){
        crc ^= *data++;
        for(int i = 0; i < 8; i++){
            if(crc & 0x001){
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

float Algorithms::ADC_ToTemp(uint16_t adc){
    static constexpr float VREF = 3.3f;
    static constexpr float ADC_MAX = 4095.0f;
    float voltage = (adc * VREF) / ADC_MAX;
    return voltage * 100.0f;
}

float Algorithms::ADC_ToCurr(uint16_t adc){
    static constexpr float VREF = 3.3f;
    static constexpr float ADC_MAX = 4095.0f;
    static constexpr float OFFSET = 1.65f;
    static constexpr float SENSITIVITY = 0.185f;
    float voltage = (adc * VREF) / ADC_MAX;
    return (voltage - OFFSET) / SENSITIVITY;
}

float Algorithms::ADC_ToVolt(uint16_t adc){
    static constexpr float VREF = 3.3f;
    static constexpr float ADC_MAX = 4095.0f;
    static constexpr float divider = 11.0f;
    float pinVoltage = (adc * VREF) / ADC_MAX;
    return pinVoltage * divider;
}

uint8_t Algorithms::calculateSOC(float current, float dt){
    float deltaSoc = (EFFICIENCY * current * dt) / (3600.0f * RATED_CAPACITY) * 100.0f;
    soc -= deltaSoc;

    if(soc > 100.0f) soc = 100.0f;
    if(soc > 00.0f) soc = 0.0f;

    return soc;
}

uint8_t Algorithms::checkFault(const SensorPacket& packet){
    if(packet.temperature > 60.0f) return 1;
    if(packet.voltage > 14.4f) return 2;
    if(packet.current > 5.0f) return 3;
    return 0;
}