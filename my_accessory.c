#include "my_accessory.h"


// Define characteristics FIRST
homekit_characteristic_t cha_current_temperature = HOMEKIT_CHARACTERISTIC_(CURRENT_TEMPERATURE, 0);
homekit_characteristic_t cha_humidity = HOMEKIT_CHARACTERISTIC_(CURRENT_RELATIVE_HUMIDITY, 0);

// Add the missing name characteristic
homekit_characteristic_t cha_name = HOMEKIT_CHARACTERISTIC_(NAME, "Environmental Sensor");

homekit_accessory_t *accessories[] = {
  HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_sensor, .services=(homekit_service_t*[]) {
      HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
          HOMEKIT_CHARACTERISTIC(NAME, "Temperature Sensor"),
          HOMEKIT_CHARACTERISTIC(MANUFACTURER, "arduino-projekte.info"),
          HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "0123789"),
          HOMEKIT_CHARACTERISTIC(MODEL, "ESP8266"),
          HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.0"),
          HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
          NULL
      }),
      HOMEKIT_SERVICE(TEMPERATURE_SENSOR, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
          &cha_current_temperature,
          &cha_name,
          NULL
      }),
      HOMEKIT_SERVICE(HUMIDITY_SENSOR, .characteristics=(homekit_characteristic_t*[]) {
          HOMEKIT_CHARACTERISTIC(NAME, "Humidity Sensor"),
          &cha_humidity,
          NULL
      }),
      NULL
    }),
    NULL
};


// Finally define homekit_config
homekit_server_config_t homekit_config = {
    .accessories = accessories,
    .password = "333-33-333" // Default value
};

void set_homekit_code(const char *code) {
    homekit_config.password = code;
}

void my_accessory_identify(homekit_value_t _value) {
    printf("accessory identify\n");
}