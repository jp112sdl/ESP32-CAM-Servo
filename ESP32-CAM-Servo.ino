#define DEFAULT_FRAMESIZE FRAMESIZE_SXGA
#define NVS_NAMESPACE     "SETTINGS"

#include <WiFi.h>
#include <WiFiMulti.h>
#include "esp_camera.h"
#include "esp_timer.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"
#include "esp_http_server.h"
#include "nvs.h"

#include "prefs.h"
#include "html.h"
#include "cam.h"
#include "servo.h"
#include "webserver.h"
#include "ota_srv.h"
#include "wifi.h"


void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detection

  Serial.begin(115200);

  log_i("VER 1.1");

  Prefs.init(NVS_NAMESPACE);

  initWifi();

  Cam.init();

  Servo.init();

  initWebServer();

  xTaskCreate(&ota_server_task, "ota_server_task", 4096, NULL, 5, NULL);

  log_n("Ready.");
}




void loop() {
  if (wifiMulti.run() != WL_CONNECTED) {
    WiFi.disconnect(false, false);
    wifiMulti.run();
  }
  Servo.disable();
  yield();
}
