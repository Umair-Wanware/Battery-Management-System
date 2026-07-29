#pragma once
#include "firmware.hpp"

class FirmwareManager {
    public:
    static bool isValid();
    static bool VerifyCRC();
    static const FirmwareHeader* Header();
};