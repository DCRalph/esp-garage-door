#include "config.h"

#include <arduino_homekit_server.h>

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <WiFiManager.h>
WiFiManager wifiManager;

#include <ClickButton.h>
ClickButton B(CONFIG_SW, LOW, "yes");

extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t CURRENT_S;
extern "C" homekit_characteristic_t TARGET_S;
extern "C" homekit_characteristic_t OBSTRUCTION;

//bool sw = false;
//bool lsw = false;

byte relON[] = {0xA0, 0x01, 0x01, 0xA2};
byte relOFF[] = {0xA0, 0x01, 0x00, 0xA1};

void On() {
  Serial.write(relON, sizeof(relON));
}
void Off() {
  Serial.write(relOFF, sizeof(relOFF));
}

#define OPEN_CLOSE_DURATION 22

void wifi_connect() {
  WiFi.setAutoReconnect(true);

  wifiManager.setConfigPortalTimeout(300);
  wifiManager.setConnectTimeout(300);
  wifiManager.setAPStaticIPConfig(IPAddress(4, 3, 2, 1), IPAddress(4, 3, 2, 1), IPAddress(255, 255, 255, 0));
  wifiManager.autoConnect(CONFIG_AP_NAME);

  if(WiFi.status() != WL_CONNECTED){
   ESP.restart();
  }
}

void target_s_set(const homekit_value_t value) {

  uint8_t state = value.bool_value;
  CURRENT_S.value.bool_value = state;

  //  digitalWrite(CONFIG_RELAY, 1);
  //  delay(500);
  //  digitalWrite(CONFIG_RELAY, 0);

  On();
  delay(500);
  Off();

  homekit_characteristic_notify(&CURRENT_S, CURRENT_S.value);
}

void setup() {
  Serial.begin(9600, SERIAL_8N1, SERIAL_TX_ONLY);
  wifi_connect();

  //  pinMode(CONFIG_RELAY, OUTPUT);
  //  digitalWrite(CONFIG_RELAY, LOW);
  B.longClickTime = 5000;

  //  pinMode(CONFIG_SW, INPUT_PULLUP);
  //  sw = digitalRead(CONFIG_SW);
  //  lsw = sw;

  TARGET_S.setter = target_s_set;
  arduino_homekit_setup(&config);
}

void loop() {
  arduino_homekit_loop();
  B.Update();

  if (B.clicks == 1) {
    TARGET_S.value = CURRENT_S.value;
    TARGET_S.value.bool_value = !TARGET_S.value.bool_value;
    target_s_set(TARGET_S.value);
    homekit_characteristic_notify(&TARGET_S, TARGET_S.value);
  }
  if (B.clicks == -1) {
    wifiManager.resetSettings();
    homekit_storage_reset();
    ESP.restart();
  }

  //  sw = digitalRead(CONFIG_SW);
  //  if (sw != lsw) {
  //    lsw = sw;
  //    TARGET_S.value = CURRENT_S.value;
  //    if (!sw) {
  //      TARGET_S.value.bool_value = !TARGET_S.value.bool_value;
  //
  //      target_s_set(TARGET_S.value);
  //      homekit_characteristic_notify(&TARGET_S, TARGET_S.value);
  //    }
  //  }

  delay(10);
}
