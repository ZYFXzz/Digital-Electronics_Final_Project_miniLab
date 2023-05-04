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
  if (currentStep == numSteps) {
    currentStep = 0;
  }
}

void Stepper::setTimePerStep(int _timePerStep) {
  timePerStep = _timePerStep;
}

void Stepper::stepHandler(void (*f)(int)) {
  stepCallback = *f;
}

void Stepper::stepReset() {
  currentStep = 0;
}

void Stepper::stepChange(int _newStepNumber){
  numSteps = _newStepNumber;
}
