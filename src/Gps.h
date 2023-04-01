#ifndef GPS_MODULE
#define GPS_MODULE

#define GPSBAUD 9600
#define GPSRXPIN 17 // green gps
#define GPSTXPIN 21 // yellow

#include "Arduino.h" 
#include "BaseModule.h"

#include <SoftwareSerial.h>
#include <TinyGPS.h>

#include "Storage.h"

// https://rl.se/gprmc

class GpsModule : public IScreenPage, public IBaseModule {
public:
  const char* getName() {return "GPS";}
  IScreenPage* getDisplayPage() {return this;}
  GpsModule(TOpenFile getFile, int rxPin = GPSRXPIN, int txPin = GPSTXPIN);
	void setup();
	void loop();
  int delayFix();

  void displayScreenPage(IDisplay *display, int position);
  bool logGpsData = false;
private:
  TOpenFile getGpsLogFile;
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