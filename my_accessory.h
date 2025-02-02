// my_accessory.h
#pragma once
#include <homekit/homekit.h>
#include <homekit/characteristics.h>

#ifdef __cplusplus
extern "C" {
#endif

extern homekit_characteristic_t cha_current_temperature;
extern homekit_characteristic_t cha_humidity;
extern homekit_server_config_t config;

void my_accessory_identify(homekit_value_t value);

#ifdef __cplusplus
}
#endif