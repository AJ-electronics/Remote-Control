# ESP32 Universal Remote Controller

## Overview

This project is a custom-built universal remote controller for:

* RC drones
* RC aircraft
* robotics platforms
* ESP32-based wireless systems
* NRF24-based wireless systems

The remote is based on an ESP32-WROOM32 module and supports:

* Dual analog joysticks
* Two potentiometers
* Two toggle switches
* Two joystick push buttons
* NRF24L01 wireless communication
* ESP-NOW communication
* Wi-Fi communication
* Bluetooth communication
* OLED display interface
* Battery voltage monitoring with calibrated ADC mapping
* FTDI-based programming

---

# Features

* ESP32-WROOM32 based transmitter
* NRF24L01 wireless communication
* ESP-NOW support
* Wi-Fi support
* Bluetooth support
* 1.3 inch OLED display
* Dual-axis joystick control
* Additional analog potentiometer controls
* Toggle switch inputs
* Battery voltage monitoring with calibration
* FTDI serial programming support
* Dedicated boot switch for flashing firmware
* Expandable architecture for future RC protocols

---

# Hardware Used

| Component                 | Quantity |
| ------------------------- | -------- |
| ESP32-WROOM32             | 1        |
| NRF24L01 Module           | 1        |
| 1.3 inch OLED Display     | 1        |
| Analog Joystick Module    | 2        |
| Potentiometer             | 2        |
| Toggle Switch             | 2        |
| FTDI Programmer           | 1        |
| LiPo Battery (1S)         | 1        |
| Voltage Divider Resistors | 2        |
| Boot Switch               | 1        |
| Power Switch              | 1        |

---

# ESP32 Pin Connections

## Joystick Connections

### Joystick 1

| Signal      | ESP32 Pin |
| ----------- | --------- |
| X Axis      | GPIO35    |
| Y Axis      | GPIO34    |
| Push Button | GPIO17    |

### Joystick 2

| Signal      | ESP32 Pin |
| ----------- | --------- |
| X Axis      | GPIO33    |
| Y Axis      | GPIO32    |
| Push Button | GPIO16    |

---

# Potentiometer Connections

| Potentiometer | ESP32 Pin |
| ------------- | --------- |
| Pot 1         | GPIO39    |
| Pot 2         | GPIO36    |

---

# Toggle Switch Connections

| Switch   | ESP32 Pin |
| -------- | --------- |
| Toggle 1 | GPIO14    |
| Toggle 2 | GPIO13    |

---

# OLED Connections

| OLED Pin | ESP32 Pin |
| -------- | --------- |
| SDA      | GPIO21    |
| SCL      | GPIO22    |
| VCC      | 3.3V      |
| GND      | GND       |

## OLED Specifications

* SH1106 based
* 128x64 resolution
* I2C interface
* 1.3 inch display

---

# NRF24L01 Connections

| NRF24 Pin | ESP32 Pin |
| --------- | --------- |
| CE        | GPIO25    |
| CSN       | GPIO27    |
| SCK       | GPIO18    |
| MOSI      | GPIO23    |
| MISO      | GPIO19    |
| VCC       | 3.3V      |
| GND       | GND       |

## NRF24 Notes

* NRF24L01 must be powered from 3.3V only
* Add a 10uF to 47uF capacitor across VCC and GND near the NRF module
* Long wires may cause instability
* PA/LNA modules require stable power supply

---

# Battery Monitoring

The remote uses a 1S LiPo battery.

Battery voltage is monitored using a voltage divider connected to GPIO26.

## Voltage Divider

| Connection      | Value |
| --------------- | ----- |
| Top Resistor    | 10k    |
| Bottom Resistor | 10k    |

Divider midpoint is connected to:

* GPIO26

---

# Battery Calibration Table

The ESP32 ADC was calibrated manually using measured battery voltages.

| Battery Voltage | ADC Reading |
| --------------- | ----------- |
| 3.60V           | 1637        |
| 3.65V           | 1664        |
| 3.70V           | 1690        |
| 3.75V           | 1716        |
| 3.80V           | 1744        |
| 3.85V           | 1773        |
| 3.90V           | 1800        |
| 3.95V           | 1824        |
| 4.00V           | 1852        |
| 4.05V           | 1878        |
| 4.10V           | 1904        |
| 4.15V           | 1934        |
| 4.20V           | 1960        |
| 4.25V           | 1987        |
| 4.30V           | 2015        |

Firmware uses interpolation between calibration points for accurate battery voltage estimation.

---

# FTDI Programming Connections

| FTDI Pin | ESP32 Pin |
| -------- | --------- |
| TX       | RX0       |
| RX       | TX0       |
| GND      | GND       |
| VCC      | 5V / VIN  |

---

# Boot Mode Instructions

The ESP32 requires boot mode to be enabled while uploading firmware.

## Upload Procedure

1. Connect FTDI programmer
2. Turn ON the boot switch
3. Click Upload in Arduino IDE
4. Wait for flashing to begin
5. Turn OFF the boot switch
6. ESP32 boots normally

---

# Arduino IDE Settings

| Setting          | Value            |
| ---------------- | ---------------- |
| Board            | ESP32 Dev Module |
| Upload Speed     | 115200           |
| Flash Frequency  | 80MHz            |
| Partition Scheme | Default          |

---

# Required Libraries

Install the following libraries from Arduino Library Manager:

* RF24
* U8g2
* Wire
* SPI

---

# OLED Information

The OLED display shows:

* Battery voltage
* Joystick positions
* Potentiometer values
* Toggle switch states
* Button states
* NRF24 connection status

---

# Communication Protocols

This remote is designed to support multiple wireless communication methods:

## NRF24L01

* Low latency
* Long range
* RC applications

## ESP-NOW

* Direct ESP32-to-ESP32 communication
* Low latency
* No router required

## Wi-Fi

* Web interfaces
* Remote configuration
* Telemetry systems

## Bluetooth

* Mobile app control
* Device pairing
* Wireless configuration

---

# Future Improvements

* Receiver telemetry
* Long-range NRF24 PA/LNA support
* RC protocol support
* Menu-based OLED UI
* USB-C charging
* Hall-effect gimbals
* Telemetry logging
* Model memory support
* OTA firmware updates

---

# License

MIT License

Copyright (c) 2026 Anand

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files to deal in the Software
without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies.
