/*
 * servo.h
 *
 *  Created on: 26 Dec 2019
 *      Author: jp112sdl
 *
 */

#ifndef SERVO_H_
#define SERVO_H_

#define INVERT_SERVO_MOVE

#define SERVO_PWM_CH       4   //PWM Kanal
#define SERVO_PIN         12   //IO Pin

#define SERVO_DEG_MIN      0   //min. Stellwinkel
#define SERVO_DEG_MAX    180   //max. Stellwinkel
#define SERVO_DUTY_MIN  2436UL //PWM Duty bei min. Stellwinkel
#define SERVO_DUTY_MAX  8526UL //PWM Duty bei max. Stellwinkel


class ServoControl {
private:
  unsigned long servoMoveStartMillis;
  bool          servoPinIsAttached;
  uint8_t       deg_diff;

public:
  ServoControl() : servoMoveStartMillis(0), servoPinIsAttached(false), deg_diff(0) {}
  virtual ~ServoControl() {}

  void setPos(uint8_t deg) {
    deg_diff = abs(getPos() - deg);
    Prefs.putByte(PREFS_KEY_SERVOPOS, deg);
    servoMoveStartMillis = millis();
#ifdef INVERT_SERVO_MOVE
    deg = SERVO_DEG_MAX - deg;
#endif
    uint32_t duty = map(deg, SERVO_DEG_MIN, SERVO_DEG_MAX, SERVO_DUTY_MIN, SERVO_DUTY_MAX);
    ledcAttachPin(SERVO_PIN, SERVO_PWM_CH);
    servoPinIsAttached = true;
    ledcWrite(SERVO_PWM_CH, duty);
  }

  uint8_t getPos() {
    return Prefs.getByte(PREFS_KEY_SERVOPOS, 90);
  }

  void disable() {
    if (servoPinIsAttached == true && millis() - servoMoveStartMillis > (deg_diff * 6) + 300) {
      ledcDetachPin(SERVO_PIN);
      servoPinIsAttached = false;
    }
  }

  void init() {
    //ledcSetup(2, 50, 16); //channel, freq, resolution
    //ledcAttachPin(2, 2); // pin, channel
    ledcSetup(SERVO_PWM_CH, 50, 16);

    setPos(getPos());
  }

};

ServoControl Servo;

#endif /* SERVO_H_ */
