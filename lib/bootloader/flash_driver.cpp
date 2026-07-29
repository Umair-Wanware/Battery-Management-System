#include "flash_driver.hpp"
#include "memory_map.hpp"

bool static isApplicationRange(uint32_t address, size_t length){
    if(length == 0){
        return false;
    }
    if(address < MemoryMap::APP_START){
        return false;
    }

    const uint32_t end = address + static_cast<uint32_t>(length);
    if(end < address){
        return false;
    }

    if(end > MemoryMap::FLASH_END){
        return false;
    }

    return true;
}

bool FlashDriver::EraseApplication(){
    constexpr uint32_t PAGE_SIZE = 1024U;
    constexpr uint32_t APP_SIZE = MemoryMap::FLASH_END - MemoryMap::APP_START;
    constexpr uint32_t PAGE_COUNT = APP_SIZE / PAGE_SIZE;

    FLASH_EraseInitTypeDef eraseInit = {0};

    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.PageAddress = MemoryMap::APP_START;
    eraseInit.NbPages = PAGE_COUNT;

    uint32_t PAGE_ERROR = 0;

    HAL_FLASH_Unlock();

    const HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&eraseInit, &PAGE_ERROR);

    HAL_FLASH_Lock();

    return status == HAL_OK;
}

bool FlashDriver::Write(uint32_t address, const uint8_t *data, size_t length){
    if(data == nullptr){
        return false;
    }

    if(!isApplicationRange(address, length)){
        return false;
    }

    if((address & 1U) != 0U){
        return false;
    }

    HAL_FLASH_Unlock();

    size_t offset = 0;

    while(offset < length){
        uint16_t halfword = 0xFFFFU;

        halfword = data[offset];

        if((offset + 1U) < length){
            halfword |= static_cast<uint16_t>(data[offset + 1U] << 8U);
        }
        else {
            halfword |= 0xFF00U;
        }

        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address + static_cast<uint32_t>(offset), halfword) != HAL_OK){
            HAL_FLASH_Lock();
            return false;
        }

        offset += 2;
    }

    HAL_FLASH_Lock();

    return true;
}

bool FlashDriver::Verify(uint32_t address, const uint8_t *data, size_t length){
    if(data == nullptr){
        return false;
    }
    if(!isApplicationRange(address, length)){
        return false;
    }

    const volatile uint8_t *flash = reinterpret_cast<volatile const uint8_t*>(address);
    for(size_t i = 0; i < length; ++i){
        if(flash[i] != data[i]){
            return false;
        }
    }
    return true;
}