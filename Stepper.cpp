#include "Stepper.h"



Stepper::Stepper(int _numSteps, int _timePerStep) {
  numSteps = _numSteps;
  timePerStep = _timePerStep;
  lastStepTime = 0;
}

void Stepper::process() {
  if (millis() > lastStepTime + timePerStep) {
    lastStepTime = millis();
    nextStep();
    stepCallback(currentStep);
  }
}

void Stepper::nextStep() {
  currentStep++;
  if (currentStep >= numSteps) {
    currentStep = 0;
    maxStepCallback();
  }
}
void Stepper::maxStepHandler(void (*f)()){  // add this for extra parameter/control input, when number of step reached its maximum
  maxStepCallback = *f;
}

void Stepper::setTimePerStep(int _timePerStep) {
  timePerStep = _timePerStep;
}

void Stepper::stepHandler(void (*f)(int)) {
  stepCallback = *f;
}
