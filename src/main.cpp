#include "BaseModule.h"
#include "Gps.h"
#include "Touchpad.h"
#include "Screen.h"
#include "ActivityLed.h"
#include "Motion.h"
#include "Statistics.h"
#include "Common.h"
#include "Magnetic.h"
#include "Storage.h"
#include "GeneralConfiguration.h"
#include "Servos.h"
#include "Logger.h"
#include "ConsoleCommand.h"
#include "Console.h"

#define SERIAL_SETUP_SPEED 115200
#define SERIAL_SETUP_GRACEDELAY 2000
#define GPS_LOGFILE "/gpsLog.csv"

std::vector<IBaseModule*> modules;
TouchpadModule *touchpadModule;
StatisticsModule *statisticsModule;
ScreenModule *screenModule;
MagneticModule *magneticModule;

void setup()
{
  Serial.setDebugOutput(true);
  Serial.begin(SERIAL_SETUP_SPEED);
  delay(SERIAL_SETUP_GRACEDELAY);

  if(!EspNowAgent::Instance()->initialize()){
    Serial.println("Failed to setup EspNowAgent");
    delay(500);
    esp_restart();
  }

  Serial.println(">> Building modules");

  auto storage = new StorageModule();
  modules.push_back(storage);

  auto console = new ConsoleModule();
  modules.push_back(console);

  modules.push_back(LoggerModule::GetInstance());
  LoggerModule::GetInstance()->setLogfileOPener(storage->getFileOpener("/log.txt"));
 
  statisticsModule = new StatisticsModule();
  modules.push_back(statisticsModule);

  modules.push_back(new ActivityLedModule());
  touchpadModule = new TouchpadModule();
  modules.push_back(touchpadModule);
  screenModule = new ScreenModule(&touchpadModule->buttonPressed);
  modules.push_back(screenModule);

  auto gpsModule = new GpsModule(storage->getFileOpener(GPS_LOGFILE));
  modules.push_back(gpsModule);
  modules.push_back(new MotionModule());
  
  // magneticModule = new MagneticModule();
  // modules.push_back(magneticModule);
  
  modules.push_back(new GeneralConfigurationModule());
  modules.push_back(new ServosModule());

  //serialPrintLine(">> Setup to be done on module count: ", modules.size());
  logf(">> Setup to be done on module count: %i", modules.size());
  int moduleNumber = 0;
  for(IBaseModule* module : modules){
    //serialPrintLine(">>> Setup started: ", module->getName(), " #", moduleNumber);
    logf(">>> Setup started for %s (%i)", module->getName(), moduleNumber);
    module->setup();
    screenModule->addPage(module->getDisplayPage());
    //serialPrintLine("<<< Setup Done: ", module->getName(), " #", moduleNumber++);
    logf("<<< Setup ended for %s (%i)", module->getName(), moduleNumber++);
  }
  //Serial.println("<< Modules built and setuped");
  logf("<< Modules built and setuped");

  //Serial.println(">> Registering commands:");
  logf(">> Registering commands");
  console->registerCommand(new ConsoleCommand("reset", [](const char* arg){esp_restart();return 1;}));
  console->registerCommand(new ConsoleCommand("up", [](const char* arg){screenModule->moveUp();return 1;}));
  console->registerCommand(new ConsoleCommand("down", [](const char* arg){screenModule->moveDown();return 1;}));
  console->registerCommand(new ConsoleCommand("ok", [](const char* arg){screenModule->nextPage();return 1;}));
  console->registerCommand(new ConsoleCommand("sdrestart", [storage](const char* arg){return storage->mount()?1:0;}));
  console->registerCommand(new ConsoleCommand("cleangpslog", [storage](const char* arg){storage->deleteFile(GPS_LOGFILE); return 1;}));
  console->registerCommand(new ConsoleCommand("outputgps", [gpsModule](const char* arg){gpsModule->logGpsData = !gpsModule->logGpsData; return 1;}));
  //Serial.println("<< Command registered");
  logf("<< Commands registered");
  espInfos();
}

void loop()
{
  for(IBaseModule* module : modules){
    statisticsModule->measureLoop(module);
  }
}