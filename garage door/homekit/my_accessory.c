#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include "config.h"

void identify(homekit_value_t _value) {
//  printf("accessory identify\n");
}

homekit_characteristic_t CURRENT_S = HOMEKIT_CHARACTERISTIC_( CURRENT_DOOR_STATE, HOMEKIT_CHARACTERISTIC_CURRENT_DOOR_STATE_CLOSED );
homekit_characteristic_t TARGET_S = HOMEKIT_CHARACTERISTIC_( TARGET_DOOR_STATE, HOMEKIT_CHARACTERISTIC_TARGET_DOOR_STATE_CLOSED );

homekit_accessory_t *accessories[] = {
  HOMEKIT_ACCESSORY(.id = 1, .category = homekit_accessory_category_garage, .services = (homekit_service_t*[]) {
    HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics = (homekit_characteristic_t*[]) {
      HOMEKIT_CHARACTERISTIC(NAME,               CONFIG_DEVICE_NAME),
      HOMEKIT_CHARACTERISTIC(MANUFACTURER,       CONFIG_MANUFACTURER),
      HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER,      CONFIG_SERIAL_NUMBER),
      HOMEKIT_CHARACTERISTIC(MODEL,              CONGIG_MODEL),
      HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION,  CONFIG_FIRMWARE),
      HOMEKIT_CHARACTERISTIC(IDENTIFY,           identify),
      NULL
    }),
    HOMEKIT_SERVICE(GARAGE_DOOR_OPENER, .primary = true, .characteristics = (homekit_characteristic_t*[]) {
      &CURRENT_S,
      &TARGET_S,
      NULL
    }),
    NULL
  }),
  NULL
};

homekit_server_config_t config = {
  .accessories = accessories,
  .password = CONFIG_HOMEKIT_PASSWORD,
  .setupId = CONFIG_HOMEKIT_SETUP_ID
};
