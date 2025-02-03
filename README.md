# NodeMCU HomeKit-MQTT Environmental Sensor

A DIY IoT sensor that exposes temperature/humidity data to Apple HomeKit and MQTT brokers using an ESP8266 (NodeMCU) and BME280 sensor.

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

- Arduino IDE 2.x
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
git clone https://github.com/guenthereder/NodeMcuHomekitMqtt.git
cd NodeMcuHomekitMqtt

### 2. Install Dependencies

**Using Arduino IDE:**

1. Open `NodeMcuHomekitMqtt.ino`
2. Install libraries via **Tools > Manage Libraries**
3. Install all libraries listed in [Library List](#library-list)

**Using PlatformIO:**

Not yet working

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


## Flashing Instructions

### Using Arduino IDE

1. Select board: **NodeMCU 1.0 (ESP-12E Module)**
2. Set Flash Mode: **DIO**
3. Flash Frequency: **40MHz**
4. Upload Speed: **115200**
5. Port: **Select correct COM port**
6. Click **Upload**

### Using Prebuilt Binary

1. Download latest `.bin` from [Releases](https://github.com/guenthereder/NodeMcuHomekitMqtt.git/releases)
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

### 2\. Install Dependencies

**Using Arduino IDE:**

1.Open NodeMcuHomekitMqtt.ino
2.Install libraries via **Tools > Manage Libraries**
3.Install all libraries listed in requirements

### 3\. Initial Configuration

1.CopyBME280 ↔ NodeMCUVCC → 3.3VGND → GNDSCL → D1 (GPIO5)SDA → D2 (GPIO4)
2.Verify I2C address (0x76 or 0x77) using I2C scanner

First Time Setup
----------------

1.Power the device
2.Connect to WiFi AP: NodeMCU-Sensor
3.Open [http://192.168.4.1](http://192.168.4.1/) in browser
4.Configure parameters:
    *WiFi SSID/password
    *MQTT broker IP & port
    *MQTT topics
    *HomeKit setup code (format: XXX-XX-XXX)

Flashing Instructions
---------------------

### Using Arduino IDE

1.Select board: **NodeMCU 1.0 (ESP-12E Module)**
2.Set Flash Mode: **DIO**
3.Flash Frequency: **40MHz**
4.Upload Speed: **115200**
5.Port: **Select correct COM port**
6.Click **Upload**

### Browser-Based Flashing (Chrome/Edge)

1.Visit [ESP Web Tools](https://espressif.github.io/esp-web-tools/)
2.Connect NodeMCU via USB
3.Drag & drop firmware.bin from [Releases](https://github.com/guenthereder/NodeMcuHomekitMqtt/releases)
4.Follow on-screen instructions to flash

### Using Prebuilt Binary

1.Download latest .bin from [Releases](https://github.com/guenthereder/NodeMcuHomekitMqtt/releases)
2.Flash using esptool:

bashCopy

Plain textANTLR4BashCC#CSSCoffeeScriptCMakeDartDjangoDockerEJSErlangGitGoGraphQLGroovyHTMLJavaJavaScriptJSONJSXKotlinLaTeXLessLuaMakefileMarkdownMATLABMarkupObjective-CPerlPHPPowerShell.propertiesProtocol BuffersPythonRRubySass (Sass)Sass (Scss)SchemeSQLShellSwiftSVGTSXTypeScriptWebAssemblyYAMLXML`   esptool.py --port COM3 write_flash 0x0 firmware.bin   `

Usage
-----

***HomeKit Pairing:** Use configured setup code in Apple Home app
***MQTT Topics:**
 *Temperature: sensors/utilityroom/temperature
 *Humidity: sensors/utilityroom/humidity
***Update Interval:** 10 seconds (configurable in code)

Creating Release Binaries
-------------------------

### Generate Binary File

**Using Arduino IDE:**

1.Open project
2.Select **Sketch > Export Compiled Binary**
3.Binary will be created in project folder: NodeMcuHomekitMqtt.ino.nodemcu.bin

Troubleshooting
---------------

**IssueSolution**WiFi connection failedCheck ESP8266 antenna positionBME280 not foundVerify I2C address (0x76/0x77)HomeKit pairing failureReset HomeKit configurationBrowser flashing not workingEnable WebSerial in browser flags

License
-------

MIT License - See [LICENSE](https://chat.deepseek.com/LICENSE) for details

Acknowledgments
---------------

*Apple HomeKit ESP8266 implementation by Mixiaoxiao
*WiFiManager by tzapu
*Adafruit for BME280 library
*ESP Web Tools by Espressif