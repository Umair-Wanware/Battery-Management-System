#include "firmware_manager.hpp"

const FirmwareHeader* FirmwareManager::Header(){
    return reinterpret_cast<const FirmwareHeader*>(Firmware::HEADER_ADDRESS);
}

bool FirmwareManager::isValid(){
    const FirmwareHeader* hdr = Header();

    if(hdr->magic != Firmware::MAGIC) return false;
    if(hdr->imageSize == 0) return false;
    if(hdr->imageSize > (112 * 1024)) return false;

    return true;
}

bool FirmwareManager::VerifyCRC(){
    return true;
}