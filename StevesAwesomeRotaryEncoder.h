#ifndef StevesAwesomeRotaryEncoder_h
#define StevesAwesomeRotaryEncoder_h

#include "Arduino.h"

class StevesAwesomeRotaryEncoder
{
  public:

    StevesAwesomeRotaryEncoder(int _pin1, int _pin2);
    void process();
    void leftClickHandler(void (*f)());
    void rightClickHandler(void (*f)());
    void (*leftClickCallback)();
    void (*rightClickCallback)();

  private:

    int pin1;
    int pin2;
    int newPosition;
    int oldPosition;
    int subCount;
    int totalCount;
};

#endif
