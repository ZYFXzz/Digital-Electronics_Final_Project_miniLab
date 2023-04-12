#ifndef Stepper_h
#define Stepper_h

#include "Arduino.h"

class Stepper {
public:
  Stepper(int _numSteps, int _timePerStep);
  void process();
  void nextStep();
  void stepHandler(void (*f)(int));
  void maxStepHandler(void (*f)());
  void (*maxStepCallback)();
  void (*stepCallback)(int);
  void setTimePerStep(int _timePerStep);
  void stepReset();

  int numSteps;
  int currentStep;
  int timePerStep;
  unsigned long lastStepTime;
};

#endif
