#include "bootloader.hpp"
#include "memory_map.hpp"
#include "firmware_manager.hpp"
#include "flash_driver.hpp"
#include "spi_transport.hpp"
#include "crc32.hpp"
#include <string.h>
#include "stm32f1xx_hal.h"

using ApplicationEntry = void (*)();

bool Bootloader::isApplicationValid() const {
    const uint32_t stackPointer = *reinterpret_cast<const uint32_t*>(MemoryMap::APP_START);
    const uint32_t resetHandler = *reinterpret_cast<const uint32_t*>(MemoryMap::APP_START + 4U);
    const bool validStack = stackPointer >= MemoryMap::SRAM_START && stackPointer < MemoryMap::SRAM_END;
    const uint32_t resetAddress = resetHandler & ~1UL;
    const bool validResetHandler = resetAddress >= MemoryMap::APP_START && resetAddress < MemoryMap::FLASH_END;

    return validStack && validResetHandler;
}

void Bootloader::JumpToApplication(){
    const uint32_t appStack = *reinterpret_cast<const uint32_t*>(MemoryMap::APP_START);
    const uint32_t appResetHandler = *reinterpret_cast<const uint32_t*>(MemoryMap::APP_START + 4U);
    const ApplicationEntry application = reinterpret_cast<ApplicationEntry>(appResetHandler);

    __disable_irq();

    HAL_DeInit();

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    for(uint32_t i = 0; i < 8; ++i){
        NVIC->ICER[i] = 0xFFFFFFFFUL;
        NVIC->ICPR[i] = 0XFFFFFFFFUL;
    }

    SCB->VTOR = MemoryMap::APP_START;
    __set_MSP(appStack);

    __DSB();
    __ISB();

    __enable_irq();

    application();

    while(true){
        
    }
}

void Bootloader::CheckApplication(){
    if(isApplicationValid() && FirmwareManager::isValid() && FirmwareManager::VerifyCRC()){
        state = BootState::WAIT_FOR_UPDATE;
    } else {
        state = BootState::RECEIVE_HEADER;
    }
}

void Bootloader::WaitForUpdate(){
    Packet packet;

    if(SPITransport::Receive(packet)){
        if(packet.command == Command::START_UPDATE){
            state = BootState::RECEIVE_HEADER;
            return;
        }
    }
    HAL_Delay(3000);
    state = BootState::JUMP_APPLICATION;
}

void Bootloader::ReceiveHeader(){
    Packet packet;

    if(!SPITransport::Receive(packet)){
        state = BootState::ERROR;
        return;
    }
    if(packet.header != Protocol::HEADER){
        state = BootState::ERROR;
        return;
    }
    if(packet.command != Command::START_UPDATE){
        state = BootState::ERROR;
        return;
    }
    if(packet.length != sizeof(FirmwareHeader)){
        state = BootState::ERROR;
        return;
    }

    memcpy(&firmwareHeader, packet.payload, sizeof(firmwareHeader));

    if(firmwareHeader.imageSize == 0){
        state = BootState::ERROR;
        return;
    }
    if(firmwareHeader.magic != Firmware::MAGIC){
        state = BootState::ERROR;
        return;
    }

    if(firmwareHeader.imageSize == 0){
        state = BootState::ERROR;
        return;
    }
    if(firmwareHeader.imageSize > MemoryMap::APP_SIZE){
        state = BootState::ERROR;
        return;
    }

    SPITransport::SendACK(packet.sequence);
    writeAddress = MemoryMap::APP_START + sizeof(firmwareHeader);
    bytesReceived = 0;
    expectedSequence = 0;

    state = BootState::ERASE_FLASH;
}

void Bootloader::EraseFlash(){
    if(!FlashDriver::EraseApplication()){
        state = BootState::ERROR;
        return;
    }
    if(!FlashDriver::Write(MemoryMap::APP_START, reinterpret_cast<const uint8_t*>(&firmwareHeader), sizeof(firmwareHeader))){
        state = BootState::ERROR;
        return;
    }
    state = BootState::RECEIVE_IMAGE;
}

void Bootloader::ReceiveImage(){
    Packet packet;

    while(bytesReceived < firmwareHeader.imageSize){
        if(!SPITransport::Receive(packet)){
            state = BootState::ERROR;
            return;
        }
        if(packet.header != Protocol::HEADER){
            SPITransport::SendNACK(packet.sequence);
            continue;
        }
        if(packet.command != Command::DATA){
            state = BootState::ERROR;
            return;
        }
        if(packet.sequence != expectedSequence){
            SPITransport::SendNACK(expectedSequence);
            continue;
        }
        if(packet.length > Protocol::MAX_PAYLOAD){
            state = BootState::ERROR;
            return;
        }
        if(bytesReceived + packet.length > firmwareHeader.imageSize){
            state = BootState::ERROR;
            return;
        }

        const uint32_t crc = CRC32::Calculate(&packet, sizeof(packet) - sizeof(packet.crc32));
        if(crc != packet.crc32){
            SPITransport::SendNACK(packet.sequence);
            continue;
        }
        if(!FlashDriver::Write(writeAddress, packet.payload, packet.length)){
            state = BootState::ERROR;
            return;
        }

        writeAddress += packet.length;
        bytesReceived += packet.length;
        ++expectedSequence;

        SPITransport::SendACK(packet.sequence);
    }
    state = BootState::VERIFY_IMAGE;
}

void Bootloader::VerifyImage(){
    if(FirmwareManager::VerifyCRC()){
        state = BootState::JUMP_APPLICATION;
    } else {
        state = BootState::ERROR;
    }
}

void Bootloader::Run(){
    state = BootState::INIT;

    while(true){
        switch(state){
            case BootState::INIT:
                state = BootState::CHECK_APPLICATION;
                break;

            case BootState::CHECK_APPLICATION:
                CheckApplication();
                break;

            case BootState::WAIT_FOR_UPDATE:
                WaitForUpdate();
                break;

            case BootState::RECEIVE_HEADER:
                ReceiveHeader();
                break;

            case BootState::ERASE_FLASH:
                EraseFlash();
                break;

            case BootState::RECEIVE_IMAGE:
                ReceiveImage();
                break;

            case BootState::VERIFY_IMAGE:
                VerifyImage();
                break;

            case BootState::JUMP_APPLICATION:
                JumpToApplication();
                break;

            case BootState::ERROR:
                HAL_Delay(100);
                break;

            default:
            state = BootState::ERROR;
            break;
        }
    }
}