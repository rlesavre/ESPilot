#ifndef SCREEN_MODULE
#define SCREEN_MODULE
#include <vector>

#include "Arduino.h" 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "BaseModule.h"
#include "Observer.h"
#include "ScreenPage.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
#define SCREEN_BRIGHTNESS 64 // 0 - 255
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define DRAW_PERIOD 100

class ScreenModule : public Observer<int>, public IScreenPage, public IBaseModule, public IDisplay {
public:
  const char* getName() {return "Screen";}
  IScreenPage* getDisplayPage() {return this;}

  ScreenModule(Subject<int> *buttonPressed, TwoWire *twi = &Wire);
  void setup();
  void loop();

  void addPage(IScreenPage *page);
  void nextPage();
  void previousPage();
  void moveDown();
  void moveUp();

  void notify(int arg);

  void displayScreenPage(IDisplay *display, int position);

  void whiteOnBlack();
  void blackOnWhite();

  void print(const char* line);
  void print(char c);
  void print(unsigned char uc);
  void print(int i);
  void print(unsigned int ui);
  void print(long l);
  void print(unsigned long ul);
  void print(double d);   
  void print(bool b);
  void eol();

private:
  std::vector<IScreenPage*> pages;
  Adafruit_SSD1306 *display;
  int currentPage = 0;
  int currentPositionInPage = 0;
  uint32_t lastDraw = 0;
};
#endif