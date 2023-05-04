#include "Arduino.h"
#include "StevesAwesomeRotaryEncoder.h"

StevesAwesomeRotaryEncoder::StevesAwesomeRotaryEncoder(int _pin1, int _pin2, int _encoderNumb)
{
  pin1 = _pin1;
  pin2 = _pin2;
  encoderNumb = _encoderNumb;

  oldPosition = -1;
  newPosition = -1;
  subCount = 0;
  totalCount = 0;

  pinMode(pin1, INPUT);
  digitalWrite(pin1, HIGH);
  pinMode(pin2, INPUT);
  digitalWrite(pin2, HIGH);
}

void StevesAwesomeRotaryEncoder::leftClickHandler(void (*f)(int))
{
  leftClickCallback = *f;
}

void StevesAwesomeRotaryEncoder::rightClickHandler(void (*f)(int))
{
  rightClickCallback = *f;
}

void StevesAwesomeRotaryEncoder::process()
{
  newPosition = (digitalRead(pin2) * 2) + digitalRead(pin1);

  if (newPosition != oldPosition) {

    int isFwd = ((oldPosition == 0) && (newPosition == 1)) ||
                ((oldPosition == 1) && (newPosition == 3)) ||
                ((oldPosition == 3) && (newPosition == 2)) ||
                ((oldPosition == 2) && (newPosition == 0));

    if (isFwd == true) subCount++;
    else if (isFwd == false) subCount--;

    if (subCount == -4) {
      subCount = 0;
      rightClickCallback(encoderNumb);
    } else if (subCount == 4) {
      subCount = 0;
      leftClickCallback(encoderNumb);
    }
    oldPosition = newPosition;
  }
}
