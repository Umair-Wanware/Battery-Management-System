#pragma once

#include "stm32f1xx_hal.h"

class Bootloader {
    public:
    void Run();

    private:
    bool isApplicationValid() const;
    void JumpToApplication();
};