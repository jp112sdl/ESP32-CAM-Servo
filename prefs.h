/*
 * prefs.h
 *
 *  Created on: 1 Jan 2020
 *      Author: pechj
 */

#ifndef PREFS_H_
#define PREFS_H_

const char * PREFS_KEY_SERVOPOS      = "servopos";
const char * PREFS_KEY_USEFLASHLIGHT = "useflashlight";

const char * nvs_errors[] = { "OTHER", "NOT_INITIALIZED", "NOT_FOUND", "TYPE_MISMATCH", "READ_ONLY", "NOT_ENOUGH_SPACE", "INVALID_NAME", "INVALIDprefs_handle", "REMOVE_FAILED", "KEY_TOO_LONG", "PAGE_FULL", "INVALID_STATE", "INVALID_LENGHT"};
#define nvs_error(e) (((e)>ESP_ERR_NVS_BASE)?nvs_errors[(e)&~(ESP_ERR_NVS_BASE)]:nvs_errors[0])

class NVSPreferences {
protected:
  uint32_t prefs_handle;
  bool     prefs_open;

public:
  NVSPreferences() : prefs_handle(0), prefs_open(false) {}
  virtual ~NVSPreferences () {}

  size_t getPrefsFree() {
    nvs_stats_t nvs_stats;
    esp_err_t err = nvs_get_stats(NULL, &nvs_stats);
    if(err){
      log_e("Failed to get nvs statistics");
      return 0;
    }
    return nvs_stats.free_entries;
  }

  bool init(const char * name) {
    if(prefs_open){
      return false;
    }
    esp_err_t err = nvs_open(name, NVS_READWRITE, &prefs_handle);
    if(err){
      log_e("nvs_open failed: %s", nvs_error(err));
      return false;
    }
    log_i("NVS free entries: %u", getPrefsFree());
    prefs_open = true;
    return true;
  }

  bool remove(const char * key) {
    if (!prefs_open) return false;

    esp_err_t err = nvs_erase_key(prefs_handle, key);
    if(err){
      log_e("nvs_erase_key fail: %s %s", key, nvs_error(err));
      return false;
   }
    return true;
  }

  size_t putByte(const char* key, uint8_t value){
    if(!prefs_open){
      return 0;
    }
    esp_err_t err = nvs_set_u8(prefs_handle, key, value);
    if(err){
      log_e("nvs_set_u8 fail: %s %s", key, nvs_error(err));
      return 0;
    }
    err = nvs_commit(prefs_handle);
    if(err){
      log_e("nvs_commit fail: %s %s", key, nvs_error(err));
      return 0;
    }
    return 1;
  }

  size_t putString(const char* key, const char* value){
    if(!prefs_open){
      return 0;
    }
    esp_err_t err = nvs_set_str(prefs_handle, key, value);
    if(err){
      log_e("nvs_set_str fail: %s %s", key, nvs_error(err));
      return 0;
    }
    err = nvs_commit(prefs_handle);
    if(err){
      log_e("nvs_commit fail: %s %s", key, nvs_error(err));
      return 0;
    }
    return strlen(value);
  }

  size_t putString(const char* key, const String value){
    return putString(key, value.c_str());
  }

  uint8_t getByte(const char* key, const uint8_t defaultValue){
    uint8_t value = defaultValue;
    if(!prefs_open){
      return value;
    }
    esp_err_t err = nvs_get_u8(prefs_handle, key, &value);
    if(err){
      log_v("nvs_get_u8 fail: %s %s", key, nvs_error(err));
    }
    return value;
  }

  String getString(const char* key, const String defaultValue){
    char * value = NULL;
    size_t len = 0;
    if(!prefs_open){
      return String(defaultValue);
    }
    esp_err_t err = nvs_get_str(prefs_handle, key, value, &len);
    if(err){
      log_e("nvs_get_str len fail: %s %s", key, nvs_error(err));
      return String(defaultValue);
    }
    char buf[len];
    value = buf;
    err = nvs_get_str(prefs_handle, key, value, &len);
    if(err){
      log_e("nvs_get_str fail: %s %s", key, nvs_error(err));
      return String(defaultValue);
    }
    return String(buf);
  }

};

NVSPreferences Prefs;

#endif /* PREFS_H_ */
