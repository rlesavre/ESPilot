#ifndef GPS_MODULE
#define GPS_MODULE

#define GPSBAUD 9600
#define GPSRXPIN 17 // green gps
#define GPSTXPIN 21 // yellow

#define MAX_SATELLITES 40

#include "Arduino.h" 
#include "BaseModule.h"

#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

#include "Screen.h"
#include "Logger.h"

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
  int satellites;
  //The effect of the DOP on the horizontal position value. The more good visible satellites low in the sky, the better the HDOP and the horizontal position (Latitude and Longitude) are.
  float hdop;

	int year;
  byte month, day, hour, minute, second, hundredths;
	
  int rxPin;
  int txPin;
  int lastGpsFix;

	SoftwareSerial gpsSerial;
  TinyGPSPlus gpsPlus;

  boolean gpsStatus[7] = {false, false, false, false, false, false, false};
  unsigned long start;

  void configureUblox(byte *settingsArrayPointer);
  void calcChecksum(byte *checksumPayload, byte payloadSize);
  void sendUBX(byte *UBXmsg, byte msgLength);
  byte getUBX_ACK(byte *msgID);
  void printHex(uint8_t *data, uint8_t length);
  void setBaud(byte baudSetting);
};

#endif