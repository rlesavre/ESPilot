#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
#include <map>

#include "Arduino.h"
#include "BaseModule.h"
#include "Common.h"

#define LOOPCOUNT 1000

typedef struct
{
  std::vector<int> lastLoopTimes;
  float meanLoopTimes;
} statistic_t;

typedef std::map<IBaseModule *, statistic_t*> statMap_t;

class StatisticsModule : public IBaseModule, public IScreenPage
{
private:
  int loopCount = 0;
  unsigned long uptimeMs = 0;
  unsigned long lastLoopMicros = 0;
  float loopPerSeconds = 0;
  statMap_t mapOfStatistics;

  void addModule(IBaseModule *module)
  {
    if (mapOfStatistics.find(module) != mapOfStatistics.end())
    {
      serialPrintLine("Module already inserted:", module->getName());
      return;
    }
    //serialPrintLine("Inserting module: ", module->getName());
    mapOfStatistics[module] = new statistic_t;   
  }

public:
  StatisticsModule() {}
  const char *getName() { return "STATISTICS"; }
  IScreenPage *getDisplayPage() { return this; }
  void setup() { uptimeMs = millis();}
  void loop()
  {
    if (loopCount++ < LOOPCOUNT) // Do not overcompute
      return;

    for(statMap_t::iterator it=mapOfStatistics.begin(); it!=mapOfStatistics.end(); ++it){
      IBaseModule* module = it->first;
      statistic_t* stat = it->second;

      int measureCount = stat->lastLoopTimes.size();     
      if (measureCount > LOOPCOUNT)
      {
        stat->lastLoopTimes.erase(stat->lastLoopTimes.begin(), stat->lastLoopTimes.begin() + (measureCount - LOOPCOUNT));
      }

      unsigned long sum = 0;
      for (int measure : stat->lastLoopTimes)
      {
        sum += measure;
      }
      stat->meanLoopTimes = (float)sum / (float)(stat->lastLoopTimes.size());
    }

    // Compute loop per seconds
    int delayBetweenLoop = millis() - lastLoopMicros;
    loopPerSeconds = ((float)1000 / (float)delayBetweenLoop) * loopCount;
    lastLoopMicros = millis();
    loopCount = 0;
  }

  void measureLoop(IBaseModule *module)
  {
    if (mapOfStatistics.find(module) == mapOfStatistics.end())
    {
      //serialPrintLine("Module not found: ", module->getName());
      addModule(module);
    }

    unsigned long start = micros();
    module->loop();
    int loopTime = micros() - start;
    mapOfStatistics[module]->lastLoopTimes.push_back(loopTime);
  }

  void displayScreenPage(IDisplay *display, int position)
  {
    std::vector<TDrawFunction> lines;
    StatisticsModule *that = this;

    lines.push_back([that](IDisplay *display){
      display->blackOnWhite();
      displayPrintline(display, that->getName());
      display->whiteOnBlack();
    });

    lines.push_back([that](IDisplay *display){
      uint32_t uptime = (millis() - that->uptimeMs);
      if(uptime < 30000){
        displayPrintline(display, "Uptime: ", uptime, " ms");
      } else if(uptime >= 30000){
        displayPrintline(display, "Uptime: ", (float) uptime / (float)1000U, " s");
      }
    });

    lines.push_back([that](IDisplay *display){
      displayPrintline(display, "Loop/s: ", that->loopPerSeconds);
    });

   for(statMap_t::iterator it=mapOfStatistics.begin(); it!=mapOfStatistics.end(); ++it){
      IBaseModule *module = it->first;
      statistic_t* stat = it->second;
      lines.push_back([that, module, stat](IDisplay *display) {
        displayPrintline(display, module->getName());
      });
      lines.push_back([that, module, stat](IDisplay *display) {
        displayPrintline(display, " -> ", stat->meanLoopTimes, " ms");
      });
      //serialPrint("MEAN_", module->getName(), ":", stat->meanLoopTimes, ",\t");
    }
    //Serial.println("");

    int startingPosition = position % (lines.size() - 4 + 1);
    for (int i = 0; i < 4 && i <= lines.size() - startingPosition; i++)
    {
      lines.at(i + startingPosition)(display);
    }
  }
};
#endif