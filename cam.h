/*
 * cam.h
 *
 *  Created on: 23 Dec 2019
 *      Author: jp112sdl
 */

#ifndef CAM_H_
#define CAM_H_

// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define FLASH_LED_PIN      4
#define FLASH_LED_PWM_CH   5               // PWM-Kanal Blitz

void initCamera() {
  // OV2640 camera module
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size   = DEFAULT_FRAMESIZE;
  config.jpeg_quality = 8;
  config.fb_count     = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    log_e("Camera init failed with error 0x%x", err);
    ESP.restart();
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_brightness(s,1);

  ledcSetup(FLASH_LED_PWM_CH, 500, 8);
  ledcAttachPin(FLASH_LED_PIN, FLASH_LED_PWM_CH);
  ledcWrite(FLASH_LED_PWM_CH, 0);

  log_i("Flash LED check...");
  ledcWrite(FLASH_LED_PWM_CH, 255);
  delay(500);
  ledcWrite(FLASH_LED_PWM_CH, 0);
}

#endif /* CAM_H_ */
