#include "boot_state.hpp"

const char* BootStateToString(BootState state){
    switch(state){
        case BootState::INIT: return "INIT";
        case BootState::CHECK_APPLICATION: return "CHECK_APPLICATION";
        case BootState::WAIT_FOR_UPDATE: return "WAIT_FOR_UPDATE";
        case BootState::RECEIVE_HEADER: return "RECEIVE_HEADER";
        case BootState::ERASE_FLASH: return "ERASE_FLASH";
        case BootState::RECEIVE_IMAGE: return "RECEIVE_IMAGE";
        case BootState::VERIFY_IMAGE: return "VERIFY_IMAGE";
        case BootState::JUMP_APPLICATION: return "JUMP_TO_APPLICATION";
        case BootState::ERROR: return "ERROR";
        default: return "UNKOWN";
    }
}