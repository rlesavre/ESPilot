#ifndef MOTION_MODULE
#define MOTION_MODULE
#include <vector>
#include "Arduino.h" 
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include "BaseModule.h"
#include <Wire.h>

#define BNO055_ADDRESS 0x28

typedef struct {
  float x = -1;
  float y = -1;
  float z = -1;
} vec_t;

class MotionModule: public IScreenPage, public IBaseModule {
public:
  MotionModule(TwoWire *twi = &Wire);
  void setup();
  void loop();

  const char* getName() {return "Motion";}
  IScreenPage* getDisplayPage() {return this;}

  void displayScreenPage(Adafruit_SSD1306 *display, int position);
private:
  void readEvent(sensors_event_t* event);
  void addVectorDisplayLines(std::vector<TDrawFunction> *lines, Adafruit_SSD1306 *display, const char *name, vec_t &vector);

  int lastMotionRead = -1;
  Adafruit_BNO055 *bno;
  vec_t orientation;
  vec_t acceleration;
  vec_t linear_acceleration;
  vec_t magnetic;
  vec_t gyroscope;
  vec_t rotation;
  vec_t gravity;

  int8_t boardTemp = -1;
  uint8_t system, gyro, accel, mag = 0;
};
#endif