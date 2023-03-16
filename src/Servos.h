#ifndef SERVOS_MODULE
#define SERVOS_MODULE

#include <vector>

#include "Arduino.h" 
#include "BaseModule.h"
#include "Common.h"

// Measured values for servos
#define MIN_SG90_MICROS      200
#define MAX_SG90_MICROS      1350
#define MIN_DM996_MICROS      200
#define MAX_DM996_MICROS      1250

#define PIN_SERVO_1 40
#define PIN_SERVO_2 39
#define PIN_SERVO_3 36
#define PIN_SERVO_4 34
#define PIN_SERVO_5 18
#define PIN_SERVO_6 16
#define PIN_SERVO_7 15
#define PIN_SERVO_8 38

#define DELAY 20


// Select different ESP32 timer number (0-3) to avoid conflict
#define USE_ESP32_TIMER_NO          3

class ServosModule: public IBaseModule, public IScreenPage {
public:
  const char* getName() {return "Servos";}
  IScreenPage* getDisplayPage() {return this;}
  void displayScreenPage(IDisplay *display, int position);

	ServosModule();
	void setup();
	void loop();

private:
  unsigned long lastloop = 0;
  int servo1position = 0;
  int servoIndex1 = -1;
};

#endif