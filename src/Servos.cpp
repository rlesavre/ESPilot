#include "Servos.h"

#ifndef ESP32
	#error This code is designed to run on ESP32 platform, not Arduino nor ESP8266! Please check your Tools->Board setting.
#endif
#define TIMER_INTERRUPT_DEBUG       1
#define ISR_SERVO_DEBUG             0
#include "ESP32_ISR_Servo.h"

ServosModule::ServosModule() {

}

void ServosModule::setup() {
  // Read configuration

  // Setup servo indice

  // Sample
  Serial.print(F("\nStarting ServosModule on "));
	Serial.println(ARDUINO_BOARD);
	Serial.println(ESP32_ISR_SERVO_VERSION);
  //Select ESP32 timer USE_ESP32_TIMER_NO
	ESP32_ISR_Servos.useTimer(USE_ESP32_TIMER_NO);

  servoIndex1 = ESP32_ISR_Servos.setupServo(PIN_SERVO_1, MIN_SG90_MICROS, MAX_SG90_MICROS);
  if (servoIndex1 != -1)
		Serial.println(F("Setup Servo1 OK"));
	else
		Serial.println(F("Setup Servo1 failed"));
}

void ServosModule::loop() {
  // LOOP

  // Sample move
  if(millis() - lastloop > DELAY){
    servo1position++;
    servo1position = servo1position>360 ? 0 : servo1position;
    if(servo1position>0){
      ESP32_ISR_Servos.setPosition(servoIndex1, servo1position<180 ? servo1position : 360 - servo1position);
    }
    lastloop = millis();
  }
}

void ServosModule::displayScreenPage(IDisplay *display, int position) {
  std::vector<TDrawFunction> lines;
  ServosModule *that = this;

  lines.push_back([that](IDisplay *display){
    display->blackOnWhite();
    displayPrintline(display, that->getName());
    display->whiteOnBlack();
  });

  lines.push_back([that](IDisplay *display){
    displayPrintline(display, "SERVO 1: ", that->servo1position);
  });

  for(int i=0; i<lines.size(); i++){
    lines.at(i)(display);
  }
}