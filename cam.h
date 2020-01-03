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

class Camera {
private:
  bool use_flashlight;
  bool setup;
public:
  Camera() : use_flashlight(false), setup(true) {}
  virtual ~Camera() {}

  uint8_t setFlashActive(bool a) {
    use_flashlight = a;
    Prefs.putU8(PREFS_KEY_USEFLASHLIGHT, use_flashlight);
    if (use_flashlight == false) {
      digitalWrite(FLASH_LED_PIN, LOW);
    }
    return 0xFF;
  }

  bool getFlashActive() {
    return use_flashlight;
  }

  void flashLedOn() {
    if (use_flashlight == true || setup == true)
      digitalWrite(FLASH_LED_PIN, HIGH);
    else
      digitalWrite(FLASH_LED_PIN, LOW);
  }

  void flashLedOff() {
    if (use_flashlight == true || setup == true)
      digitalWrite(FLASH_LED_PIN, LOW);
  }

  void saveSettings() {
    sensor_t * s = esp_camera_sensor_get();
    Prefs.putU8 (PREFS_KEY_CAM_FRAMESIZE,      s->status.framesize);
    Prefs.putU8 (PREFS_KEY_CAM_QUALITY,        s->status.quality);
    Prefs.putI8 (PREFS_KEY_CAM_BRIGHTNESS,     s->status.brightness);
    Prefs.putI8 (PREFS_KEY_CAM_CONTRAST,       s->status.contrast);
    Prefs.putI8 (PREFS_KEY_CAM_SATURATION,     s->status.saturation);
    Prefs.putU8 (PREFS_KEY_CAM_SPECIAL_EFFECT, s->status.special_effect);
    Prefs.putU8 (PREFS_KEY_CAM_WBMODE,         s->status.wb_mode);
    Prefs.putU8 (PREFS_KEY_CAM_AWB,            s->status.awb);
    Prefs.putU8 (PREFS_KEY_CAM_AWBGAIN,        s->status.awb_gain);
    Prefs.putU8 (PREFS_KEY_CAM_AEC,            s->status.aec);
    Prefs.putU8 (PREFS_KEY_CAM_AEC2,           s->status.aec2);
    Prefs.putI8 (PREFS_KEY_CAM_AELEVEL,        s->status.ae_level);
    Prefs.putU16(PREFS_KEY_CAM_AECVALUE,       s->status.aec_value);
    Prefs.putU8 (PREFS_KEY_CAM_AGC,            s->status.agc);
    Prefs.putU8 (PREFS_KEY_CAM_AGC_GAIN,       s->status.agc_gain);
    Prefs.putU8 (PREFS_KEY_CAM_GAIN_CEIL,      s->status.gainceiling);
    Prefs.putU8 (PREFS_KEY_CAM_RAW,            s->status.raw_gma);
    Prefs.putU8 (PREFS_KEY_CAM_LENC,           s->status.lenc);
    Prefs.putU8 (PREFS_KEY_CAM_VFLIP,          s->status.vflip);
    Prefs.putU8 (PREFS_KEY_CAM_HMIRROR,        s->status.hmirror);

    log_i("Camera settings saved to NVS");
  }

  void loadSettings() {
    sensor_t * s = esp_camera_sensor_get();
    s->set_framesize      (s,(framesize_t)Prefs.getU8(PREFS_KEY_CAM_FRAMESIZE    , FRAMESIZE_SXGA));
    s->set_quality        (s,Prefs.getU8 (PREFS_KEY_CAM_QUALITY                  , 10));
    s->set_brightness     (s,Prefs.getI8 (PREFS_KEY_CAM_BRIGHTNESS               , 0));
    s->set_contrast       (s,Prefs.getI8 (PREFS_KEY_CAM_CONTRAST                 , 0));
    s->set_saturation     (s,Prefs.getI8 (PREFS_KEY_CAM_SATURATION               , 0));
    s->set_special_effect (s,Prefs.getU8 (PREFS_KEY_CAM_SPECIAL_EFFECT           , 0));
    s->set_wb_mode        (s,Prefs.getU8 (PREFS_KEY_CAM_WBMODE                   , 0));
    s->set_whitebal       (s,Prefs.getU8 (PREFS_KEY_CAM_AWB                      , 0));
    s->set_awb_gain       (s,Prefs.getU8 (PREFS_KEY_CAM_AWBGAIN                  , 0));
    s->set_aec_value      (s,Prefs.getU8 (PREFS_KEY_CAM_AEC                      , 0));
    s->set_aec2           (s,Prefs.getU8 (PREFS_KEY_CAM_AEC2                     , 0));
    s->set_ae_level       (s,Prefs.getI8 (PREFS_KEY_CAM_AELEVEL                  , 0));
    s->set_aec_value      (s,Prefs.getU16(PREFS_KEY_CAM_AECVALUE                 , 0));
    s->set_gain_ctrl      (s,Prefs.getU8 (PREFS_KEY_CAM_AGC                      , 1));
    s->set_agc_gain       (s,Prefs.getU8 (PREFS_KEY_CAM_AGC_GAIN                 , 1));
    s->set_gainceiling    (s,(gainceiling_t)Prefs.getU8 (PREFS_KEY_CAM_GAIN_CEIL , GAINCEILING_2X));
    s->set_raw_gma        (s,Prefs.getU8 (PREFS_KEY_CAM_RAW                      , 0));
    s->set_lenc           (s,Prefs.getU8 (PREFS_KEY_CAM_LENC                     , 1));
    s->set_vflip          (s,Prefs.getU8 (PREFS_KEY_CAM_VFLIP                    , 0));
    s->set_hmirror        (s,Prefs.getU8 (PREFS_KEY_CAM_HMIRROR                  , 0));

    log_i("Camera settings loaded from NVS");
  }

  void init() {
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
    config.frame_size   = FRAMESIZE_SXGA;
    config.jpeg_quality = 10;
    config.fb_count     = 2;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
      log_e("Camera init failed with error 0x%x", err);
      ESP.restart();
    }

    loadSettings();

    pinMode(FLASH_LED_PIN, OUTPUT);

    setFlashActive(Prefs.getU8(PREFS_KEY_USEFLASHLIGHT, 0));

    log_i("Flash LED check...");
    flashLedOn();
    delay(300);
    flashLedOff();
    setup = false;
  }
};

Camera Cam;

#endif /* CAM_H_ */
