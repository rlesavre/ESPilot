#ifndef LOGGER_H
#define LOGGER_H

#include "BaseModule.h"
#include "Common.h"

using std::runtime_error;

enum LogLevel{
  debug, info, warn, error, fatal
};

class ILogger {
   public:

    virtual void log(LogLevel level, char* message);
    void debug(char* message) { log(LogLevel::debug, message); }
    void info(char* message) { log(LogLevel::info, message); }
    void warn(char* message) { log(LogLevel::warn, message); }
    void error(char* message) { log(LogLevel::error, message); }
    void fatal(char* message) { log(LogLevel::fatal, message); }
};

class LoggerModule : public IBaseModule, public IScreenPage, public ILogger
{
private:
  static LoggerModule* _instance;
  bool halted = false;
public:

  static LoggerModule *GetInstance(){
    if(_instance==NULL){
      _instance = new LoggerModule();
    }
    return _instance;
  }
  const char *getName() { return "Logger"; }
  IScreenPage *getDisplayPage() { return this; }
  void setup();
  void loop();
  void displayScreenPage(IDisplay *display, int position);
  void log(LogLevel level, char* message) override;

  ILogger* instance(){
    return _instance;    
  }

  bool isHalted(){
    return halted;
  }
};
#endif