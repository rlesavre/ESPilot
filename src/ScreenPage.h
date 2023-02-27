#ifndef SCREEN_PAGE
#define SCREEN_PAGE

#include <Adafruit_SSD1306.h>
// #include "fonts/Org_01.h" "Small" 7x5 (not small enough)

typedef std::function<void(Adafruit_SSD1306 *display)> TDrawFunction;

class IScreenPage {
   public:
   // Should we unify the drawing of line of text ?
    virtual void displayScreenPage(Adafruit_SSD1306 *display, int position) = 0;
};

#endif