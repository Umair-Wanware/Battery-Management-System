#include "app.hpp"

#include "stm32f1xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "adc/adc_drivers.h"
#include "i2c/i2c_drivers.h"
#include "uart/uart_drivers.h"
#include "spi/spi_drivers.h"
#include "ssd1306/ssd1306.h"
#include "ssd1306/ssd1306_fonts.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct SensorPacket {
    float voltage, temperature, current;
    uint16_t header, crc;
    uint8_t fault, soc;
};

volatile uint32_t idleCounter = 0;
extern "C" {
    void vApplicationIdleHook(){
        idleCounter++;
        __WFI();
    }

    void vApplicationMallocFailedHook(void){
        taskDISABLE_INTERRUPTS();
        while(1);
    }

    void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName){
        (void)xTask;
        (void)pcTaskName;
        taskDISABLE_INTERRUPTS();
        while(1);
    }
}

float ADC_TOTemp(uint16_t adc){
    static constexpr float VREF = 3.3f;
    static constexpr float ADC_MAX = 4095.0f;
    float voltage = (adc * VREF) / ADC_MAX;
    return voltage * 100.0f;
}

float ADC_TOVolt(uint16_t adc){
    static constexpr float VREF = 3.3f;;
    static constexpr float ADC_MAX = 4095.0f;
    static constexpr float divider = 11.0f;
    float pinVoltage = (adc * VREF) / ADC_MAX;
    return pinVoltage * divider;
}

float ADC_TOCurr(uint16_t adc){
    static constexpr float VREF = 3.3f;
    static constexpr float ADC_MAX = 4095.0f;
    static constexpr float OFFSET = 1.65f;
    static constexpr float SENSITIVITY = 0.185f;
    float voltage = (adc * VREF) / ADC_MAX;
    return (voltage - OFFSET) / SENSITIVITY;
}

uint8_t calculateSoc(float voltage){
    if(voltage >= 12.60f) return 100;
    if(voltage <= 10.80f) return 0;
    return static_cast<uint8_t>(((voltage - 10.8f) / (12.6 - 10.8f)) * 100.0f);
}

uint8_t checkFault(const SensorPacket& packet){
    if(packet.temperature > 60.0f) return 1;
    if(packet.voltage > 14.4f) return 2;
    if(packet.current > 5.0f) return 3;
    return 0;
}

uint16_t CRC16(const uint8_t *data, uint16_t length){
    uint16_t crc = 0xFFFF;
    while(length--){
        crc ^= *data++;
        for(uint8_t i = 0; i < 8; i++){
            if(crc & 0x0001){
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

class SensorTask {
    public:
    SensorTask(QueueHandle_t sQ, QueueHandle_t dQ) : sensorQueue(sQ), displayQueue(dQ) {}
    void Start(){
        xTaskCreate(task_entry, "SENSOR", 512, this, 5, nullptr);
    }

    private:
    QueueHandle_t sensorQueue, displayQueue;
    static void task_entry(void *pvPara){
        static_cast<SensorTask*>(pvPara)->Run();
    }
    
    void Run(){
        SensorPacket packet;

        while(true){
            uint16_t rawTemp = ADC_Read(ADC_CHANNEL_0);
            uint16_t rawVolt = ADC_Read(ADC_CHANNEL_1);
            uint16_t rawCurr = ADC_Read(ADC_CHANNEL_2);

            packet.header = 0x55AA;

            packet.temperature = ADC_TOTemp(rawTemp);
            packet.voltage = ADC_TOVolt(rawVolt);
            packet.current = ADC_TOCurr(rawCurr);

            packet.soc = calculateSoc(packet.voltage);
            packet.fault = checkFault(packet);

            packet.crc = CRC16(reinterpret_cast<uint8_t*>(&packet), sizeof(packet) - sizeof(packet.crc));
            
            xQueueOverwrite(sensorQueue, &packet);
            xQueueOverwrite(displayQueue, &packet);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
};

class CommunicationTask {
    public:
    CommunicationTask(QueueHandle_t sQ) : sensorQueue(sQ) {}
    void Start(){
        xTaskCreate(task_entry, "COMM", 512, this, 4, nullptr);
    }

    private:
    QueueHandle_t sensorQueue;
    static void task_entry(void *pvPara){
        static_cast<CommunicationTask*>(pvPara)->Run();
    }

    void Run(){
        SensorPacket packet;
        while(true){
            if(xQueueReceive(sensorQueue, &packet, portMAX_DELAY) == pdPASS){
                HAL_SPI_Transmit(&hspi, reinterpret_cast<uint8_t*>(&packet), sizeof(packet), 1000);
            }
        }
    }
};

class DisplayTask {
    public:
    DisplayTask(QueueHandle_t dQ) : displayQueue(dQ) {}
    void Start(){
        xTaskCreate(task_entry, "DISP", 2048, this, 3, nullptr);
    }

    private:
    QueueHandle_t displayQueue;
    static void task_entry(void *pvPara){
        static_cast<DisplayTask*>(pvPara)->Run();
    }

    void Run(){
        SensorPacket packet;
        char buffer[32];

        while(true){
            if(xQueuePeek(displayQueue, &packet, portMAX_DELAY) == pdPASS){
                ssd1306_Fill(Black);

                snprintf(buffer, sizeof(buffer), "Temp: %.1f C", packet.temperature);
                ssd1306_SetCursor(0, 0);
                ssd1306_WriteString(buffer, Font_7x10, White);

                snprintf(buffer, sizeof(buffer), "Volt: %.2f V", packet.voltage);
                ssd1306_SetCursor(0, 16);
                ssd1306_WriteString(buffer, Font_7x10, White);

                snprintf(buffer, sizeof(buffer), "Curr: %.2f A", packet.current);
                ssd1306_SetCursor(0, 32);
                ssd1306_WriteString(buffer, Font_7x10, White);

                snprintf(buffer, sizeof(buffer), "SOC: %d %%", packet.soc);
                ssd1306_SetCursor(0, 48);
                ssd1306_WriteString(buffer, Font_7x10, White);

                ssd1306_UpdateScreen();
            }
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
};

void App_Init(){
    UART_Init();
    SPI_Init();
    ADC_Init();
    I2C_Init();

    ssd1306_Init();

    static QueueHandle_t sensorQueue = xQueueCreate(1, sizeof(SensorPacket));
    static QueueHandle_t displayQueue = xQueueCreate(1, sizeof(SensorPacket));
    if(sensorQueue == nullptr || displayQueue == nullptr){
        while(1);
    }

    static SensorTask sensorTask(sensorQueue, displayQueue);
    static CommunicationTask communicationTask(sensorQueue);
    static DisplayTask displayTask(displayQueue);

    sensorTask.Start();
    communicationTask.Start();
    displayTask.Start();

    vTaskStartScheduler();

    while(true);
}