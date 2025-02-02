# NodeMCU HomeKit-MQTT Environmental Sensor

A DIY IoT sensor that exposes temperature/humidity data to Apple HomeKit and MQTT brokers using an ESP8266 (NodeMCU) and BME280 sensor.

![Project Diagram](https://raw.githubusercontent.com/yourusername/NodeMCU-HomeKit-MQTT/main/images/diagram.png)

## Features
- 📶 WiFi configuration via captive portal
- 🏡 Apple HomeKit integration
- 📡 MQTT publishing to any broker
- 🔄 Over-the-Air (OTA) update support
- ⚡ Automatic recovery modes
- 📈 Configurable update intervals

## Hardware Requirements
- NodeMCU ESP8266 development board
- BME280 temperature/humidity sensor
- Jumper wires (female-to-female)
- Micro USB cable
- 3.3V/5V power source

## Software Requirements
- Arduino IDE 2.x or PlatformIO
- ESP8266 Arduino Core
- Required Libraries:
  - Arduino-HomeKit-ESP8266
  - WiFiManager
  - PubSubClient
  - Adafruit BME280 Library
  - ArduinoJson

## Installation & Setup

### 1. Clone Repository
```bash
git clone https://github.com/yourusername/NodeMCU-HomeKit-MQTT.git
cd NodeMCU-HomeKit-MQTT
```

### 2. Install Dependencies
**Using Arduino IDE:**
1. Open `NodeMcuHomekitMqtt.ino`
2. Install libraries via **Tools > Manage Libraries**
3. Install all libraries listed in [Library List](#library-list)

**Using PlatformIO:**
```ini
[env:nodemcuv2]
platform = espressif8266
board = nodemcuv2
framework = arduino
lib_deps = 
    knolleary/PubSubClient
    tzapu/WiFiManager
    bblanchon/ArduinoJson
    adafruit/Adafruit BME280 Library
    adafruit/Adafruit Unified Sensor
    Mixiaoxiao/Arduino-HomeKit-ESP8266
```

### 3. Initial Configuration
1. Connect BME280 to NodeMCU:
   ```
   BME280 ↔ NodeMCU
   VCC   → 3.3V
   GND   → GND
   SCL   → D1 (GPIO5)
   SDA   → D2 (GPIO4)
   ```
2. Verify I2C address (0x76 or 0x77) using I2C scanner

## First Time Setup
1. Power the device
2. Connect to WiFi AP: `NodeMCU-Sensor`
3. Open http://192.168.4.1 in browser
4. Configure parameters:
   - WiFi SSID/password
   - MQTT broker IP & port
   - MQTT topics
   - HomeKit setup code (format: XXX-XX-XXX)

![Captive Portal](https://raw.githubusercontent.com/yourusername/NodeMCU-HomeKit-MQTT/main/images/portal.png)

## Flashing Instructions

### Using Arduino IDE
1. Select board: **NodeMCU 1.0 (ESP-12E Module)**
2. Set Flash Mode: **DIO**
3. Flash Frequency: **40MHz**
4. Upload Speed: **115200**
5. Port: **Select correct COM port**
6. Click **Upload**

### Using Prebuilt Binary
1. Download latest `.bin` from [Releases](https://github.com/yourusername/NodeMCU-HomeKit-MQTT/releases)
2. Flash using esptool:
```bash
esptool.py --port COM3 write_flash 0x0 firmware.bin
```

## Usage
- **HomeKit Pairing:** Use configured setup code in Apple Home app
- **MQTT Topics:**
  - Temperature: `sensors/utilityroom/temperature`
  - Humidity: `sensors/utilityroom/humidity`
- **Update Interval:** 10 seconds (configurable in code)

## Creating Release Binaries

### 1. Generate Binary File
**Using Arduino IDE:**
1. Open project
2. Select **Sketch > Export Compiled Binary**
3. Binary will be created in project folder: `NodeMcuHomekitMqtt.ino.nodemcu.bin`

**Using PlatformIO:**
```bash
pio run -e nodemcuv2 -t buildfs -t upload
```

### 2. Create GitHub Release
1. Tag your release: `v1.0.0`
2. Upload binary file
3. Add release notes
4. Publish!

Recommended file naming convention:
`NodeMCU-HomeKit-MQTT-vX.Y.Z.bin`

## Troubleshooting

| Issue                          | Solution                      |
|--------------------------------|-------------------------------|
| WiFi connection failed         | Check ESP8266 antenna position |
| BME280 not found               | Verify I2C address (0x76/0x77)|
| HomeKit pairing failure        | Reset HomeKit configuration   |
| MQTT connection issues         | Check broker IP/credentials   |
| Configuration reset            | Hold FLASH button during boot |

## License
MIT License - See [LICENSE](LICENSE) for details

## Acknowledgments
- Apple HomeKit ESP8266 implementation by Mixiaoxiao
- WiFiManager by tzapu
- Adafruit for BME280 library