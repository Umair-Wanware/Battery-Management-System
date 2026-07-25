#include <stm32f1xx_hal.h>
#include "SystemClockConfig.h"
#include "app.hpp"
#include "bootloader.hpp"
 
int main(void){
    SCB->VTOR = 0x8004000UL;

    HAL_Init();
    SystemClock_Config();
    App_Init();

    while(1);
}