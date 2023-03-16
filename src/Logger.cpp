#include "logger.h"
LoggerModule* LoggerModule::_instance = NULL;

void LoggerModule::setup() {
}

void LoggerModule::loop()
{  
}

void LoggerModule::log(LogLevel level, char* message){
  int noop = 0;
}

void LoggerModule::displayScreenPage(IDisplay *display, int position)
{
  display->blackOnWhite();
  displayPrintline(display, this->getName());
}
