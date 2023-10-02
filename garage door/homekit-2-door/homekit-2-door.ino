#include "config.h"

#include <arduino_homekit_server.h>

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <WiFiManager.h>
WiFiManager wifiManager;

extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t CURRENT_1;
extern "C" homekit_characteristic_t TARGET_1;
extern "C" homekit_characteristic_t CURRENT_2;
extern "C" homekit_characteristic_t TARGET_2;

bool lastDoor1;
bool lastDoor2;

unsigned long ms = -999999;

void wifi_connect() {
  WiFi.setAutoReconnect(true);

  wifiManager.setAPStaticIPConfig(IPAddress(4, 3, 2, 1), IPAddress(4, 3, 2, 1), IPAddress(255, 255, 255, 0));
  wifiManager.autoConnect(CONFIG_AP_NAME);

  if(WiFi.status() != WL_CONNECTED){
   ESP.restart();
  }
}
//  homekit_characteristic_notify(&CURRENT_S, CURRENT_S.value);
void target_set_1(const homekit_value_t value) {
  if (value.bool_value) {
    CURRENT_1.value.int_value = HOMEKIT_CHARACTERISTIC_CURRENT_DOOR_STATE_CLOSING;
    TARGET_1.value.int_value = HOMEKIT_CHARACTERISTIC_TARGET_DOOR_STATE_CLOSED;
  } else {
    CURRENT_1.value.int_value = HOMEKIT_CHARACTERISTIC_CURRENT_DOOR_STATE_OPENING;
    TARGET_1.value.int_value = HOMEKIT_CHARACTERISTIC_TARGET_DOOR_STATE_OPEN;
  }

  if (digitalRead(CONFIG_SENSOR_1)) {
    CURRENT_1.value.int_value = HOMEKIT_CHARACTERISTIC_CURRENT_DOOR_STATE_OPEN;
  } else {
    CURRENT_1.value.int_value = HOMEKIT_CHARACTERISTIC_CURRENT_DOOR_STATE_CLOSED;
  }

  homekit_characteristic_notify(&CURRENT_1, CURRENT_1.value);
  homekit_characteristic_notify(&TARGET_1, TARGET_1.value);

  digitalWrite(CONFIG_RELAY_1, CONFIG_RELAY_ON);
  delay(RELAY_PULSE);
  digitalWrite(CONFIG_RELAY_1, !CONFIG_RELAY_ON);
}
void target_set_2(const homekit_value_t value) {
  if (value.bool_value) {
    CURRENT_2.value.int_value = HOMEKIT_CHARACTERISTIC_CURRENT_DOOR_STATE_CLOSING;
    TARGET_2.value.int_value = HOMEKIT_CHARACTERISTIC_TARGET_DOOR_STATE_CLOSED;
  } else {
    CURRENT_2.value.int_value = HOMEKIT_CHARACTERISTIC_CURRENT_DOOR_STATE_OPENING;
    TARGET_2.value.int_value = HOMEKIT_CHARACTERISTIC_TARGET_DOOR_STATE_OPEN;
  }
  
  if (digitalRead(CONFIG_SENSOR_2)) {
    CURRENT_2.value.int_value = HOMEKIT_CHARACTERISTIC_CURRENT_DOOR_STATE_OPEN;
  } else {
    CURRENT_2.value.int_value = HOMEKIT_CHARACTERISTIC_CURRENT_DOOR_STATE_CLOSED;
  }

  homekit_characteristic_notify(&CURRENT_2, CURRENT_2.value);
  homekit_characteristic_notify(&TARGET_2, TARGET_2.value);

  digitalWrite(CONFIG_RELAY_2, CONFIG_RELAY_ON);
  delay(RELAY_PULSE);
  digitalWrite(CONFIG_RELAY_2, !CONFIG_RELAY_ON);
}

void checkSensor1(bool sensor) {
  if (sensor) {
    CURRENT_1.value.int_value = HOMEKIT_CHARACTERISTIC_CURRENT_DOOR_STATE_OPEN;
    TARGET_1.value.int_value = HOMEKIT_CHARACTERISTIC_TARGET_DOOR_STATE_OPEN;
  } else {
    CURRENT_1.value.int_value = HOMEKIT_CHARACTERISTIC_CURRENT_DOOR_STATE_CLOSED;
    TARGET_1.value.int_value = HOMEKIT_CHARACTERISTIC_TARGET_DOOR_STATE_CLOSED;
  }
  homekit_characteristic_notify(&TARGET_1, TARGET_1.value);
  homekit_characteristic_notify(&CURRENT_1, CURRENT_1.value);
}
void checkSensor2(bool sensor) {
  if (sensor) {
    CURRENT_2.value.int_value = HOMEKIT_CHARACTERISTIC_CURRENT_DOOR_STATE_OPEN;
    TARGET_2.value.int_value = HOMEKIT_CHARACTERISTIC_TARGET_DOOR_STATE_OPEN;
  } else {
    CURRENT_2.value.int_value = HOMEKIT_CHARACTERISTIC_CURRENT_DOOR_STATE_CLOSED;
    TARGET_2.value.int_value = HOMEKIT_CHARACTERISTIC_TARGET_DOOR_STATE_CLOSED;
  }
  homekit_characteristic_notify(&TARGET_2, TARGET_2.value);
  homekit_characteristic_notify(&CURRENT_2, CURRENT_2.value);
}

void trigger1(){
    TARGET_1.value = CURRENT_1.value;
    TARGET_1.value.bool_value = !TARGET_1.value.bool_value;
    target_set_1(TARGET_1.value);
}

void trigger2(){
    TARGET_2.value = CURRENT_2.value;
    TARGET_2.value.bool_value = !TARGET_2.value.bool_value;
    target_set_2(TARGET_2.value);
}

void setup() {
  Serial.begin(115200);
//  homekit_storage_reset();
  wifi_connect();

  pinMode(CONFIG_RELAY_1, OUTPUT);
  pinMode(CONFIG_RELAY_2, OUTPUT);
  pinMode(CONFIG_SENSOR_1, INPUT_PULLUP);
  pinMode(CONFIG_SENSOR_2, INPUT_PULLUP);

  digitalWrite(CONFIG_RELAY_1, !CONFIG_RELAY_ON);
  digitalWrite(CONFIG_RELAY_2, !CONFIG_RELAY_ON);

  lastDoor1 = digitalRead(CONFIG_SENSOR_1);
  lastDoor2 = digitalRead(CONFIG_SENSOR_2);

  TARGET_1.setter = target_set_1;
  TARGET_2.setter = target_set_2;
  arduino_homekit_setup(&config);
}

void loop() {
  arduino_homekit_loop();

  if (digitalRead(CONFIG_SENSOR_1) != lastDoor1) {
    lastDoor1 = digitalRead(CONFIG_SENSOR_1);
    checkSensor1(lastDoor1);
  }

  if (digitalRead(CONFIG_SENSOR_2) != lastDoor2) {
    lastDoor2 = digitalRead(CONFIG_SENSOR_2);
    checkSensor2(lastDoor2);
  }

  if (millis() - ms > 30000) {
    ms = millis(); 
  }

  delay(10);
}
