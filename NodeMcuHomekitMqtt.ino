#include <PubSubClient.h>

#include <Arduino.h>
#include <arduino_homekit_server.h>

#include "wifi_info.h"
#include "my_accessory.h"

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);

Adafruit_BME280 bme; // I2C

unsigned long delayTime;
float last_temperature = -1000;  // Store last values to prevent unnecessary updates
float last_humidity = -1000;

// MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);

const char* mqttServer = "<MQTT SERVER IP>";
const int mqttPort = 1883;
const char* mttopicTemp = "sensors/utilityroom/temperature";
const char* mttopicHumid = "sensors/utilityroom/humidity";


void setup() {
    Serial.begin(115200);
    wifi_connect(); // Ensure WiFi is stable

    // Uncomment this to reset HomeKit pairing (upload once, then remove)
    // homekit_storage_reset();

    my_homekit_setup();

    // Initialize BME280 sensor
    if (!bme.begin(0x76)) {
        Serial.println("Could not find a valid BME280 sensor!");
    }

    // Initialize MQTT
    mqttClient.setServer(mqttServer, mqttPort);
}

void loop() {
    // Check WiFi and reconnect if needed
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi lost, reconnecting...");
        WiFi.reconnect();
        delay(5000); // Allow time to reconnect
    }

    my_homekit_loop();
    mqttClient.loop(); // Process MQTT messages
    delay(100); // Reduce CPU load, avoid unnecessary rapid looping
}


//==============================
// Homekit setup and loop
//==============================

// Access HomeKit characteristics
extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t cha_current_temperature;
extern "C" homekit_characteristic_t cha_humidity;

static uint32_t next_heap_millis = 0;
static uint32_t next_report_millis = 0;

void my_homekit_setup() {
    arduino_homekit_setup(&config);
}

void my_homekit_loop() {
    arduino_homekit_loop();
    const uint32_t t = millis();

    if (t > next_report_millis) {
        // Report sensor values every 10 seconds
        next_report_millis = t + 10 * 1000;
        my_homekit_report();
    }

    if (t > next_heap_millis) {
        // Show heap info every 5 seconds
        next_heap_millis = t + 5 * 1000;
        LOG_D("Free heap: %d, HomeKit clients: %d",
                ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
    }
}

void mqttReconnect() {
  Serial.println("Attempting MQTT connection...");
  if (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("NodeMCU_HomeKit")) {
      Serial.println("MQTT connected");
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.print(mqttClient.state());
      // Serial.println(" retrying in 5 seconds...");
      // delay(5000);
    }
  }
}

void my_homekit_report() {
    float temperature_value = bme.readTemperature();
    float humidity_value = bme.readHumidity();

    // Only notify HomeKit if values have changed significantly (to save memory & prevent spam)
    if (abs(temperature_value - last_temperature) > 0.1) {
        last_temperature = temperature_value;
        cha_current_temperature.value.float_value = temperature_value;
        LOG_D("Current temperature: %.1f°C", temperature_value);
        homekit_characteristic_notify(&cha_current_temperature, cha_current_temperature.value);
    }

    if (abs(humidity_value - last_humidity) > 0.5) {
        last_humidity = humidity_value;
        cha_humidity.value.float_value = humidity_value;
        LOG_D("Humidity: %.1f%%", humidity_value);
        homekit_characteristic_notify(&cha_humidity, cha_humidity.value);
    }

    mqttReconnect(); // Non-blocking reconnect attempt

    if (mqttClient.connected()) {
        // Create JSON payload
        String payload = "{";
        payload += "\"temperature\":" + String(temperature_value) + ",";
        payload += "\"humidity\":" + String(humidity_value);
        payload += "}";

        // Publish to MQTT
        mqttClient.publish(mttopicTemp, payload.c_str());
        mqttClient.publish(mttopicHumid, payload.c_str());
    } else {
        Serial.println("MQTT not connected, skipping publish");
    }
}

int random_value(int min, int max) {
    return min + random(max - min);
}