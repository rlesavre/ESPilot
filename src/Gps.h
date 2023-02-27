#ifndef GPS_MODULE
#define GPS_MODULE

#define GPSBAUD 9600
#define GPSRXPIN 17 // green gps
#define GPSTXPIN 21 // yellow

#include "Arduino.h" 
#include "BaseModule.h"

#include <SoftwareSerial.h>
#include <TinyGPS.h>

// https://rl.se/gprmc

class GpsModule : public IScreenPage, public IBaseModule {
public:
  const char* getName() {return "GPS";}
  IScreenPage* getDisplayPage() {return this;}
  GpsModule(int rxPin = GPSRXPIN, int txPin = GPSTXPIN);
	void setup();
	void loop();
  int delayFix();

  void displayScreenPage(Adafruit_SSD1306 *display, int position);

private:
  bool outputedDate = false;
  bool logging = false;
	float longitude;
	float latitude;
	float course;
	float altitude;
	float speed_mps;

	int year;
  byte month, day, hour, minute, second, hundredths;
	
  int rxPin;
  int txPin;
  int lastGpsFix;
	TinyGPS gps;
	SoftwareSerial gpsSerial;
  void getgps(TinyGPS &gps);
};

#endif