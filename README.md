# Battery Management System (BMS)

A modular Battery Management System (BMS) firmware developed for the **STM32F103RB** using **Modern C++**, **STM32 HAL**, and **FreeRTOS**.

The project demonstrates production-style embedded firmware architecture including a custom bootloader, RTOS-based task scheduling, battery monitoring algorithms, SPI communication, OLED display, firmware update support, and modular driver development.

---

# Features

- Modern C++ firmware architecture
- FreeRTOS task scheduling
- Custom STM32 Bootloader
- Custom Linker Scripts
- Firmware Update Framework
- Flash Memory Driver
- Firmware Image Verification
- CRC16 Packet Validation
- CRC32 Firmware Verification
- SPI Communication
- UART Communication
- I2C Driver
- ADC Driver
- SSD1306 OLED Display
- Sensor Data Processing
- Battery Voltage Monitoring
- Battery Current Monitoring
- Temperature Monitoring
- State of Charge (SOC) Calculation
- Fault Detection
- Packet Based Communication Protocol
- Modular Driver Layer
- Hardware Abstraction Layer

---

# Hardware

- STM32F103RB
- SSD1306 OLED Display
- Voltage Divider
- ACS712 Current Sensor
- LM35 Temperature Sensor
- ESP32 (Firmware Updater)
- SPI Interface
- UART Interface

---

# Software Stack

- C++17
- STM32 HAL
- FreeRTOS
- PlatformIO
- VS Code

---

# Project Structure

```
Battery Management System
│
├── src/
│   └── main.cpp
│
├── lib/
│   ├── application/
│   ├── algorithms/
│   ├── bootloader/
│   ├── drivers/
│   │   ├── adc/
│   │   ├── spi/
│   │   ├── uart/
│   │   ├── i2c/
│   │   └── ssd1306/
│   └── protocol/
│
├── linker/
│
├── include/
│
├── platformio.ini
│
└── README.md
```

---

# Firmware Architecture

```
                   Application Layer
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
  Sensor Task      Communication Task    Display Task
        │                  │                  │
        └──────────────┬───┴──────────────────┘
                       │
                  Queue System
                       │
                Algorithms Module
                       │
         ┌─────────────┼─────────────┐
         │             │             │
      ADC Driver   SPI Driver   UART Driver
         │
      STM32 HAL
```

---

# Bootloader Architecture

```
Power On
    │
    ▼
Initialize Hardware
    │
    ▼
Check Application
    │
    ├──────────────┐
    │              │
 Invalid        Valid
    │              │
Receive FW     Wait For Update
    │              │
Erase Flash     Timeout
    │              │
Receive Image    │
    │             │
Verify CRC ◄─────┘
    │
    ▼
Jump To Application
```

---

# Bootloader Features

- Custom Memory Map
- Custom Linker Script
- Flash Programming
- Firmware Header
- Firmware Validation
- CRC32 Verification
- SPI Firmware Update
- ACK/NACK Protocol
- Packet Sequence Checking
- Safe Jump to Application

---

# FreeRTOS Tasks

| Task | Description |
|------|-------------|
| Sensor Task | Reads ADC values and creates sensor packets |
| Communication Task | Sends packets over SPI |
| Display Task | Updates SSD1306 OLED |
| Idle Hook | Low-power mode using WFI |

---

# Sensor Packet

```cpp
struct SensorPacket
{
    float voltage;
    float temperature;
    float current;

    uint16_t header;
    uint16_t crc;

    uint8_t fault;
    uint8_t soc;
};
```

---

# Battery Algorithms

Implemented algorithms include:

- ADC to Voltage Conversion
- ADC to Current Conversion
- ADC to Temperature Conversion
- State of Charge (SOC)
- Fault Detection
- CRC16 Generation

---

# Drivers

- ADC
- SPI
- UART
- I2C
- SSD1306 OLED
- Flash Driver

---

# Communication Protocol

```
+---------+---------+---------+---------+
| Header  | Payload | CRC16   | Footer  |
+---------+---------+---------+---------+
```

---

# Memory Layout

```
Flash
────────────────────────────────────

0x08000000
│
│ Bootloader
│
0x08004000
│
│ Application
│
0x08020000

────────────────────────────────────
```

---

# Build

```bash
pio run -e bootloader
```

Build Application

```bash
pio run -e application
```

Flash Bootloader

```bash
pio run -e bootloader -t upload
```

Flash Application

```bash
pio run -e application -t upload
```

---

# Future Improvements

- Software Timers
- Battery State of Health (SOH)
- ESP32 OTA Updater

---

# Learning Objectives

This project demonstrates practical knowledge of

- Embedded C++
- FreeRTOS
- STM32 HAL
- Firmware Architecture
- Bootloader Design
- Flash Programming
- Embedded Communication Protocols
- Embedded Software Design Patterns
- Memory Management
- Interrupt Handling
- Embedded Driver Development

---

# Author

**Umair Wanware**

Embedded Systems | Software Developer

GitHub: https://github.com/Umair-Wanware
