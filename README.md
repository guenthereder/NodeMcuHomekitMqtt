[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://www.buymeacoffee.com/guenthereder)

# NodeMCU HomeKit-MQTT Environmental Sensor

A DIY IoT sensor that exposes temperature/humidity data to Apple HomeKit and MQTT brokers using an ESP8266 (NodeMCU) and BME280 sensor.

There are many good solutions out there, due to some unknown reason I could not get them to work as I'd liked. This is why here is another solution.

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

### Web Installer

The easiest solution is to install the firmware via a web installer. You only need a chrome based browser.
[Web Installer](https://geder.at/node_mcu)

### DIY

#### 1. Clone Repository

```bash
git clone https://github.com/guenthereder/NodeMcuHomekitMqtt.git
cd NodeMcuHomekitMqtt
```

#### 2. Install Dependencies

**Using Arduino IDE:**

1. Open `NodeMcuHomekitMqtt.ino`
2. Install libraries via **Tools > Manage Libraries**
3. Install all libraries listed in [Library List](#library-list)

**Using PlatformIO:**

(Not yet working)

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

## First Time Setup

1. Power the device
2. Connect to WiFi AP: `NodeMCU-Sensor`
3. Wait: the web guard should open automatically
4. Configure parameters:
   - WiFi SSID/password
   - MQTT broker IP & port
   - MQTT topics
   - HomeKit setup code (format: XXX-XX-XXX)

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

(not yet working)

```bash
pio run -e nodemcuv2 -t buildfs -t upload
```

### 2\. Install Dependencies

**Using Arduino IDE:**

1.Open NodeMcuHomekitMqtt.ino
2.Install libraries via **Tools > Manage Libraries**
3.Install all libraries listed in requirements

## Server-Side Setup for MQTT, Telegraf, InfluxDB, and Grafana

I like having a temperature and humidity sensor in Homekit, but the visualization and evaluation part is somewhat limited.
So I wanted to have a nice time series plot of the sensor data. That is why I use the node as an MQTT broker.
The next step is a telegraf server that writes the received data into an influxdb which in turn is read by grafana.
Grafana has very nice plotting and visualization capabilities.

The following setup is only needed if you also want that MQTT broker capability working.

1. Mosquitto MQTT Broker
2. Telegraf (to collect MQTT data and write to InfluxDB)
3. InfluxDB 1.x (time-series database)
4. Grafana (for data visualization)

### Prerequisites

A Raspberry Pi or Linux-based system.
Basic familiarity with the command line.
NodeMCU or similar device publishing data to MQTT.

### 1. Install Mosquitto MQTT Broker

Step 1: Install Mosquitto

```bash
sudo apt update
sudo apt install mosquitto mosquitto-clients
```

Step 2: Enable and Start Mosquitto

```bash
sudo systemctl enable mosquitto
sudo systemctl start mosquitto
```

Step 3: Test Mosquitto

Subscribe to a topic in one terminal:

```bash
mosquitto_sub -h localhost -t "test/topic"
```

Publish a message in another terminal:

```bash
mosquitto_pub -h localhost -t "test/topic" -m "Hello, MQTT!"
```

Verify the message appears in the subscriber terminal.

### 2. Install InfluxDB 1.x

I use version 1.x as the 2.x was not available on arm (raspberry pi) at the creation of this project.

Step 1: Add the InfluxDB Repository

```bash
wget -qO- https://repos.influxdata.com/influxdb.key | sudo apt-key add -
echo "deb https://repos.influxdata.com/debian $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/influxdb.list
```

Step 2: Install InfluxDB

```bash
sudo apt update
sudo apt install influxdb
```

Step 3: Start and Enable InfluxDB

```bash
sudo systemctl start influxdb
sudo systemctl enable influxdb
```

Step 4: Verify InfluxDB

Open the InfluxDB shell:

```bash
influx
```

Create a database for sensor data:

```sql
CREATE DATABASE sensor_data
```

Verify the database:

```sql
SHOW DATABASES
```

## 3. Install Telegraf

Step 1: Install Telegraf

```bash
sudo apt install telegraf
```

Step 2: Configure Telegraf

Edit the Telegraf configuration file:

```bash
sudo nano /etc/telegraf/telegraf.conf
```

Add the following sections:

```ini
[[inputs.mqtt_consumer]]
  servers = ["tcp://localhost:1883"]
  topics = ["sensors/#"]
  data_format = "json"  # Use "value" for plain text messages

[[outputs.influxdb]]
  urls = ["http://localhost:8086"]
  database = "sensor_data"
  skip_database_creation = false
```

Step 3: Start and Enable Telegraf

```bash
sudo systemctl start telegraf
sudo systemctl enable telegraf
```

Step 4: Verify Telegraf Logs

Check the logs for errors:

```bash
sudo journalctl -u telegraf
```

## 4. Install Grafana

Step 1: Install Grafana

```bash
sudo apt install grafana
```

Step 2: Start and Enable Grafana

```bash
sudo systemctl start grafana-server
sudo systemctl enable grafana-server
```

Step 3: Access Grafana

Open a browser and go to:

```bash
http://<PI_IP_ADDRESS>:3000
Log in with the default credentials:
Username: admin
Password: admin
```

Step 4: Add InfluxDB as a Data Source

Go to Configuration → Data Sources → Add Data Source.
Select InfluxDB.
Configure the data source:
URL: http://localhost:8086
Database: sensor_data
Click Save & Test.

5. Verify the Setup

Step 1: Publish Test Data to MQTT

Publish a test JSON message to the MQTT broker:

```bash
mosquitto_pub -h localhost -t "sensors/livingroom/temperature" -m '{"temperature": 23.5}'
```

Step 2: Check InfluxDB

Open the InfluxDB shell:

```bash
influx
```

Query the data:

```sql
USE sensor_data
SELECT * FROM "sensors/livingroom/temperature"
```

## Step 3: Create a Grafana Dashboard

Go to Dashboards → New Dashboard → Add Panel.
In the query editor:
Select the sensor_data database.
Choose the measurement (e.g., sensors/livingroom/temperature).
Select the field (e.g., temperature).
Save the panel and dashboard.

## Troubleshooting

## Mosquitto

### Check if Mosquitto is running

```bash
sudo systemctl status mosquitto
```

### Check logs

```bash
sudo journalctl -u mosquitto
```

## Telegraf

### Check logs

```bash
sudo journalctl -u telegraf
```

Verify MQTT message format:

```bash
mosquitto_sub -h localhost -t "sensors/#"
```

## InfluxDB

Check if InfluxDB is running:

```bash
sudo systemctl status influxdb
```

Check logs:

```bash
sudo journalctl -u influxdb
```

Grafana

Check if Grafana is running:

```bash
sudo systemctl status grafana-server
```

Check logs:

```bash
sudo journalctl -u grafana-server
```

## License

MIT License - See [LICENSE](https://chat.deepseek.com/LICENSE) for details

## Acknowledgments

* Apple HomeKit ESP8266 implementation by Mixiaoxiao
* WiFiManager by tzapu
* Adafruit for BME280 library
* ESP Web Tools by Espressif