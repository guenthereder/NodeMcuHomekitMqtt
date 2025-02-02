#pragma once
#include <homekit/homekit.h>
#include <homekit/characteristics.h>

#ifdef __cplusplus
extern "C" {
#endif

// Add forward declarations
extern homekit_accessory_t *accessories[];
extern homekit_characteristic_t cha_current_temperature;
extern homekit_characteristic_t cha_humidity;
extern homekit_server_config_t homekit_config; // Renamed from 'config'

void my_accessory_identify(homekit_value_t value);
void set_homekit_code(const char *code);

#ifdef __cplusplus
}
#endif