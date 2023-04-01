#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Arduino.h"
#include "ArduinoJson.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "BaseModule.h"
#include "Common.h"

typedef std::function<File(void)> TOpenFile;

class StorageModule : public IBaseModule, public IScreenPage
{
private:
  uint8_t cardType = CARD_NONE;
  uint64_t cardSize = 0;
  unsigned long totalBytes = 0;
  unsigned long usedBytes = 0;

public:
  StorageModule() {
  }
  const char *getName() { return "Storage"; }
  IScreenPage *getDisplayPage() { return this; }

  const char *getCard()
  {
    switch (cardType)
    {
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

  void setup()
  {
    if(!mount()){
      delay(5000);
      ESP.restart();
    }
  }

  bool mount(){
    SD.end();
    delay(1);
    if (!SD.begin())
    {
      Serial.println("Card Mount Failed");
      return false;
    }

    cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
      Serial.println("Error: No SD card attached");     
      return false;
    }
    cardSize = SD.cardSize() / (1024 * 1024);
    serialPrintLine("Storage found, card type: ", getCard(), ", card size: ", cardSize, "MB");
    return true;
  }

  void readFile(fs::FS &fs, const char *path)
  {
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file)
    {
      Serial.println("Failed to open file for reading");
      return;
    }

    Serial.print("Read from file: ");
    while (file.available())
    {
      Serial.write(file.read());
    }
    Serial.println();
    file.close();
  }

  File openFile(const char *filename)
  {
    if(SD.exists(filename)){
      return SD.open(filename, FILE_APPEND);
    }
    return SD.open(filename, FILE_WRITE);
  }

  TOpenFile getFileOpener(const char *filename)
  {
    auto that = this;
    return [that, filename]()
    { return that->openFile(filename); };
  }

  void deleteFile(const char *filename){
    if(SD.exists(filename)){
      SD.remove(filename);
    }
  }

  void loop() {}

  void displayScreenPage(IDisplay *display, int position)
  {
    display->blackOnWhite();
    displayPrintline(display, this->getName());

    display->whiteOnBlack();

    cardSize = SD.cardSize() / (1024 * 1024);
    totalBytes = SD.totalBytes() / (1024 * 1024);
    usedBytes = SD.usedBytes() / (1024 * 1024);

    displayPrintline(display, "Card type: ", getCard());
    displayPrintline(display, "Card size: ", (unsigned long)cardSize, " MB");

    // if (position != 0)
    // {
    //   serialPrintLine("Storage found, card type: ", getCard(), ", card size: ", cardSize, "MB, used: ", usedBytes, "MB ");
    //   readFile(SD, "/gpsLog.csv");
    // }
  }
};
#endif