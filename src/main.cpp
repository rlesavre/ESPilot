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

#define SERIAL_SETUP_GRACEDELAY 2000

std::vector<IBaseModule*> modules;
TouchpadModule *touchpadModule;
StatisticsModule *statisticsModule;
ScreenModule *screenModule;
MagneticModule *magneticModule;
void setup()
{
  Serial.begin(115200);
  delay(SERIAL_SETUP_GRACEDELAY);
  Serial.println(">> Building modules");

  statisticsModule = new StatisticsModule();
  modules.push_back(statisticsModule);

  modules.push_back(new ActivityLedModule());
  touchpadModule = new TouchpadModule();
  modules.push_back(touchpadModule);
  screenModule = new ScreenModule(&touchpadModule->buttonPressed);
  modules.push_back(screenModule);
  modules.push_back(new GpsModule());
  modules.push_back(new MotionModule());
  
  magneticModule = new MagneticModule();
  modules.push_back(magneticModule);
  modules.push_back(new StorageModule());
  modules.push_back(new GeneralConfigurationModule());
  modules.push_back(new ServosModule());

  serialPrintLine(">> Setup to be done on module count: ", modules.size());
  int moduleNumber = 0;
  for(IBaseModule* module : modules){
    serialPrintLine(">>> Setup started: ", module->getName(), " #", moduleNumber);
    module->setup();
    screenModule->addPage(module->getDisplayPage());
    serialPrintLine("<<< Setup Done: ", module->getName(), " #", moduleNumber++);
  }
  Serial.println("<< Modules built and setuped");
}

void loop()
{
  for(IBaseModule* module : modules){
    statisticsModule->measureLoop(module);
  }
}
