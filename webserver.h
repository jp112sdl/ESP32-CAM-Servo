/*
 * webserver.h
 *
 *  Created on: 23 Dec 2019
 *      Author: jp112sdl
 */

#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";

httpd_handle_t httpd = NULL;
httpd_handle_t vid_httpd = NULL;

static esp_err_t index_handler(httpd_req_t *req){
  esp_err_t res = ESP_OK;

  std::string s;
  s.append(index_html_pre_css);
  s.append(css);
  s.append(index_html_post_css);

  res = httpd_resp_send(req, s.c_str(), s.size());
  return res;
}

static esp_err_t settings_handler(httpd_req_t *req){
  esp_err_t res = ESP_OK;

  std::string s;
  s.append(settings_html_pre_css);
  s.append(css);
  s.append(settings_html_post_css);

  res = httpd_resp_send(req, s.c_str(), s.size());

  return res;
}

static esp_err_t status_handler(httpd_req_t *req){
    static char json_response[1024];

    sensor_t * s = esp_camera_sensor_get();
    char * p = json_response;
    *p++ = '{';

    p+=sprintf(p, "\"framesize\":%u,", s->status.framesize);
    p+=sprintf(p, "\"quality\":%u,", s->status.quality);
    p+=sprintf(p, "\"brightness\":%d,", s->status.brightness);
    p+=sprintf(p, "\"contrast\":%d,", s->status.contrast);
    p+=sprintf(p, "\"saturation\":%d,", s->status.saturation);
    p+=sprintf(p, "\"sharpness\":%d,", s->status.sharpness);
    p+=sprintf(p, "\"special_effect\":%u,", s->status.special_effect);
    p+=sprintf(p, "\"wb_mode\":%u,", s->status.wb_mode);
    p+=sprintf(p, "\"awb\":%u,", s->status.awb);
    p+=sprintf(p, "\"awb_gain\":%u,", s->status.awb_gain);
    p+=sprintf(p, "\"aec\":%u,", s->status.aec);
    p+=sprintf(p, "\"aec2\":%u,", s->status.aec2);
    p+=sprintf(p, "\"ae_level\":%d,", s->status.ae_level);
    p+=sprintf(p, "\"aec_value\":%u,", s->status.aec_value);
    p+=sprintf(p, "\"agc\":%u,", s->status.agc);
    p+=sprintf(p, "\"agc_gain\":%u,", s->status.agc_gain);
    p+=sprintf(p, "\"gainceiling\":%u,", s->status.gainceiling);
    p+=sprintf(p, "\"raw_gma\":%u,", s->status.raw_gma);
    p+=sprintf(p, "\"lenc\":%u,", s->status.lenc);
    p+=sprintf(p, "\"vflip\":%u,", s->status.vflip);
    p+=sprintf(p, "\"hmirror\":%u", s->status.hmirror);
    *p++ = '}';
    *p++ = 0;
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json_response, strlen(json_response));
}

static esp_err_t get_handler(httpd_req_t *req){
  esp_err_t res = ESP_OK;

  size_t buf_len;
  char*  buf;


  char str[4];
  buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char*)malloc(buf_len);
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      char param[20];
      if (httpd_query_key_value(buf, "item", param, sizeof(param)) == ESP_OK) {
        log_i("item = %s", param);

        if(!strcmp(param,"servopos"))
          sprintf(str, "%d", Servo.getPos());

        if(!strcmp(param,"flashlight"))
          sprintf(str, "%d", Cam.getFlashActive());
      }
    }
    free(buf);
  }

  log_i("response = %s", str);

  httpd_resp_send(req, str, strlen(str));
  return res;
}

static esp_err_t set_handler(httpd_req_t *req){

  char content[100];
  size_t recv_size = min(req->content_len, sizeof(content));
  int returncode = httpd_req_recv(req, content, recv_size);
  if (returncode <= 0) {
    log_e("err %u", returncode);
    if (returncode == HTTPD_SOCK_ERR_TIMEOUT) httpd_resp_send_408(req);
    return ESP_FAIL;
  }

  log_d("set_handler: %s", content);

  int res = -1;

  const char delim[2] = "=";
  if (strchr(content,delim[0])) {
   char *post;

   post = strtok(content, delim);
   const char * param = post;
   post = strtok(NULL, delim);
   const char * value = post;

   int val = atoi(value);

   sensor_t * s = esp_camera_sensor_get();

        if (!strcmp(param, "framesize"))      res = s->set_framesize(s,(framesize_t)val); // 0 ... 10
   else if (!strcmp(param, "quality"))        res = s->set_quality(s, val);
   else if (!strcmp(param, "contrast"))       res = s->set_contrast(s, val);
   else if (!strcmp(param, "brightness"))     res = s->set_brightness(s, val);
   else if (!strcmp(param, "saturation"))     res = s->set_saturation(s, val);
   else if (!strcmp(param, "sharpness"))      res = s->set_sharpness(s, val);
   else if (!strcmp(param, "gainceiling"))    res = s->set_gainceiling(s, (gainceiling_t)val);
   else if (!strcmp(param, "colorbar"))       res = s->set_colorbar(s, val);
   else if (!strcmp(param, "awb"))            res = s->set_whitebal(s, val);
   else if (!strcmp(param, "agc"))            res = s->set_gain_ctrl(s, val);
   else if (!strcmp(param, "aec"))            res = s->set_exposure_ctrl(s, val);
   else if (!strcmp(param, "hmirror"))        res = s->set_hmirror(s, val);
   else if (!strcmp(param, "vflip"))          res = s->set_vflip(s, val);
   else if (!strcmp(param, "awb_gain"))       res = s->set_awb_gain(s, val);
   else if (!strcmp(param, "agc_gain"))       res = s->set_agc_gain(s, val);
   else if (!strcmp(param, "aec_value"))      res = s->set_aec_value(s, val);
   else if (!strcmp(param, "aec2"))           res = s->set_aec2(s, val);
   else if (!strcmp(param, "raw_gma"))        res = s->set_raw_gma(s, val);
   else if (!strcmp(param, "lenc"))           res = s->set_lenc(s, val);
   else if (!strcmp(param, "special_effect")) res = s->set_special_effect(s, val);
   else if (!strcmp(param, "wb_mode"))        res = s->set_wb_mode(s, val);
   else if (!strcmp(param, "ae_level"))       res = s->set_ae_level(s, val);
   else if (!strcmp(param, "servopos"))       res = Servo.setPos(atoi(value));
   else if (!strcmp(param, "flashlight"))     res = Cam.setFlashActive(atoi(value));

   log_i("param = %s, value = %s, res = %u", param, value, res);
  }

  if(res == -1) return httpd_resp_send_500(req);
  else if (res != 0xFF) Cam.saveSettings();


  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);
  return ESP_OK;
}

