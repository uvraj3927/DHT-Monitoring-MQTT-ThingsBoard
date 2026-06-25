# 🌤️ DHT-Monitoring-MQTT-ThingsBoard: NodeMCU Weather Station

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Build-orange?logo=platformio&style=flat-square)](https://platformio.org/)
[![Framework](https://img.shields.io/badge/Framework-Arduino-blue?logo=arduino&style=flat-square)](https://www.arduino.cc/)
[![Cloud](https://img.shields.io/badge/Broker-ThingsBoard-blue?logo=thingsboard&style=flat-square)](https://thingsboard.io/)
[![License](https://img.shields.io/badge/License-MIT-green.svg?style=flat-square)](LICENSE)

DHT-Monitoring-MQTT-ThingsBoard  is a secure, lightweight, and efficient Internet of Things (IoT) weather station built using a **NodeMCU ESP8266** microcontroller and a **DHT11** temperature and humidity sensor. Telemetry data is parsed locally and transmitted securely over the **MQTT protocol** to a customized dashboard on the **ThingsBoard Cloud**.

---

##  Real Circuit 

Below is the physical assembly of the hardware components:

![Real Circuit Setup](images/realckt.jpg)

---

## Overview & Features

Managing microclimates requires continuous data acquisition. This project provides a low-cost, self-healing, and secure solution for transmitting environmental metrics.

### Key Features:
*   **Non-Blocking Telemetry:** Utilizes a non-blocking `millis()` timer instead of CPU-blocking `delay()` calls, ensuring continuous execution of the MQTT client loops.
*   **Decoupled & Secure Config:** Sensitive settings (Wi-Fi credentials, broker tokens) are entirely isolated into a local configuration header (`config.h`), kept safe from public git history via `.gitignore`.
*   **Robust Connection Recovery:** Automatic self-healing reconnection routines for both Wi-Fi networks and the MQTT broker if communication drops.
*   **Telemetry Verification:** Validates data readings (checking for `nan` errors) before payload serialization to avoid sending corrupted points.

---

## System Architecture

The data pipeline runs sequentially from the physical sensor up to the cloud dashboard:

```mermaid
graph TD
    A[DHT11 Sensor] -->|Digital Output| B(NodeMCU ESP8266)
    B -->|WiFi / JSON Telemetry| C{MQTT Broker}
    C -->|v1/devices/me/telemetry| D[ThingsBoard Cloud Dashboard]
```

1.  **DHT11 Sensor:** Captures environmental temperature and relative humidity.
2.  **NodeMCU (ESP8266):** Processes the raw digital pulse stream, performs data integrity checks, and serializes metrics into a JSON payload.
3.  **MQTT Client:** Establishes connection to the ThingsBoard broker on the default port `1883`, publishing client-authenticated payloads to the telemetry topic.
4.  **ThingsBoard Cloud:** De-serializes the payload and updates the user-facing dashboard widgets in real-time.

![System Architecture Flow](images/architecture_flow.png)

---


## 🔌 Hardware Setup & Pinout

Connect the DHT11 sensor to the NodeMCU development board according to the pin mapping below.

| DHT11 Pin | NodeMCU Pin | GPIO Pin | Cable Color | Purpose |
| :--- | :--- | :--- | :--- | :--- |
| **VCC** | **3V3** | - | Red | 3.3V Power Line |
| **GND** | **GND** | - | Black | System Ground Reference |
| **DATA**| **D2** | **GPIO4** | Yellow | Single-Bus Digital Signal |

![Hardware Wiring Diagram](images/wiring_diagram.png)

---

## ⚙️ Software Setup & Build Instructions

This project is configured for **PlatformIO IDE** (Visual Studio Code plugin) or the **PlatformIO Core CLI**.

### Required libraries:
These are specified inside your `platformio.ini` project file:
*   `adafruit/DHT sensor library`
*   `adafruit/Adafruit Unified Sensor`
*   `knolleary/PubSubClient`

### Compilation & Upload:
Open your terminal in the project directory and run the following command to compile and upload the firmware to your NodeMCU:
```bash
# Compile project
pio run

# Upload code to the connected board
pio run --target upload

# Open Serial Monitor for logs (speed: 115200)
pio device monitor
```


