#define DEFAULT_FRAMESIZE FRAMESIZE_SXGA

#include <WiFi.h>
#include <WiFiMulti.h>
#include "esp_camera.h"
#include "esp_timer.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"
#include "esp_http_server.h"

#include "html.h"
#include "cam.h"
#include "servo.h"
#include "webserver.h"
#include "wifi.h"


void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detection

  Serial.begin(115200);

  initWifi();

  initCamera();

  initServo(SERVO_PIN);

  initWebServer();

  log_i("Ready.");
}

void loop() {
  wifiMulti.run();
  updateServo();
  yield();
}
