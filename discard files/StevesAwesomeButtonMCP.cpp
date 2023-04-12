#include "Arduino.h"
#include <Adafruit_MCP23X17.h>
#include "StevesAwesomeButtonMCP.h"



StevesAwesomeButton::StevesAwesomeButtonMCP(int _buttonPin, int _buttonNum, int _buttonType)
{
  buttonPin = _buttonPin;
  buttonNum = _buttonNum;
  buttonState = 0;
  lastButtonState = 0;
  buttonType = _buttonType;

  if (buttonType == INPUT) {
    onState = HIGH;
    offState = LOW;
  } else {
    onState = LOW;
    offState = HIGH;
  }

  mcp = Adafruit_MCP23X17();

  // initialize the MCP23X17
  if (!mcp.begin_I2C()) {
    Serial.println("Failed to initialize MCP23X17!");
    while (1);
  }

  mcp.pinMode(buttonPin, buttonType);
}


StevesAwesomeButton::StevesAwesomeButtonMCP(int _buttonPin, int _buttonNum)
{
  buttonPin = _buttonPin;
  buttonNum = _buttonNum;
  buttonState = 0;
  lastButtonState = 0;

  // this constructor has no buttonType arg so we assume its an INPUT
  buttonType = INPUT;
  onState = HIGH;
  offState = LOW;

  mcp.pinMode(buttonPin, buttonType);
}

void StevesAwesomeButton::processMCP()
{
  lastButtonState = buttonState;
  buttonState = digitalRead(buttonPin);

  if (lastButtonState == offState && buttonState == onState) {
    if (pressCallbackMCP != NULL) pressCallback(buttonNum);
    delay(5);
  }
  else if (lastButtonState == onState && buttonState == onState) {
    if (holdCallbackMCP != NULL) holdCallback(buttonNum);
    delay(5);
  }
  else if (lastButtonState == onState && buttonState == offState) {
    if (releaseCallbackMCP != NULL) releaseCallback(buttonNum);
    delay(5);
  }
}

void StevesAwesomeButton::pressHandlerMCP(void (*f)(int))    //button down
{
  pressCallbackMCP = *f;
}

void StevesAwesomeButton::holdHandlerMCP(void (*f)(int))  //button hold
{
  holdCallbackMCP = *f;
}

void StevesAwesomeButton::releaseHandlerMCP(void (*f)(int))  //button release
{
  releaseCallbackMCP = *f;
}
