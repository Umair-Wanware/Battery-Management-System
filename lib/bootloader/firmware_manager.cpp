#include "firmware_manager.hpp"
#include "firmware.hpp"
#include "memory_map.hpp"
#include "crc32.hpp"

const FirmwareHeader* FirmwareManager::Header(){
    return reinterpret_cast<const FirmwareHeader*>(MemoryMap::APP_START);
}

bool FirmwareManager::isValid(){
    const FirmwareHeader* hdr = Header();

    if(hdr->magic != Firmware::MAGIC) return false;
    if(hdr->imageSize == 0) return false;
    if(hdr->imageSize > MemoryMap::APP_SIZE) return false;

    return true;
}

bool FirmwareManager::VerifyCRC(){
    const FirmwareHeader* hdr = Header();
    const uint8_t* Firmware = reinterpret_cast<const uint8_t*>(MemoryMap::APP_START + sizeof(FirmwareHeader));
    
    uint32_t crc = CRC32::Calculate(Firmware, hdr->imageSize);
    return crc == hdr->crc32;
}