#ifndef MichaelsAwesomeOledController_h
#define MichaelsAwesomeOledController_h

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class MichaelsAwesomeOledController 
{
public:
  MichaelsAwesomeOledController(Adafruit_SSD1306 *_oled);
  void displayButtonPressed(int _midiNumb);
  void displayCcValueSent(int _ccValue, int _ccChannel);
  void clear();
private:
  Adafruit_SSD1306 *oled; 
};

#endif