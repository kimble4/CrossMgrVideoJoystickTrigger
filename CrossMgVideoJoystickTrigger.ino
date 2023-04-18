//use Arduino Micro board

#define VERSION "20230418.1"


#include <Joystick.h>

Joystick_ Joystick;

#define LED_RED 13
#define LED_YELLOW 11
#define LED_BLUE 12
#define IR_INPUT A1
#define TOGGLE A4
#define AUTO_CAPTURE A2
#define CAPTURE A5
#define BUZZER 5

#define CROSSMGR_BUTTON_CAPTURE 0
#define CROSSMGR_BUTTON_AUTOCAPTURE 1
#define CROSSMGR_BUTTON_SNAPSHOT 2

#define RETRIGGER_WAIT_TIME 2000  //milliseconds

unsigned long _beam_last_tripped = 0;
boolean _toggled = false;

void setup() {
  pinMode(CAPTURE, INPUT_PULLUP);
  pinMode(AUTO_CAPTURE, INPUT_PULLUP);
  pinMode(IR_INPUT, INPUT_PULLUP);
  pinMode(TOGGLE, INPUT_PULLUP);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_BLUE, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
  tone(BUZZER, 349);
  delay(500);
  digitalWrite(LED_YELLOW, LOW);
  tone(BUZZER, 392);
  delay(500);
  tone(BUZZER, 440);
  digitalWrite(LED_BLUE, LOW);
  delay(500);
  if (digitalRead(AUTO_CAPTURE) == LOW && digitalRead(CAPTURE) == LOW) {  //lamp test
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_BLUE, LOW);
    do {
      if (digitalRead(TOGGLE) == LOW) {
        tone(BUZZER, 440, 200);
      } else {
        noTone(BUZZER);
      }
      delay(1000);
    } while (digitalRead(AUTO_CAPTURE) == HIGH && digitalRead(CAPTURE) == HIGH);
  }
  noTone(BUZZER);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_BLUE, HIGH);
  delay(500);
  // Initialize Joystick Library
  Joystick.begin();
  _toggled = digitalRead(TOGGLE) == LOW;
  digitalWrite(LED_RED, LOW);
  Joystick.setButton(CROSSMGR_BUTTON_SNAPSHOT, HIGH);
  Joystick.setButton(CROSSMGR_BUTTON_SNAPSHOT, LOW);
}


void loop() {
  if (digitalRead(CAPTURE) == LOW) {
    Joystick.setButton(CROSSMGR_BUTTON_CAPTURE, HIGH);
    digitalWrite(LED_BLUE, LOW);
    tone(BUZZER, 440, 333);
    while (digitalRead(CAPTURE) == LOW) {
      delay(1);
    }
    Joystick.setButton(CROSSMGR_BUTTON_CAPTURE, LOW);
    digitalWrite(LED_BLUE, HIGH);
    noTone(BUZZER);
    delay(100);
  } else if (digitalRead(AUTO_CAPTURE) == LOW) {
    Joystick.setButton(CROSSMGR_BUTTON_AUTOCAPTURE, HIGH);
    digitalWrite(LED_BLUE, LOW);
    tone(BUZZER, 880, 200);
    delay(500);
    Joystick.setButton(CROSSMGR_BUTTON_AUTOCAPTURE, LOW);
    digitalWrite(LED_BLUE, HIGH);
    delay(100);
    while (digitalRead(AUTO_CAPTURE) == LOW) {
      delay(1);
    }
  }
  if (!_beam_last_tripped && digitalRead(IR_INPUT) == HIGH) {
    digitalWrite(LED_YELLOW, HIGH);
    _beam_last_tripped = millis();
    if (_toggled) {
      Joystick.setButton(CROSSMGR_BUTTON_AUTOCAPTURE, HIGH);
      digitalWrite(LED_BLUE, LOW);
      tone(BUZZER, 1760, 200);
      for (int i = 0; i < 500; i++) {
        digitalWrite(LED_YELLOW, digitalRead(IR_INPUT));  //always show beam state on yellow LED
        delay(1);
      }
      Joystick.setButton(CROSSMGR_BUTTON_AUTOCAPTURE, LOW);
      digitalWrite(LED_BLUE, HIGH);
      for (int i = 0; i < 100; i++) {
        digitalWrite(LED_YELLOW, digitalRead(IR_INPUT));  //always show beam state on yellow LED
        delay(1);
      }
    }
  } else if (_beam_last_tripped && millis() - _beam_last_tripped >= RETRIGGER_WAIT_TIME && digitalRead(IR_INPUT) == LOW) {
    _beam_last_tripped = 0;
  }
  boolean tog = digitalRead(TOGGLE) == LOW;
  if (tog != _toggled) {
    _toggled = tog;
    if (_toggled) {
      tone(BUZZER, 392, 333);
    } else {
      tone(BUZZER, 349, 333);
    }
  }
  digitalWrite(LED_YELLOW, digitalRead(IR_INPUT));  //always show beam state on yellow LED
  if (_toggled && _beam_last_tripped && (millis() - _beam_last_tripped)%30000 == 0) {  //buzz every 30s when beam still tripped and toggle enabled
    tone(BUZZER, 220, 1000);
    if ((millis() - _beam_last_tripped)%60000 == 0) {  // single-frame snapshot once per minute when beam still tripped
      Joystick.setButton(CROSSMGR_BUTTON_SNAPSHOT, HIGH);
      Joystick.setButton(CROSSMGR_BUTTON_SNAPSHOT, LOW);
    }
  }
}
