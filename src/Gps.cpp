#include "Gps.h"
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include "Screen.h"

GpsModule::GpsModule(int rxPin, int txPin) {
  lastGpsFix = -1;
  longitude = -1;
  latitude = -1;
  course = -1;
  altitude = -1;
  speed_mps = -1;
  this->rxPin = rxPin;
  this->txPin = txPin;
}

void GpsModule::setup(){
  gpsSerial.begin(GPSBAUD, SWSERIAL_8N1, rxPin, txPin, false);
}

void GpsModule::loop(){
  while(gpsSerial.available())
  {
    char c = gpsSerial.read();
    if(logging){
      Serial.print(c);
    }    
    if(gps.encode(c))
    {
      lastGpsFix = millis();
      getgps(gps);         // then grab the data.
    }
  }
}

int GpsModule::delayFix(){
  return millis() - lastGpsFix;
}

void GpsModule::getgps(TinyGPS &gps)
{
  gps.f_get_position(&latitude, &longitude);
  altitude = gps.f_altitude();
  course = gps.f_course();
  speed_mps = gps.f_speed_mps();

  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths);

  if(!outputedDate){
    outputedDate = true;
    Serial.print("GPS Date: "); Serial.print(month, DEC); Serial.print("/"); 
    Serial.print(day, DEC); Serial.print("/"); Serial.print(year);
    Serial.print("  Time: "); Serial.print(hour, DEC); Serial.print(":"); 
    Serial.print(minute, DEC); Serial.print(":"); Serial.print(second, DEC); 
    Serial.print("."); Serial.println(hundredths, DEC);
  }

  // Serial.print("Lat/Long: "); 
  // Serial.print(latitude,5); 
  // Serial.print(", "); 
  // Serial.println(longitude,5);
  
  // Serial.print("Date: "); Serial.print(month, DEC); Serial.print("/"); 
  // Serial.print(day, DEC); Serial.print("/"); Serial.print(year);
  // Serial.print("  Time: "); Serial.print(hour, DEC); Serial.print(":"); 
  // Serial.print(minute, DEC); Serial.print(":"); Serial.print(second, DEC); 
  // Serial.print("."); Serial.println(hundredths, DEC);

  // Serial.print("Altitude/Heading: "); Serial.print(altitude);  Serial.print("m, ");
  // Serial.print(course); Serial.println(" ");
  // Serial.print("Speed(kmph): "); Serial.print(speed_mps); Serial.println("m/s");

  // Serial.println();
  
  // // Here you can print statistics on the sentences.
  // unsigned long chars;
  // unsigned short sentences, failed_checksum;
  // gps.stats(&chars, &sentences, &failed_checksum);
}

typedef std::function<void()> TLineFunction;

void GpsModule::displayScreenPage(Adafruit_SSD1306 *display, int position){
  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display->setCursor(0, 0);

  display->println("GPS MODULE");
  display->setTextColor(WHITE);

  std::vector<TLineFunction> lines;
  GpsModule *that = this;

  lines.push_back([&](){
    display->print("Last fix:"); 
    display->print(that->delayFix()); 
    display->println(" ms"); 
  });
  lines.push_back([&](){
    display->print("Lat: "); 
    display->println(that->latitude, 8); 
  });
  lines.push_back([&](){
    display->print("Long: "); 
    display->println(that->longitude, 8);
  });
  lines.push_back([&](){
    display->print("Altitude: "); 
    display->print(that->altitude); 
    display->println(" m"); 
  });
  lines.push_back([&](){
    display->print("Speed: "); 
    display->print(that->speed_mps); 
    display->println(" m/s"); 
  });
  lines.push_back([&](){
    display->print("Course: "); 
    display->println(that->course); 
  }); 

  for(int i=0; i<3; i++){
    lines.at(i + (position * 3) % lines.size())();
  }
  display->display();
}

