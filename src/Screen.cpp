#include "Screen.h"
#include "Logger.h"

ScreenModule::ScreenModule(Subject<int> *buttonPressed, TwoWire *twi)
{
  buttonPressed->registerObserver(this);
  display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, twi, OLED_RESET);
}

void ScreenModule::setup()
{
  if (!display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    logf("Fatal error: SSD1306 allocation failed");
    delay(500);
    esp_restart();
  }
  display->ssd1306_command(SSD1306_SETCONTRAST);
  display->ssd1306_command(SCREEN_BRIGHTNESS);
}

void ScreenModule::loop()
{
  if (millis() - lastDraw >= DRAW_PERIOD)
  {
    lastDraw = millis();
    display->clearDisplay();
    display->setTextSize(1);
    display->setCursor(0, 0);
    this->pages.at(currentPage % pages.size())->displayScreenPage(this, this->currentPositionInPage);
    display->display();
  }
}

void ScreenModule::addPage(IScreenPage *page)
{
  if (page != NULL)
  {
    pages.push_back(page);
  }
}

void ScreenModule::nextPage()
{
  currentPage++;
  if (currentPage >= pages.size())
  {
    currentPage = 0;
  }
  currentPositionInPage = 0;
}

void ScreenModule::previousPage()
{
  currentPage--;
  if (currentPage < 0)
  {
    currentPage = pages.size() - 1;
  }
  currentPositionInPage = 0;
}

void ScreenModule::moveDown()
{
  currentPositionInPage++;
}

void ScreenModule::moveUp()
{
  currentPositionInPage--;
}

void ScreenModule::notify(int arg)
{
  switch (arg)
  {
  case 0:
    moveUp();
    break;
  case 1:
    moveDown();
    break;
  case 2:
    nextPage();
    break;
  }
}

void ScreenModule::displayScreenPage(IDisplay *display, int position)
{
  display->blackOnWhite();
  displayPrintline(display, this->getName());
  display->whiteOnBlack();

  displayPrintline(display, "Screen page count: ", (int)pages.size());
  displayPrintline(display, " - page: ", currentPage);
  displayPrintline(display, " - position: ", currentPositionInPage);
}

void ScreenModule::whiteOnBlack()
{
  display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
}
void ScreenModule::blackOnWhite()
{
  display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
}

void ScreenModule::print(const char *line)
{
  display->print(line);
}
void ScreenModule::print(char c)
{
  display->print(c);
}
void ScreenModule::print(unsigned char uc)
{
  display->print(uc);
}
void ScreenModule::print(int i)
{
  display->print(i);
}
void ScreenModule::print(unsigned int ui)
{
  display->print(ui);
}
void ScreenModule::print(long l)
{
  display->print(l);
}
void ScreenModule::print(unsigned long ul)
{
  display->print(ul);
}
void ScreenModule::print(double d)
{
  display->print(d);
}

void ScreenModule::eol()
{
  display->println();
}