static esp_err_t vid_handler(httpd_req_t *req) {
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t fb_buf_len = 0;
  uint8_t * fb_buf = NULL;
  char * part_buf[64];
  res = httpd_resp_set_type(req, "multipart/x-mixed-replace;boundary=" PART_BOUNDARY);

  if(res != ESP_OK){
    return res;
  }
  log_i("opening vid_handler for streaming");
  while(true){
    Cam.flashLedOn();
    fb = esp_camera_fb_get();

    if (!fb) {
      log_e("Camera capture failed");
      res = ESP_FAIL;
    } else {
      fb_buf_len = fb->len;
      fb_buf = fb->buf;
    }
    if(res == ESP_OK){
      size_t hlen = snprintf((char *)part_buf, 64, "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, (const char *)fb_buf, fb_buf_len);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if(fb){
      esp_camera_fb_return(fb);
      fb = NULL;
      fb_buf = NULL;
    } else if(fb_buf){
      free(fb_buf);
      fb_buf = NULL;
    }
    if(res != ESP_OK){
      break;
    }
      //log_i("MJPG: %uB",(uint32_t)(fb_buf_len));
  }
  Cam.flashLedOff();
  return res;
}

static esp_err_t pic_handler(httpd_req_t *req){
    camera_fb_t * fb  = NULL;
    esp_err_t res     = ESP_OK;
    size_t fb_buf_len = 0;

    int64_t fr_start  = esp_timer_get_time();

    Cam.flashLedOn();

    delay(100);

    fb = esp_camera_fb_get();

    if (!fb) {
      log_e("Camera capture failed");
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }

    res = httpd_resp_set_type(req, "image/jpeg");
    if(res == ESP_OK){
      res = httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    }
    if(res == ESP_OK){
      fb_buf_len = fb->len;
      res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
    }

    esp_camera_fb_return(fb);

    Cam.flashLedOff();

    int64_t fr_end = esp_timer_get_time();

    log_i("JPG: %uKB %ums", (uint32_t)(fb_buf_len/1024), (uint32_t)((fr_end - fr_start)/1000));

    return res;

  return ESP_OK;
}

void initWebServer(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t settings_uri = {
    .uri       = "/settings",
    .method    = HTTP_GET,
    .handler   = settings_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t status_uri = {
      .uri       = "/status",
      .method    = HTTP_GET,
      .handler   = status_handler,
      .user_ctx  = NULL
  };

  httpd_uri_t pic_uri = {
    .uri       = "/pic",
    .method    = HTTP_GET,
    .handler   = pic_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t vid_uri = {
    .uri       = "/vid",
    .method    = HTTP_GET,
    .handler   = vid_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t get_uri = {
    .uri       = "/get",
    .method    = HTTP_GET,
    .handler   = get_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t set_uri = {
    .uri       = "/set",
    .method    = HTTP_POST,
    .handler   = set_handler,
    .user_ctx  = NULL
  };

  log_i("Starting web server on port: '%d'", config.server_port);
  if (httpd_start(&httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(httpd, &index_uri);
    httpd_register_uri_handler(httpd, &settings_uri);
    httpd_register_uri_handler(httpd, &status_uri);
    httpd_register_uri_handler(httpd, &pic_uri);
    httpd_register_uri_handler(httpd, &set_uri);
    httpd_register_uri_handler(httpd, &get_uri);
  }

  config.server_port = 81;
  config.ctrl_port   = 32769;
  log_i("Starting vid server on port: '%d'", config.server_port);
  if (httpd_start(&vid_httpd, &config) == ESP_OK) {
      httpd_register_uri_handler(vid_httpd, &vid_uri);
  }
}

#endif /* WEBSERVER_H_ */
