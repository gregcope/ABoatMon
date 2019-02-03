#include <Arduino.h>
#include "Sleep.h"

Sleep::Sleep() {
  // Constructor
  // when we were made
  _now = 0;
  _time = 0;


  // 1st time
  // time = 0, now = 0
  // having run 4200 millis since boot
  // time = 0 + 8000 + 4200 - 0
  // after 8 sec sleep,
  // millis = 4200 (does not increase on sleep)
  // time == 12200 
  // wake now = 4200 (as we capture millis() on wake)

  // 2nd time
  // assume 1 second run time
  // millis = 5200
  // time = 12200 + 8000 + 5200 - 4200
  // time = 21200
  // time should be ... 12.2 secs + 1 sec + 8 secs = 21200 millis
  // correct !!!
}

uint32_t Sleep::time(void){
  // actual time is time + millis() - now
  // first boot this should be just millis()
  // after any sleep should be adjusted for sleep
  return _time + millis() - _now;  
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
   // woke up and track millis now;
  _now = millis();
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
