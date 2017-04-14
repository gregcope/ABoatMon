#include <Arduino.h>
#include "Sleep.h"

Sleep::Sleep() {
  // Constructor
}

void Sleep::kip8Secs(void) {
  Serial.println("8 ");
  Serial.flush();
  Serial1.flush();
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

void Sleep::kip2Secs(void) {
  Serial.println("2 ");
  Serial.flush();
  Serial1.flush();
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
}

void Sleep::kip1Sec(void) {
  Serial.println("1 ");
  Serial.flush();
  Serial1.flush();
  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
}
