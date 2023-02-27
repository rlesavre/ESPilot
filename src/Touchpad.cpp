#include "Touchpad.h"

/**
 * Can't find a way to give a class member as interrupt function pointer :(
*/
void gotTouch0(){ TouchpadModule::onTouchInterrupt(0); }
void gotTouch1(){ TouchpadModule::onTouchInterrupt(1); }
void gotTouch2(){ TouchpadModule::onTouchInterrupt(2); }
void gotTouch3(){ TouchpadModule::onTouchInterrupt(3); }
void gotTouch4(){ TouchpadModule::onTouchInterrupt(4); }
void gotTouch5(){ TouchpadModule::onTouchInterrupt(5); }
callbackPtr getCallback(int button){
  switch(button){
    case 0: return gotTouch0;
    case 1: return gotTouch1;
    case 2: return gotTouch2;
    case 3: return gotTouch3;
    case 4: return gotTouch4;
    case 5: return gotTouch5;
    default: return NULL;
  }
}

bool volatile TouchpadModule::touchesDetected[TOUCH_BUTTON_COUNT] = { false, false, false, false, false, false };

TouchpadModule::TouchpadModule() {
}

void TouchpadModule::setup(){
  for (int i = 0; i < TOUCH_BUTTON_COUNT; i++) {
    touchAttachInterrupt(TOUCH_BUTTON_PINS[i], getCallback(i), TOUCH_BUTTON_THRESHOLDS[i]); 
  }
}

void TouchpadModule::loop(){
  for (int i = 0; i < TOUCH_BUTTON_COUNT; i++) {
    if(enableThresholdDump){
       serialPrintLine("TOUCH_PIN_", TOUCH_BUTTON_PINS[i],":",touchRead(TOUCH_BUTTON_PINS[i]), 
         ",THRESHOLD_PIN_", TOUCH_BUTTON_PINS[i], ":", TOUCH_BUTTON_THRESHOLDS[i]);
    }

    if(touchesDetected[i]){
      bool pressed = true;
      touchesDetected[i] = false;
      if (touchInterruptGetLastStatus(TOUCH_BUTTON_PINS[i])) {
        buttonPressed.notifyObservers(i);
      } else {
        buttonReleased.notifyObservers(i);
        pressed = false;
      }
      serialPrintLine("Touch event on ", TOUCH_BUTTON_PINS[i], pressed ? " [pressed]":" [released]");
    }
  }
}

void TouchpadModule::onTouchInterrupt(int button){
  touchesDetected[button] = true;
}

void TouchpadModule::toggleThresholdDebugging(){
  enableThresholdDump = !enableThresholdDump;
}


