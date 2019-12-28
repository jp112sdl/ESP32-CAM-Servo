/*
 * servo.h
 *
 *  Created on: 26 Dec 2019
 *      Author: jp112sdl
 *
 *      Servo http://pastebin.com/PtqKpd9p
 */

#ifndef SERVO_H_
#define SERVO_H_

#define SERVER_MAX_DEG   114UL
#define SERVER_MIN_DEG    13UL             // Minimalwinkel für das Servo
#define SERVO_MAX_APT    205UL   // Gradzahl um die der Servo verstellt werden kann

#define SERVO_PWM_CH    6
#define SERVO_DELAY_MS 20

#define SERVO_PIN    12

uint8_t glb_servopos_deg = 0;

void initServo() {
  ledcSetup(SERVO_PWM_CH, 166, 8);
  ledcAttachPin(SERVO_PIN, SERVO_PWM_CH);
  //ledcWrite(SERVO_PWM_CH, SERVER_MIN_DEG);
  delay(100);
}

void updateServo() {
  uint32_t duty = ((glb_servopos_deg/(SERVO_MAX_APT))*(SERVER_MAX_DEG-SERVER_MIN_DEG)) + SERVER_MIN_DEG;
  ledcWrite(SERVO_PWM_CH, duty);
  delay(SERVO_DELAY_MS);
}



#endif /* SERVO_H_ */
