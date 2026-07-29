#include "bootloader.hpp"
#include "memory_map.hpp"
#include "firmware_manager.hpp"

typedef void (*ApplicationEntry)(void);

void Bootloader::Run(){
    if(isApplicationValid() && FirmwareManager::isValid() && FirmwareManager::VerifyCRC()){
        JumpToApplication();
    }

    while(true){

    }
}

bool Bootloader::isApplicationValid() const {
    const uint32_t stackPointer = *reinterpret_cast<const uint32_t*>(MemoryMap::APP_START);
    const uint32_t resetHandler = *reinterpret_cast<const uint32_t*>(MemoryMap::APP_START + 4U);
    const bool validStack = stackPointer >= MemoryMap::SRAM_START && stackPointer <= MemoryMap::SRAM_END;
    const uint32_t resetAddress = resetHandler & ~1UL;
    const bool validResetHandler = resetAddress >= MemoryMap::APP_START && resetAddress < MemoryMap::FLASH_END;

    return validStack &&validResetHandler;
}

void Bootloader::JumpToApplication(){
    const uint32_t appStack = *reinterpret_cast<const uint32_t*>(MemoryMap::APP_START);
    const uint32_t appResetHandler = *reinterpret_cast<const uint32_t*>(MemoryMap::APP_START + 4U);
    ApplicationEntry application = reinterpret_cast<ApplicationEntry>(appResetHandler);

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

    __DSB();
    __ISB();

    __set_MSP(appStack);

    application();

    while(true){
        
    }
}