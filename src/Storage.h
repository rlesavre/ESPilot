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

  void displayScreenPage(IDisplay *display, int position)
  {
    display->blackOnWhite();
    displayPrintline(display, this->getName());

    display->whiteOnBlack();
    displayPrintline(display, "Card type: ", getCard());
    displayPrintline(display, "Card size: ", (unsigned long)cardSize, " MB");
  }
};
#endif