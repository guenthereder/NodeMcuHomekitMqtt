#include "my_accessory.h"


void my_accessory_identify(homekit_value_t _value) {
	printf("accessory identify\n");
}

// (required) format: float; HAP section 9.35; min 0, max 100, step 0.1, unit celsius
homekit_characteristic_t cha_current_temperature = HOMEKIT_CHARACTERISTIC_(CURRENT_TEMPERATURE, 0);

// (optional) format: string; HAP section 9.62; max length 64
homekit_characteristic_t cha_name = HOMEKIT_CHARACTERISTIC_(NAME, "Temperature Sensor");

homekit_characteristic_t cha_humidity  = HOMEKIT_CHARACTERISTIC_(CURRENT_RELATIVE_HUMIDITY, 0);

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

homekit_server_config_t config = {
		.accessories = accessories,
		.password = "333-33-333"
};
