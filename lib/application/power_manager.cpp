#include "power_manager.hpp"
#include "stm32f1xx_hal.h"
#include "SystemClockConfig.h"

PowerState PowerManager::CurrentState = PowerState::RUN;

void PowerManager::Init(){
    CurrentState = PowerState::RUN;
}

PowerState PowerManager::GetState(){
    return CurrentState;
}

void PowerManager::SetState(PowerState state){
    CurrentState = state;

    switch(state){
        case PowerState::RUN:
            RunMode();
            break;
        
        case PowerState::IDLE:
            IdleMode();
            break;

        case PowerState::LOW_POWER:
            LowPowerMode();
            break;

        case PowerState::STANDBY:
            StandbyMode();
            break;
        
        case PowerState::STOP:
            StopMode();
            break;
    }
}

void PowerManager::Update(float voltage, float current, float temperature){
    (void) temperature;

    if(voltage < 10.8f){
        SetState(PowerState::LOW_POWER);
    } else if(current < 0.05f){
        SetState(PowerState::IDLE);
    } else {
        SetState(PowerState::RUN);
    }
}

void PowerManager::RunMode(){
    HAL_ResumeTick();
}

void PowerManager::IdleMode(){
    __WFI();
}

void PowerManager::LowPowerMode(){
    HAL_SuspendTick();
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    HAL_ResumeTick();
}

void PowerManager::StopMode(){
    HAL_SuspendTick();
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
    SystemClock_Config();
    HAL_ResumeTick();
}

void PowerManager::StandbyMode(){
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    HAL_PWR_EnterSTANDBYMode();
}