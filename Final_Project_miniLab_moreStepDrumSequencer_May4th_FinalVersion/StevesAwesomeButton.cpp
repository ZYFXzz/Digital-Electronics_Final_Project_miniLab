#include "Arduino.h"
#include "StevesAwesomeButton.h"

StevesAwesomeButton::StevesAwesomeButton(int _buttonPin, int _buttonNum, int _buttonType)
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

  pinMode(buttonPin, buttonType);
}


StevesAwesomeButton::StevesAwesomeButton(int _buttonPin, int _buttonNum)
{
  buttonPin = _buttonPin;
  buttonNum = _buttonNum;
  buttonState = 0;
  lastButtonState = 0;

  // this constructor has no buttonType arg so we assume its an INPUT
  buttonType = INPUT;
  onState = HIGH;
  offState = LOW;

  pinMode(buttonPin, buttonType);
}

void StevesAwesomeButton::process()
{
  lastButtonState = buttonState;
  buttonState = digitalRead(buttonPin);

  if (lastButtonState == offState && buttonState == onState) {
    if (pressCallback != NULL) pressCallback(buttonNum);
    delay(5);
  }
  else if (lastButtonState == onState && buttonState == onState) {
    if (holdCallback != NULL) holdCallback(buttonNum);
    delay(5);
  }
  else if (lastButtonState == onState && buttonState == offState) {
    if (releaseCallback != NULL) releaseCallback(buttonNum);
    delay(5);
  }
}

void StevesAwesomeButton::pressHandler(void (*f)(int))    //button down
{
  pressCallback = *f;
}

void StevesAwesomeButton::holdHandler(void (*f)(int))  //button hold
{
  holdCallback = *f;
}

void StevesAwesomeButton::releaseHandler(void (*f)(int))  //button release
{
  releaseCallback = *f;
}
