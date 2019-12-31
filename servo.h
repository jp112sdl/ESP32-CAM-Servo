/*
 * servo.h
 *
 *  Created on: 26 Dec 2019
 *      Author: jp112sdl
 *
 */

#ifndef SERVO_H_
#define SERVO_H_

#define SERVO_PWM_CH       4   //PWM Kanal
#define SERVO_PIN         12   //IO Pin

#define SERVO_DEG_MIN      0   //min. Stellwinkel
#define SERVO_DEG_MAX    180   //max. Stellwinkel
#define SERVO_DUTY_MIN  2436UL //PWM Duty bei min. Stellwinkel
#define SERVO_DUTY_MAX  8526UL //PWM Duty bei max. Stellwinkel

unsigned long servoMoveStartMillis = 0;
bool          servoPinIsAttached   = false;

void setServoPos(uint8_t deg) {
  servoMoveStartMillis = millis();
  uint32_t duty = map(deg, SERVO_DEG_MIN, SERVO_DEG_MAX, SERVO_DUTY_MIN, SERVO_DUTY_MAX);
  ledcAttachPin(SERVO_PIN, SERVO_PWM_CH);
  servoPinIsAttached = true;
  ledcWrite(SERVO_PWM_CH, duty);
}

void disableServo() {
  if (servoPinIsAttached == true && millis() - servoMoveStartMillis > 1000) {
    ledcDetachPin(SERVO_PIN);
    servoPinIsAttached = false;
  }
}

void initServo() {
  //ledcSetup(2, 50, 16); //channel, freq, resolution
  //ledcAttachPin(2, 2); // pin, channel
  ledcSetup(SERVO_PWM_CH, 50, 16);
  setServoPos(90);
}





#endif /* SERVO_H_ */
