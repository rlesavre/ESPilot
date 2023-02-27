#include "Screen.h"

ScreenModule::ScreenModule(Subject<int> *buttonPressed, TwoWire *twi){
  buttonPressed->registerObserver(this);
  display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, twi, OLED_RESET);
}

void ScreenModule::setup(){
  if(!display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display->ssd1306_command(SSD1306_SETCONTRAST);
  display->ssd1306_command(SCREEN_BRIGHTNESS);
}

void ScreenModule::loop() {
  if (millis() - lastDraw >= DRAW_PERIOD) {
    lastDraw = millis();
    this->pages.at(currentPage % pages.size())->displayScreenPage(this->display, this->currentPositionInPage);
  }
}

void ScreenModule::addPage(IScreenPage *page){
  if(page != NULL){
    pages.push_back(page);
  }  
}

void ScreenModule::nextPage(){
  currentPage++;
  if(currentPage >= pages.size()){
    currentPage = 0;
  }
  currentPositionInPage = 0;
}

void ScreenModule::previousPage(){
  currentPage--;
  if(currentPage < 0 ){
    currentPage = pages.size()-1;
  }
  currentPositionInPage = 0;
}

void ScreenModule::moveDown(){
  currentPositionInPage++;
}

void ScreenModule::moveUp(){
  currentPositionInPage--;
}

void ScreenModule::notify(int arg)
{
  switch(arg){
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

void ScreenModule::displayScreenPage(Adafruit_SSD1306 *display, int position){
  display->clearDisplay();
  display->setTextSize(1);
  display->setCursor(0, 0);
  display->setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display->println("SCREEN MODULE");
  display->setTextColor(WHITE);
  display->print("Screen page count");
  display->println(pages.size());
  display->print("- page: ");
  display->println(currentPage);
  display->print("- position: ");
  display->println(currentPositionInPage);
  display->display();
}