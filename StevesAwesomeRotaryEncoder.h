#ifndef StevesAwesomeRotaryEncoder_h
#define StevesAwesomeRotaryEncoder_h

#include "Arduino.h"

class StevesAwesomeRotaryEncoder
{
  public:

    StevesAwesomeRotaryEncoder(int _pin1, int _pin2, int _encoderNumb);
    void process();
    void leftClickHandler(void (*f)(int));
    void rightClickHandler(void (*f)(int));
    void (*leftClickCallback)(int);
    void (*rightClickCallback)(int);

  private:

    int pin1;
    int pin2;
    int newPosition;
    int oldPosition;
    int subCount;
    int totalCount;
    int encoderNumb;
};

#endif
