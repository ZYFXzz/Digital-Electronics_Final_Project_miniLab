#ifndef StevesAwesomePotentiometer_h
#define StevesAwesomePotentiometer_h

class StevesAwesomePotentiometer {
public:
  StevesAwesomePotentiometer(int _pin, int _num);
  void update();
  void turnHandler(void (*f)(int, int));
  void lock();


private:
  int releaseLockAmount = 25;
  int lockBackAmount = 5;
  int lockBackCounter = 0;
  int actualAnalogVal = 0;
  int lastActualAnalogVal = 0;
  bool locked = false;
  int lockedAtVal;
  int pin;
  int num;
  void (*turnCallback)(int, int);
};

#endif