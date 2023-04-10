#include "MichaelsAwesomeOledController.h"


MichaelsAwesomeOledController::MichaelsAwesomeOledController(Adafruit_SSD1306 *_oled) 
{
  oled = _oled;

  oled->begin(SSD1306_SWITCHCAPVCC, 0x3c);
  oled->clearDisplay();
  oled->display();

  //print "Hello, world"
  oled->clearDisplay();

  oled->setTextSize(1);
  oled->setTextColor(SSD1306_WHITE);
  oled->setCursor(0, 28);
  oled->println(F("Hello, world"));

  oled->display();
}

void MichaelsAwesomeOledController::displayCcValueSent(int _ccValue, int _ccChannel) {
  oled->clearDisplay();

  oled->setTextSize(1);
  oled->setTextColor(SSD1306_WHITE);
  oled->setCursor(0, 28);
  oled->println(_ccChannel);//print _ccName
  oled->println(_ccValue);

  oled->display();
}

void MichaelsAwesomeOledController::displayButtonPressed(int _midiNumb){
  oled->clearDisplay();

  oled->setTextSize(1);
  oled->setTextColor(SSD1306_WHITE);
  oled->setCursor(0, 28);
  oled->print(F("Midi Note "));
  oled->print(_midiNumb);
  oled->println(F(" is pressed"));

  oled->display();
}



void MichaelsAwesomeOledController::clear() {
  oled->clearDisplay();
  oled->display();
}
