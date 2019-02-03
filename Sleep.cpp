#include <Arduino.h>
#include "Sleep.h"

Sleep::Sleep() {
  // Constructor
  _now = millis();
}

uint32_t Sleep::time(void){
  return _now + _time;  
}

void Sleep::kip8Secs(void) {
  Serial.println("Sleep ... 8 secs");
  Serial.flush();
  Serial1.flush();

  // set time before kipping
  _time = _time + 8000 + millis() - _now;
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);

  // woke up and track millis now;
  _now = millis();
  Serial.flush();
  Serial1.flush();
}

void Sleep::kip2Secs(void) {
  Serial.println("Sleep ... 2 secs");
  Serial.flush();
  Serial1.flush();
  // set time before kipping
  _time = _time + 2000 + millis() - _now;
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
  Serial.flush();
  Serial1.flush();
}

void Sleep::kip1Sec(void) {
  Serial.println("sleep ... 1sec");
  Serial.flush();
  Serial1.flush();
  // set time before kipping
  _time = _time + 1000 + millis() - _now;
  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
   // woke up and track millis now;
  _now = millis();
  Serial.flush();
  Serial1.flush();
}
