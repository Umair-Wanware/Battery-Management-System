#include "spi_transport.hpp"
#include "stm32f1xx_hal.h"

extern "C" {
    #include "spi/spi_drivers.h"
}

SPI_HandleTypeDef hspi;

bool SPITransport::Initialize(){
    return HAL_SPI_Init(&hspi) == HAL_OK;
}

bool SPITransport::Receive(Packet& packet){
    return HAL_SPI_Receive(&hspi, reinterpret_cast<uint8_t*>(&packet), sizeof(Packet), HAL_MAX_DELAY) == HAL_OK;
}

bool SPITransport::Send(const Packet& packet){
    return HAL_SPI_Transmit(&hspi, const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&packet)), sizeof(Packet), HAL_MAX_DELAY) == HAL_OK;
}

bool SPITransport::SendACK(uint16_t sequence){
    Packet packet{};

    packet.header = Protocol::HEADER;
    packet.command = Command::ACK;
    packet.sequence = sequence;
    packet.length = 0;

    return Send(packet);
}

bool SPITransport::SendNACK(uint16_t sequence){
    Packet packet{};

    packet.header = Protocol::HEADER;
    packet.command = Command::NACK;
    packet.sequence = sequence;
    packet.length = 0;

    return Send(packet);
}