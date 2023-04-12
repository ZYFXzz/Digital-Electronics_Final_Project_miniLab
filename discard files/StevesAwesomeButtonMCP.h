#ifndef StevesAwesomeButtonMCP_h
#define StevesAwesomeButtonMCP_h

#include "Arduino.h"

class StevesAwesomeButtonMCP
{
  public:
    StevesAwesomeButtonMCP(int _buttonPin, int _buttonNum, int _buttonType);
    StevesAwesomeButtonMCP(int _buttonPin, int _buttonNum);
    
    void processMCP();

    void pressHandlerMCP(void (*f)(int));
    void holdHandlerMCP(void (*f)(int));
    void releaseHandlerMCP(void (*f)(int));
    void (*pressCallbackMCP)(int);
    void (*holdCallbackMCP)(int);
    void (*releaseCallbackMCP)(int);

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
