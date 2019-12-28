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

  res = httpd_resp_send(req, index_html, strlen(index_html));
  return res;
}

static esp_err_t servo_handler(httpd_req_t *req){
  esp_err_t res = ESP_OK;

  size_t buf_len;
  char*  buf;

  bool get = false;
  buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char*)malloc(buf_len);
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      char param[4];
      if (httpd_query_key_value(buf, "pos", param, sizeof(param)) == ESP_OK) {
        if (!strcmp(param,"get")) {
          get = true;
          log_v("Getting servo position");
        } else {
          glb_servopos_deg = atoi(param);
          log_i("Setting servo position to %udeg",glb_servopos_deg);
        }
      }
    }
    free(buf);
  }

  char str[4];
  sprintf(str, "%d", glb_servopos_deg);

  const char * ret = get ? str : "SET SERVOPOS OK";
  httpd_resp_send(req, ret, strlen(ret));
  return res;
}

static esp_err_t set_handler(httpd_req_t *req){
  Serial.println("set_handler");

  int framesize = DEFAULT_FRAMESIZE;
  static int old_framesize = DEFAULT_FRAMESIZE;
  int brightness = 0;
  static int old_brightness = 0;

  char content[100];
  size_t recv_size = min(req->content_len, sizeof(content));
  int returncode = httpd_req_recv(req, content, recv_size);
  if (returncode <= 0) {
    if (returncode == HTTPD_SOCK_ERR_TIMEOUT) httpd_resp_send_408(req);
    return ESP_FAIL;
  }

  const char delim[2] = "=";
  if (strchr(content,delim[0])) {
   char *post;

   post = strtok(content, delim);
   const char * param = post;
   post = strtok(NULL, delim);
   const char * value = post;

   if (!strcmp(param,"framesize"))  framesize  = atoi(value);
   if (!strcmp(param,"brightness")) brightness = atoi(value);
   log_i("param = %s, value = %s", param, value);
  }

  if (old_brightness != brightness) {
    sensor_t * s = esp_camera_sensor_get();
    log_i("Setting brightness to %u", framesize);
    s->set_brightness(s,brightness);
    old_brightness = brightness;
  }

  if (old_framesize != framesize) {
    sensor_t * s = esp_camera_sensor_get();
    log_i("Setting framesize to %u", framesize);
    s->set_framesize(s, (framesize_t)framesize );
    old_framesize = framesize;
    delay(framesize * 100);
  }

  const char * ret = "SET OK";
  httpd_resp_send(req, ret, strlen(ret));
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
  while(true){
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
  return res;
}

static esp_err_t pic_handler(httpd_req_t *req){
    camera_fb_t * fb  = NULL;
    esp_err_t res     = ESP_OK;
    size_t fb_buf_len = 0;

    int64_t fr_start  = esp_timer_get_time();

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

  httpd_uri_t servo_uri = {
    .uri       = "/servo",
    .method    = HTTP_GET,
    .handler   = servo_handler,
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
    httpd_register_uri_handler(httpd, &pic_uri);
    httpd_register_uri_handler(httpd, &set_uri);
    httpd_register_uri_handler(httpd, &servo_uri);
  }

  config.server_port = 81;
  config.ctrl_port   = 32769;
  log_i("Starting vid server on port: '%d'", config.server_port);
  if (httpd_start(&vid_httpd, &config) == ESP_OK) {
      httpd_register_uri_handler(vid_httpd, &vid_uri);
  }
}

#endif /* WEBSERVER_H_ */
