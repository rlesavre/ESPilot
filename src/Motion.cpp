#include "Motion.h"

uint16_t BNO055_SAMPLERATE_DELAY_MS = 100;

MotionModule::MotionModule(TwoWire *twi) {
  bno = new Adafruit_BNO055(55, BNO055_ADDRESS, twi);
}

void MotionModule::setup() {
  if (!bno->begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1)
      ;
  }
}

void setVector(sensors_vec_t source, vec_t *dest){
  dest->x = source.x;
  dest->y = source.y;
  dest->z = source.z;
}

void MotionModule::readEvent(sensors_event_t *event)
{
  switch (event->type)
  {
  case SENSOR_TYPE_ACCELEROMETER:
    setVector(event->acceleration, &acceleration);
    break;
  case SENSOR_TYPE_ORIENTATION:
    setVector(event->orientation, &orientation);
    break;
  case SENSOR_TYPE_MAGNETIC_FIELD:
    setVector(event->magnetic, &magnetic);
    break;
  case SENSOR_TYPE_GYROSCOPE:
    setVector(event->gyro, &gyroscope);
    break;
  case SENSOR_TYPE_ROTATION_VECTOR:
    setVector(event->gyro, &rotation);
    break;
  case SENSOR_TYPE_LINEAR_ACCELERATION:
    setVector(event->acceleration, &linear_acceleration);
    break;
  case SENSOR_TYPE_GRAVITY:
    setVector(event->acceleration, &gravity);
    break;
  }
}

void MotionModule::loop() {
  int lastLoop = millis() - lastMotionRead;
  if (lastLoop > 0 && lastLoop < BNO055_SAMPLERATE_DELAY_MS){
    return;
  }
  lastMotionRead = millis();
  
  sensors_event_t orientationData, angVelocityData, linearAccelData, magnetometerData, accelerometerData, gravityData;
  bno->getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno->getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno->getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  bno->getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
  bno->getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno->getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);
  
  readEvent(&orientationData);
  readEvent(&angVelocityData);
  readEvent(&linearAccelData);
  readEvent(&magnetometerData);
  readEvent(&accelerometerData);
  readEvent(&gravityData);

  boardTemp = bno->getTemp();
  bno->getCalibration(&system, &gyro, &accel, &mag);
}



void MotionModule::addVectorDisplayLines(std::vector<TDrawFunction> *lines, IDisplay *display, const char *name, vec_t &vector)
{
  MotionModule *that = this;
  lines->push_back([that, name, vector](IDisplay *display){
    display->blackOnWhite();
    displayPrintline(display, name);
    display->whiteOnBlack();
  });

  lines->push_back([that, name, vector](IDisplay *display){
    displayPrintline(display, vector.x, " ", vector.y, " ", vector.z);  
  });
}

void MotionModule::displayScreenPage(IDisplay *display, int position) {

  std::vector<TDrawFunction> lines;
  MotionModule *that = this;

  lines.push_back([that](IDisplay *display){
    display->blackOnWhite();
    displayPrintline(display, that->getName());
    display->whiteOnBlack();
  });

  lines.push_back([that](IDisplay *display){
    displayPrintline(display, "Board temp: ", that->boardTemp); 
  });

  lines.push_back([that](IDisplay *display){
    displayPrintline(display, "Cal.Sys = ", that->system); 
  });

  lines.push_back([that](IDisplay *display){
    displayPrintline(display, "Cal.Gyro = ", that->gyro); 
  });

  lines.push_back([that](IDisplay *display){
    displayPrintline(display, "Cal.Acc = ", that->accel); 
  });

  lines.push_back([that](IDisplay *display){
    displayPrintline(display, "Cal.Mag = ", that->mag); 
  });
  addVectorDisplayLines(&lines, display, "orientation", orientation);
  addVectorDisplayLines(&lines, display, "acceleration", acceleration);
  addVectorDisplayLines(&lines, display, "linear_acceleration", linear_acceleration);
  addVectorDisplayLines(&lines, display, "magnetic", magnetic);
  addVectorDisplayLines(&lines, display, "gyroscope", gyroscope);
  addVectorDisplayLines(&lines, display, "rotation", rotation);
  addVectorDisplayLines(&lines, display, "gravity", gravity);
  
  int startingPosition = position % (lines.size()-4 + 1);
  for(int i=0; i<4 && i<=lines.size() - startingPosition; i++){
    lines.at(i + startingPosition)(display);
  }
}