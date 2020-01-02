/*
 * wifi.h
 *
 *  Created on: 23 Dec 2019
 *      Author: jp112sdl
 */

#ifndef WIFI_H_
#define WIFI_H_

#include "secrets.h"

WiFiMulti wifiMulti;

void initWifi() {
  WiFi.mode (WIFI_STA);
  WiFi.setSleep(false);

  wifiMulti.addAP(ssid1, password1);
  wifiMulti.addAP(ssid2, password2);

  uint8_t connect_counter = 0;
  Serial.print("Connecting Wifi.");
  WiFi.disconnect();
  WiFi.begin(ssid1, password1);
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (connect_counter++ > 20) {
      log_e("\nWiFi Connect timeout! Restarting ESP!\n");
      ESP.restart();
    }
  }

  log_i("Connected to '%s', IP: %s (%s) ",WiFi.SSID().c_str(),WiFi.localIP().toString().c_str(), WiFi.macAddress().c_str());
}

#endif /* WIFI_H_ */
