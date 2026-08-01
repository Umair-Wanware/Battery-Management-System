#pragma once

#include "protocol.hpp"

class SPITransport {
    public:
    static bool Initialize();
    static bool Receive(Packet& packet);
    static bool Send(const Packet& packet);
    static bool SendACK(uint16_t sequence);
    static bool SendNACK(uint16_t sequence);
};