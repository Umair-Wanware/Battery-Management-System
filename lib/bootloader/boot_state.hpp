#pragma once

#include <stdint.h>

enum class BootState : uint8_t {
    INIT = 0,

    CHECK_APPLICATION,
    WAIT_FOR_UPDATE,
    RECEIVE_HEADER,
    ERASE_FLASH,
    RECEIVE_IMAGE,
    VERIFY_IMAGE,
    JUMP_APPLICATION,
    ERROR
};