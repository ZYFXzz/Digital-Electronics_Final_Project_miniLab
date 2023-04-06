#ifndef StevesAwesomeButton_h
#define StevesAwesomeButton_h

#include "Arduino.h"

class StevesAwesomeButton
{
  public:
    StevesAwesomeButton(int _buttonPin, int _buttonNum, int _buttonType);
    StevesAwesomeButton(int _buttonPin, int _buttonNum);
    
    void process();

    void pressHandler(void (*f)(int));
    void holdHandler(void (*f)(int));
    void releaseHandler(void (*f)(int));
    void (*pressCallback)(int);
    void (*holdCallback)(int);
    void (*releaseCallback)(int);

  private:
    int buttonPin;
    int buttonNum;
    bool buttonState;
    bool lastButtonState;
    int buttonType;
    bool onState;
    bool offState;
};

#endif
