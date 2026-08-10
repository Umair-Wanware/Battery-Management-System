#include "boot_screen.hpp"
#include "stm32f1xx_hal.h"
#include "ssd1306/ssd1306.h"
#include "ssd1306/ssd1306_fonts.h"
#include <stdio.h>

char buffer[48];
size_t size = sizeof(buffer);

void BootScreen::Version(){
    ssd1306_Fill(Black);

    ssd1306_SetCursor(5, 0);
    snprintf(buffer, size, "Battery Management System");
    ssd1306_WriteString(buffer, Font_7x10, White);

    ssd1306_SetCursor(20, 16);
    sniprintf(buffer, size, "Control Unit");
    ssd1306_WriteString(buffer, Font_11x18, White);

    ssd1306_SetCursor(18, 48);
    sniprintf(buffer, size, "Version 2.0.0");
    ssd1306_WriteString(buffer, Font_7x10, White);

    ssd1306_UpdateScreen();
    HAL_Delay(2000);
}

void BootScreen::Init(){
    ssd1306_Fill(Black);

    ssd1306_SetCursor(0, 0);
    snprintf(buffer, size, "Initializing....");
    ssd1306_WriteString(buffer, Font_7x10, White);
    HAL_Delay(3000);

    ssd1306_SetCursor(0, 16);
    snprintf(buffer, size, "GPIO [OK]");
    ssd1306_WriteString(buffer, Font_7x10, White);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);
    
    ssd1306_SetCursor(0, 26);
    snprintf(buffer, size, "I2C [OK]");
    ssd1306_WriteString(buffer, Font_7x10, White);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);

    ssd1306_SetCursor(0, 36);
    snprintf(buffer, size, "ADC [OK]");
    ssd1306_WriteString(buffer, Font_7x10, White);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);

    ssd1306_SetCursor(0, 46);
    snprintf(buffer, size, "SPI [OK]");
    ssd1306_WriteString(buffer, Font_7x10, White);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);
}

void BootScreen::Ready(){
    ssd1306_Fill(Black);

    ssd1306_SetCursor(25, 20);
    snprintf(buffer, size, "SYSTEM");
    ssd1306_WriteString(buffer, Font_11x18, White);

    ssd1306_SetCursor(30, 42);
    snprintf(buffer, size, "READY");
    ssd1306_WriteString(buffer, Font_11x18, White);
    ssd1306_UpdateScreen();

    HAL_Delay(1000);
}