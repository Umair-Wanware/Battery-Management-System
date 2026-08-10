#pragma once
#include <stdint.h>

enum class PowerState : uint8_t {
    RUN,
    IDLE,
    LOW_POWER,
    STOP,
    STANDBY
};

class PowerManager {
    public:
    static void Init();
    static void SetState(PowerState state);
    static PowerState GetState();
    static void Update(float voltage, float current, float temperature);
    static const char* toString();

    private:
    static PowerState CurrentState;
    static void RunMode();
    static void IdleMode();
    static void LowPowerMode();
    static void StandbyMode();
    static void StopMode();
};