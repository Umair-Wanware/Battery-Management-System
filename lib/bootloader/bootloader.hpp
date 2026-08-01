#pragma once

#include "stm32f1xx_hal.h"
#include "boot_state.hpp"
#include "firmware_manager.hpp"
#include "memory_map.hpp"

class Bootloader {
    public:
    void Run();

    private:
    bool isApplicationValid() const;
    void JumpToApplication();

    BootState state;

    void CheckApplication();
    void WaitForUpdate();
    void ReceiveHeader();
    void EraseFlash();
    void ReceiveImage();
    void VerifyImage();
    void Jump();

    BootState state = BootState::INIT;

    uint32_t writeAddress = MemoryMap::APP_START + sizeof(FirmwareHeader);
    uint32_t bytesReceived = 0;
    uint16_t expectedSequence = 0;

    FirmwareHeader firmwareHeader{};
};