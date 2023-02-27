#ifndef GENERALCONFIGURATION_H
#define GENERALCONFIGURATION_H

#include "Arduino.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "BaseModule.h"
#include "Common.h"

struct Config {
  char hostname[64];
  bool logTouchThresholds = false;
  bool logGps = false;
  int loopFrequencyForLed = 1000;
};

class GeneralConfigurationModule : public IBaseModule, public IScreenPage
{
private:
  Config config;
  const char *filename = "/config.json";  // <- SD library uses 8.3 filenames

  // Prints the content of a file to the Serial
  void printFile(const char *filename) {
    // Open file for reading
    File file = SD.open(filename);
    if (!file) {
      Serial.println(F("Failed to read file"));
      return;
    }

    // Extract each characters by one by one
    while (file.available()) {
      Serial.print((char)file.read());
    }
    Serial.println();

    // Close the file
    file.close();
  }

  // Loads the configuration from a file
  void loadConfiguration(const char *filename, Config &config) {
    // Open file for reading
    File file = SD.open(filename);

    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use https://arduinojson.org/v6/assistant to compute the capacity.
    StaticJsonDocument<512> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
      Serial.println(F("Failed to read file, using default configuration"));

    // Copy values from the JsonDocument to the Config
    config.loopFrequencyForLed = doc["loopFrequencyForLed"] | 1000;
    config.logTouchThresholds = doc["logTouchThresholds"] | false;
    config.logGps = doc["logGps"] | false;

    strlcpy(config.hostname,                  // <- destination
            doc["hostname"] | "example.com",  // <- source
            sizeof(config.hostname));         // <- destination's capacity

    // Close the file (Curiously, File's destructor doesn't close the file)
    file.close();
  }

  // Saves the configuration to a file
  void saveConfiguration(const char *filename, const Config &config) {
    // Delete existing file, otherwise the configuration is appended to the file
    SD.remove(filename);

    // Open file for writing
    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
      Serial.println(F("Failed to create file"));
      return;
    }

    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use https://arduinojson.org/assistant to compute the capacity.
    StaticJsonDocument<256> doc;

    // Set the values in the document
    doc["hostname"] = config.hostname;
    doc["loopFrequencyForLed"] = config.loopFrequencyForLed;
    doc["logTouchThresholds"] = config.logTouchThresholds;
    doc["logGps"] = config.logGps;

    // Serialize JSON to file
    if (serializeJson(doc, file) == 0) {
      Serial.println(F("Failed to write to file"));
    }

    // Close the file
    file.close();
  }

public:
  GeneralConfigurationModule() {}
  const char *getName() { return "Configuration"; }
  IScreenPage *getDisplayPage() { return this; }


  void setup() {  // Should load default config if run for the first time
    Serial.println(F("Loading configuration..."));
    loadConfiguration(filename, config);

    Serial.println(F("Saving configuration..."));
    saveConfiguration(filename, config);

    // Dump config file
    Serial.println(F("Print config file..."));
    printFile(filename);
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

    display->print("GPS logging: ");
    display->println(config.logGps);

    display->print("Touch logging: ");
    display->println(config.logTouchThresholds);

    display->print("LED loop/s: ");
    display->println(config.loopFrequencyForLed);

    display->display();
  }
};
#endif