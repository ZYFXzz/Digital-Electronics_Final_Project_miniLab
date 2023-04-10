#include "StevesAwesomePotentiometer.h"
#include "Arduino.h"

StevesAwesomePotentiometer::StevesAwesomePotentiometer(int _pin, int _num) {
  pin = _pin;
  num = _num;
}

void StevesAwesomePotentiometer::update() {
  lastActualAnalogVal = actualAnalogVal;
  actualAnalogVal = analogRead(pin);
  if (locked) {
    // Serial.println("locked");
    if (abs(lockedAtVal - actualAnalogVal) > releaseLockAmount) locked = false;
  } else {
    // Serial.println("not locked");
    if (lastActualAnalogVal != actualAnalogVal) turnCallback(num, actualAnalogVal);
  }
}

void StevesAwesomePotentiometer::lock() {
  locked = true;
  lockedAtVal = analogRead(pin);
}




void StevesAwesomePotentiometer::turnHandler(void (*f)(int, int)) {
  turnCallback = *f;
}