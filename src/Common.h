#ifndef COMMON_H
#define COMMON_H
#include "Arduino.h"

// To handle base case of below recursive
// Variadic function Template
inline void serialPrintLine()
{
  Serial.println("");
}
 
// Variadic function Template that takes
// variable number of arguments and prints
// all of them in one line (spaced by " ")
template <typename T, typename... Types> 
inline void serialPrintLine(T var1, Types... var2)
{
  Serial.print(var1);
  serialPrintLine(var2...);
}

// To handle base case of below recursive
// Variadic function Template
inline void serialPrint()
{
  Serial.print("");
}
 
// Variadic function Template that takes
// variable number of arguments and prints
// all of them in one line (spaced by " ")
template <typename T, typename... Types> 
inline void serialPrint(T var1, Types... var2)
{
  Serial.print(var1);
  serialPrint(var2...);
}


inline static const char *getResetReasonStr()
{
    switch (esp_reset_reason())
    {
    case ESP_RST_BROWNOUT:
        return "Brownout reset (software or hardware)";
    case ESP_RST_DEEPSLEEP:
        return "Reset after exiting deep sleep mode";
    case ESP_RST_EXT:
        return "Reset by external pin (not applicable for ESP32)";
    case ESP_RST_INT_WDT:
        return "Reset (software or hardware) due to interrupt watchdog";
    case ESP_RST_PANIC:
        return "Software reset due to exception/panic";
    case ESP_RST_POWERON:
        return "Reset due to power-on event";
    case ESP_RST_SDIO:
        return "Reset over SDIO";
    case ESP_RST_SW:
        return "Software reset via esp_restart";
    case ESP_RST_TASK_WDT:
        return "Reset due to task watchdog";
    case ESP_RST_WDT:
        return "ESP_RST_WDT";

    case ESP_RST_UNKNOWN:
    default:
        return "Unknown";
    }
}

inline static const char *getFlashModeStr()
{
    switch (ESP.getFlashChipMode())
    {
    case FM_DIO:
        return "DIO";
    case FM_DOUT:
        return "DOUT";
    case FM_FAST_READ:
        return "FAST READ";
    case FM_QIO:
        return "QIO";
    case FM_QOUT:
        return "QOUT";
    case FM_SLOW_READ:
        return "SLOW READ";
    case FM_UNKNOWN:
    default:
        return "UNKNOWN";
    }
}

inline void espInfos(){
  esp_chip_info_t info;
  esp_chip_info(&info);
  Serial.printf("ESP-IDF Version: %s\n", ESP.getSdkVersion());
  Serial.printf("Chip info:\n");
  Serial.printf("\tModel: %s\n", ESP.getChipModel());
  Serial.printf("\tRevison number: %d\n", ESP.getChipRevision());
  Serial.printf("\tCores: %d\n", ESP.getChipCores());
  Serial.printf("\tClock: %d MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("\tFeatures:%s%s%s%s%s\r\n",
          info.features & CHIP_FEATURE_WIFI_BGN ? " 802.11bgn " : "",
          info.features & CHIP_FEATURE_BLE ? " BLE " : "",
          info.features & CHIP_FEATURE_BT ? " BT " : "",
          info.features & CHIP_FEATURE_EMB_FLASH ? " Embedded-Flash " : " External-Flash ",
          info.features & CHIP_FEATURE_EMB_PSRAM ? " Embedded-PSRAM" : "");
  Serial.printf("Flash size: %d MB (mode: %s, speed: %d MHz)\n", ESP.getFlashChipSize() / (1024 * 1024), getFlashModeStr(), ESP.getFlashChipSpeed() / (1024 * 1024));
  Serial.printf("PSRAM size: %d MB\n", ESP.getPsramSize() / (1024 * 1024));
  Serial.printf("Sketch size: %d KB\n", ESP.getSketchSize() / (1024));
  Serial.printf("Sketch MD5: %s\n", ESP.getSketchMD5().c_str());
  Serial.printf("Reset reason: %s\r\n", getResetReasonStr());
}


#endif