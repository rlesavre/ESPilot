#ifndef LOGGER_H
#define LOGGER_H

#include "BaseModule.h"
#include "Common.h"
#include "Storage.h"
#include "EspNowAgent.h"
#include "functional"

using std::runtime_error;
int logf(const char *format, ...);

class LoggerModule : public IBaseModule, public IScreenPage
{
private:
  static LoggerModule* _instance;
  bool halted = false;
  TOpenFile getLogfile;
  File logfile;
  bool setuped = false;

public:
  static LoggerModule *GetInstance(){
    if(_instance==NULL){
      _instance = new LoggerModule();
    }
    return _instance;
  }
  const char *getName() { return "Logger"; }
  IScreenPage *getDisplayPage() { return this; }
  void setLogfileOPener(TOpenFile getFile);
  void setup();
  void loop();
  void displayScreenPage(IDisplay *display, int position);

  void log(const char text[], bool localOnly = false);
  void log(const uint8_t *data, size_t len, bool localOnly = false);

  bool isHalted(){
    return halted;
  }
};
#endif