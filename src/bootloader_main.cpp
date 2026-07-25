#include "stm32f1xx_hal.h"
#include "bootloader.hpp"

int main(void){
    HAL_Init();

    Bootloader bootloader;
    bootloader.Run();

    while(true);
}