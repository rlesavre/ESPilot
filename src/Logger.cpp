#include "logger.h"
LoggerModule *LoggerModule::_instance = NULL;

void LoggerModule::setup()
{
  setuped = true;
}

void LoggerModule::loop()
{
  // log last use of file, if older than X, close it
  EspNowAgent::Instance()->loop();
}

void LoggerModule::setLogfileOPener(TOpenFile getFile)
{
  getLogfile = getFile;
}

void LoggerModule::log(const char text[], bool localOnly)
{
  log((const uint8_t *)text, (size_t)strlen(text), localOnly);
}
void LoggerModule::log(const uint8_t *data, size_t len, bool localOnly)
{
  // Serial log
  Serial.write(data, (size_t)len);
  Serial.println();

  if(setuped) {
    // File log
    logfile = getLogfile();
    if(logfile){
      logfile.write(data, len);
      logfile.println();
      logfile.close();
    }

    // Wifi logging
    EspNowAgent::Instance()->sendDataAsync(data, len);
    delay(25);
  }
}

int logf(const char *format, ...)
{
  char loc_buf[64];
  char *temp = loc_buf;
  va_list arg;
  va_list copy;

  va_start(arg, format);
  va_copy(copy, arg);
  int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
  va_end(copy);
  if (len < 0)
  {
    va_end(arg);
    return 0;
  };
  if (len >= (int)sizeof(loc_buf))
  { // comparation of same sign type for the compiler
    temp = (char *)malloc(len + 1);
    if (temp == NULL)
    {
      va_end(arg);
      return 0;
    }
    len = vsnprintf(temp, len + 1, format, arg);
  }
  va_end(arg);

  LoggerModule::GetInstance()->log((uint8_t *)temp, (size_t)len);

  if (temp != loc_buf)
  {
    free(temp);
  }
  return len;
}

void LoggerModule::displayScreenPage(IDisplay *display, int position)
{
  display->blackOnWhite();
  displayPrintline(display, this->getName());
}
