#include <PubSubClient.h>
#include <Arduino.h>
#include <arduino_homekit_server.h>
#include <WiFiManager.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "my_accessory.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define LOG_D(fmt, ...) printf_P(PSTR(fmt "\n"), ##__VA_ARGS__);
#define CONFIG_FILE "/config.json"

Adafruit_BME280 bme;
unsigned long delayTime;
float last_temperature = -1000;
float last_humidity = -1000;

// MQTT Client
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Configuration structure
struct AppConfig {
  char mqtt_server[40];
  char mqtt_port[6];
  char mttopicTemp[64];
  char mttopicHumid[64];
  char homekit_code[11];
};


extern "C" void set_homekit_code(const char *code);

AppConfig appConfig;

void initializeDefaultConfig() {
  // MQTT Defaults
  strlcpy(appConfig.mqtt_server, "192.168.1.100", sizeof(appConfig.mqtt_server));
  strlcpy(appConfig.mqtt_port, "1883", sizeof(appConfig.mqtt_port));
  strlcpy(appConfig.mttopicTemp, "sensors/livingroom/temperature", sizeof(appConfig.mttopicTemp));
  strlcpy(appConfig.mttopicHumid, "sensors/livingroom/humidity", sizeof(appConfig.mttopicHumid));
  
  // HomeKit Default
  strlcpy(appConfig.homekit_code, "333-33-333", sizeof(appConfig.homekit_code));
}

void setup() {
  Serial.begin(115200);
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor!");
  }

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount SPIFFS");
    return;
  }

  // Initialize with defaults first
  initializeDefaultConfig();

  // Load or create config
  if (!loadConfig()) {
    startConfigPortal();
  }

  // Connect to WiFi using stored credentials
  connectWifi();

  // Initialize HomeKit
  set_homekit_code(appConfig.homekit_code);
  my_homekit_setup();

  // Setup MQTT
  mqttClient.setServer(appConfig.mqtt_server, atoi(appConfig.mqtt_port));
}

// Add these functions before loop()
bool loadConfig() {
  File file = SPIFFS.open(CONFIG_FILE, "r");
  if (!file) {
    Serial.println("No config file found");
    return false;
  }

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("Failed to parse config file");
    file.close();
    return false;
  }

  // Load values while preserving defaults for missing fields
  strlcpy(appConfig.mqtt_server, doc["mqtt_server"] | appConfig.mqtt_server, sizeof(appConfig.mqtt_server));
  strlcpy(appConfig.mqtt_port, doc["mqtt_port"] | appConfig.mqtt_port, sizeof(appConfig.mqtt_port));
  strlcpy(appConfig.mttopicTemp, doc["mttopicTemp"] | appConfig.mttopicTemp, sizeof(appConfig.mttopicTemp));
  strlcpy(appConfig.mttopicHumid, doc["mttopicHumid"] | appConfig.mttopicHumid, sizeof(appConfig.mttopicHumid));
  strlcpy(appConfig.homekit_code, doc["homekit_code"] | appConfig.homekit_code, sizeof(appConfig.homekit_code));

  file.close();
  return true;
}

void saveConfig() {
  StaticJsonDocument<512> doc;
  doc["mqtt_server"] = appConfig.mqtt_server;
  doc["mqtt_port"] = appConfig.mqtt_port;
  doc["mttopicTemp"] = appConfig.mttopicTemp;
  doc["mttopicHumid"] = appConfig.mttopicHumid;
  doc["homekit_code"] = appConfig.homekit_code;

  File file = SPIFFS.open(CONFIG_FILE, "w");
  if (!file) {
    Serial.println("Failed to open config file for writing");
    return;
  }

  serializeJson(doc, file);
  file.close();
}

bool isValidHomeKitCode(const char* code) {
  return strlen(code) == 10 && 
         code[3] == '-' && 
         code[6] == '-' &&
         isdigit(code[0]) && isdigit(code[1]) && isdigit(code[2]) &&
         isdigit(code[4]) && isdigit(code[5]) &&
         isdigit(code[7]) && isdigit(code[8]) && isdigit(code[9]);
}

void startConfigPortal() {
  WiFiManager wm;
  wm.setBreakAfterConfig(true);
  wm.setCustomHeadElement("<style>li{list-style:none;} input{margin:10px 0;}</style>");

  // Create parameters with placeholders
  WiFiManagerParameter custom_mqtt_server("server", "MQTT Server", appConfig.mqtt_server, 40, "placeholder=\"192.168.1.100\"");
  WiFiManagerParameter custom_mqtt_port("port", "MQTT Port", appConfig.mqtt_port, 6, "placeholder=\"1883\"");
  WiFiManagerParameter custom_mttopicTemp("topic_temp", "Temperature Topic", appConfig.mttopicTemp, 64, "placeholder=\"sensors/utilityroom/temperature\"");
  WiFiManagerParameter custom_mttopicHumid("topic_humid", "Humidity Topic", appConfig.mttopicHumid, 64, "placeholder=\"sensors/utilityroom/humidity\"");
  WiFiManagerParameter custom_homekit_code("homekit", "HomeKit Setup Code", appConfig.homekit_code, 11, "placeholder=\"333-33-333\"");

  // Add parameters
  wm.addParameter(&custom_mqtt_server);
  wm.addParameter(&custom_mqtt_port);
  wm.addParameter(new WiFiManagerParameter("<div class='note'>Use default values or customize:</div>"));
  wm.addParameter(&custom_mttopicTemp);
  wm.addParameter(&custom_mttopicHumid);
  wm.addParameter(new WiFiManagerParameter("<div class='note'>HomeKit setup code format: XXX-XX-XXX</div>"));
  wm.addParameter(&custom_homekit_code);

  if (!wm.startConfigPortal("NodeMCU-Sensor")) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
  }

  strcpy(appConfig.mqtt_server, custom_mqtt_server.getValue());
  strcpy(appConfig.mqtt_port, custom_mqtt_port.getValue());
  strcpy(appConfig.mttopicTemp, custom_mttopicTemp.getValue());
  strcpy(appConfig.mttopicHumid, custom_mttopicHumid.getValue());
  strcpy(appConfig.homekit_code, custom_homekit_code.getValue());

  // Modify in startConfigPortal() after getting values:
  if (!isValidHomeKitCode(appConfig.homekit_code)) {
    Serial.println("Invalid HomeKit code format! Using default.");
    strlcpy(appConfig.homekit_code, "333-33-333", sizeof(appConfig.homekit_code));
  }

  saveConfig();
}

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin();

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to WiFi, starting config portal");
    startConfigPortal();
  }

  Serial.printf("\nWiFi connected, IP: %s\n", WiFi.localIP().toString().c_str());
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

static uint32_t next_heap_millis = 0;
static uint32_t next_report_millis = 0;

void my_homekit_setup() {
    arduino_homekit_setup(&homekit_config);
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
        mqttClient.publish(appConfig.mttopicTemp, payload.c_str());
        mqttClient.publish(appConfig.mttopicHumid, payload.c_str());
} else {
        Serial.println("MQTT not connected, skipping publish");
    }
}

int random_value(int min, int max) {
    return min + random(max - min);
}