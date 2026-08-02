#include "app.hpp"

#include "stm32f1xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "adc/adc_drivers.h"
#include "i2c/i2c_drivers.h"
#include "uart/uart_drivers.h"
#include "spi/spi_drivers.h"
#include "ssd1306/ssd1306.h"
#include "ssd1306/ssd1306_fonts.h"
#include "algorithms.hpp"
#include "sensor_packet.hpp"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

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
        Algorithms algorithms;

        while(true){
            uint16_t rawTemp = ADC_Read(ADC_CHANNEL_0);
            uint16_t rawVolt = ADC_Read(ADC_CHANNEL_1);
            uint16_t rawCurr = ADC_Read(ADC_CHANNEL_2);

            packet.header = 0x55AA;
            packet.temperature = algorithms.ADC_ToTemp(rawTemp);
            packet.voltage = algorithms.ADC_ToVolt(rawVolt);
            packet.current = algorithms.ADC_ToCurr(rawCurr);
            packet.soc = algorithms.calculateSOC(packet.voltage);
            packet.fault = algorithms.checkFault(packet);
            packet.crc = algorithms.calculateCRC16(reinterpret_cast<uint8_t*>(&packet), sizeof(packet) - sizeof(packet.crc));

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
        while(true);
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