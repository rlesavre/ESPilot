#ifndef ACTIVITYLED_H
#define ACTIVITYLED_H

#include "Arduino.h"
#include "BaseModule.h"

#define LEDPIN 15
#define LOOPCOUNT_THRESHOLD 1000

class ActivityLedModule: public IBaseModule {
public:
  ActivityLedModule(){}
  const char* getName() {return "ActivityLed";}
  IScreenPage* getDisplayPage() {return NULL;}
  void setup(){
    pinMode(LEDPIN, OUTPUT);
    digitalWrite(LEDPIN, ledToggle ? HIGH:LOW);
  }
  void loop(){
    if(loopcount++ > LOOPCOUNT_THRESHOLD){
      ledToggle = !ledToggle;
      digitalWrite(LEDPIN, ledToggle ? HIGH:LOW);
      loopcount = 0;
    }
  }
private:
  int loopcount = 0;
  bool ledToggle = true;
};
#endif