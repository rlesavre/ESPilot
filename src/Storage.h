#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Arduino.h"
#include "ArduinoJson.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "BaseModule.h"
#include "Common.h"


class StorageModule : public IBaseModule, public IScreenPage
{
private:
  uint8_t cardType = CARD_NONE;
  uint64_t cardSize = 0;

public:
  StorageModule() {}
  const char *getName() { return "Storage"; }
  IScreenPage *getDisplayPage() { return this; }

  const char* getCard(){
    switch(cardType){
      case CARD_NONE:
        return "none";
      case CARD_MMC:
        return "MMC";        
      case CARD_SD:
        return "SD";        
      case CARD_SDHC:
        return "SDHC";
      default:
        return "Unknown";        
    }
  }

  void setup() {
    if(!SD.begin()){
      Serial.println("Card Mount Failed");
      return;
    }
    cardType = SD.cardType();
    if(cardType == CARD_NONE){
        Serial.println("Error: No SD card attached");
        delay(10000);
        ESP.restart();
        return;
    }
    cardSize = SD.cardSize() / (1024 * 1024);
    serialPrintLine("Storage found, card type: ", getCard(), ", card size: ", cardSize, "MB");
  }
  
  void loop() {}

  void displayScreenPage(Adafruit_SSD1306 *display, int position)
  {
    display->clearDisplay();
    display->setTextSize(1);
    display->setCursor(0, 0);

    display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display->println(this->getName());
    display->setTextColor(WHITE); 

    display->print("Card type: ");
    display->println(getCard());

    display->print("Card size: ");
    display->print(cardSize);
    display->println(" MB");

    display->display();
  }
};
#endif