#ifndef TOUCHPAD_MODULE
#define TOUCHPAD_MODULE

#include "Arduino.h" 
#include "BaseModule.h"
#include "Observer.h"
#include "Common.h"

#define TOUCH_BUTTON_COUNT 6 // NOTE: Max 6 buttons
const int TOUCH_BUTTON_PINS[] = { T6, T8, T10, T13, T14 };
const int TOUCH_BUTTON_THRESHOLDS[] = { 30000, 30000, 30000, 30000, 30000, 30000 };

typedef void (*callbackPtr)();

class TouchpadModule: public IBaseModule {
public:
  bool verbose = false;
  const char* getName() {return "Touchpad";}
  IScreenPage* getDisplayPage() {return NULL;}

	TouchpadModule();
	void setup();
	void loop();

  void toggleThresholdDebugging();

  static void onTouchInterrupt(int button);
  Subject<int> buttonPressed;
  Subject<int> buttonReleased;
private:
  static bool volatile touchesDetected[TOUCH_BUTTON_COUNT];
  bool enableThresholdDump = false;
};

#endif