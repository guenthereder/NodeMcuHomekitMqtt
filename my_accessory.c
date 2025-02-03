#include "my_accessory.h"

// Define characteristics FIRST
homekit_characteristic_t cha_current_temperature = HOMEKIT_CHARACTERISTIC_(CURRENT_TEMPERATURE, 0);
homekit_characteristic_t cha_humidity = HOMEKIT_CHARACTERISTIC_(CURRENT_RELATIVE_HUMIDITY, 0);

// Add the missing name characteristic
homekit_characteristic_t cha_name = HOMEKIT_CHARACTERISTIC_(NAME, "Environmental Sensor");
homekit_characteristic_t serial_number = HOMEKIT_CHARACTERISTIC_(SERIAL_NUMBER, NULL);

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


static void homekit_event_handler(homekit_event_t event) {
    if (event == HOMEKIT_EVENT_PAIRING_ADDED) {
        printf("Pairing added!\n");
    } else if (event == HOMEKIT_EVENT_PAIRING_REMOVED) {
        printf("Pairing removed!\n");
        homekit_storage_reset();
    }
}


// Finally define homekit_config
homekit_server_config_t homekit_config = {
    .accessories = accessories,
    .password = "333-33-333",
    .on_event = homekit_event_handler
};

void set_homekit_code(const char *code) {
    homekit_config.password = code;
}

void my_accessory_identify(homekit_value_t _value) {
    printf("accessory identify\n");
}

void create_accessory_name(uint32_t chip_id) {
    // uint32_t chip_id = ESP.getChipId();
    char serial_str[13];
    snprintf(serial_str, sizeof(serial_str), "%08X", chip_id);
    serial_number.value = HOMEKIT_STRING(serial_str);
}
