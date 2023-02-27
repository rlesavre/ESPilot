#ifndef MAGNETIC_H
#define MAGNETIC_H

#include "AS5600.h"
#include "Wire.h"

#include "Arduino.h"
#include "BaseModule.h"
#include "Common.h"

class MagneticModule : public IBaseModule, public IScreenPage
{
private:
  AS5600 *as5600;   //  use default Wire
  uint32_t lastLoopMs = 0;
  uint16_t rawAngle = 0;
  int32_t position = 0;
  float angularSpeed = 0;

public:
  MagneticModule(TwoWire *twi = &Wire) {
    as5600 = new AS5600(twi);
  }
  const char *getName() { return "Magnetic"; }
  IScreenPage *getDisplayPage() { return this; } //this; }
  void setup() {
    as5600->begin(255);  //  set direction pin.
    serialPrintLine("Magnetic started, and connected: ", as5600->isConnected(), " magnet too weak: ", as5600->magnetTooWeak(), " magnet too strong: ", as5600->magnetTooStrong());
  }

  int loopCount = 0;

  void loop()
  {  
    if (millis() - lastLoopMs >= 100)
    {
      //serialPrintLine("reading values...");
      lastLoopMs = millis();

      rawAngle = as5600->rawAngle();
      angularSpeed = as5600->getAngularSpeed();
      position = as5600->getCumulativePosition();
      //serialPrintLine(" - values: rawAngle=", rawAngle, " angularSpeed=", angularSpeed, " position=", position, " loop count=", loopCount++);
    }
  }

  void displayScreenPage(Adafruit_SSD1306 *display, int position)
  {
    display->clearDisplay();
    display->setTextSize(1);

    std::vector<TDrawFunction> lines;
    MagneticModule *that = this;

    display->setCursor(0, 0);
    lines.push_back([that](Adafruit_SSD1306 *display){
      display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display->println(that->getName());
      display->setTextColor(WHITE); 
    });

    lines.push_back([that](Adafruit_SSD1306 *display){
      display->print("Angle: ");
      display->println(that->rawAngle * AS5600_RAW_TO_DEGREES);
    });

    lines.push_back([that](Adafruit_SSD1306 *display){
      display->print("Speed: ");
      display->println(that->angularSpeed);
    });

    lines.push_back([that](Adafruit_SSD1306 *display){
      display->print("Position: ");
      display->println(that->position);
    });

    int startingPosition = position % (lines.size() - 4 + 1);
    //serialPrintLine("position", position, "size", lines.size(), "startingPosition", startingPosition);

    for (int i = 0; i < 4 && i <= lines.size() - startingPosition; i++)
    {
      lines.at(i + startingPosition)(display);
    }

    display->display();
  }
};
#endif