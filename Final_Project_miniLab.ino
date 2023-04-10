#include "StevesAwesomeButton.h"
#include "StevesAwesomePotentiometer.h"
#include "StevesAwesomeRotaryEncoder.h"

#include "Stepper.h"


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//for display
#define SCREEN_WIDTH 128                                                    // OLED display width, in pixels
#define SCREEN_HEIGHT 64                                                    // OLED display height, in pixels
#define OLED_RESET -1                                                       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C                                                 ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);  // see:    for using multiple i2c, we can modify the display i2c bus, from Wire to Wire1 or Wire2
                                                                            // https://www.pjrc.com/teensy/td_libs_Wire.html


// leave these lines for now
// #define i2cSpeed 1000000  // see
//                           // https://forum.pjrc.com/threads/68357-Maximum-Speed-for-using-I2C-with-SSD1306-Display
// Adafruit_SSD1306 display(128, 64, &Wire, -1, i2cSpeed);
// MichaelsAwesomeOledController oled(&display);
//encoders
StevesAwesomeRotaryEncoder rotaryEncoder1(34, 33);


//buttons
StevesAwesomeButton cButton(0, 0, INPUT_PULLUP);
StevesAwesomeButton csButton(1, 1, INPUT_PULLUP);
StevesAwesomeButton dButton(2, 2, INPUT_PULLUP);
StevesAwesomeButton dsButton(3, 3, INPUT_PULLUP);
StevesAwesomeButton eButton(4, 4, INPUT_PULLUP);
StevesAwesomeButton fButton(5, 5, INPUT_PULLUP);
StevesAwesomeButton fsButton(6, 6, INPUT_PULLUP);
StevesAwesomeButton gButton(7, 7, INPUT_PULLUP);
StevesAwesomeButton gsButton(8, 8, INPUT_PULLUP);
StevesAwesomeButton aButton(9, 9, INPUT_PULLUP);
StevesAwesomeButton asButton(10, 10, INPUT_PULLUP);
StevesAwesomeButton bButton(11, 11, INPUT_PULLUP);
StevesAwesomeButton c2Button(12, 12, INPUT_PULLUP);

StevesAwesomeButton *keyboardButtons[13] = {
  &cButton, &csButton, &dButton, &dsButton, &eButton, &fButton, &fsButton,
  &gButton, &gsButton, &aButton, &asButton, &bButton, &c2Button
};

StevesAwesomeButton optionKeySwitch1(22, 0, INPUT_PULLUP);
StevesAwesomeButton optionKeySwitch2(23, 0, INPUT_PULLUP);
StevesAwesomeButton encoderButton(35, 0, INPUT_PULLUP);

int defaultMidiNotes[13] = { 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72 };  // midi notes set up on the middle c octave
int midiNotes[13] = { 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72 };         // the notes array to be modified


int octaveCounter = 0;

int encoderCcValue = 63;  // default value can be 63 or 64, depends on daw, there is no 63.5

Stepper stepper1(100, 100);  //need stepper for smoothing the fader when we are pushing it, we call lock() to lock the pot value every 0.
int potPins[4] = { A17, A16, A15, A14 };
int potVals[4] = { 0, 0, 0, 0 };
int potChannels[4] = { 1, 2, 3, 4 };
int *pots = potVals;
int lastPotVals[4] = { 0, 0, 0, 0 };


StevesAwesomePotentiometer pot0(A17, 0);  //pin 41
StevesAwesomePotentiometer pot1(A16, 1);
StevesAwesomePotentiometer pot2(A15, 2);
StevesAwesomePotentiometer pot3(A14, 3);
StevesAwesomePotentiometer *potObjects[4] = { &pot0, &pot1, &pot2, &pot3 };


void setup() {

  //Serial
  Serial.begin(9600);
  // Serial.println("Serial.begin()");
  stepper1.stepHandler(onStep);
  stepper1.maxStepHandler(onMaxStep);


  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  //keyboard buttons
  for (int i = 0; i < 13; i++) {
    keyboardButtons[i]->pressHandler(notePress);
    keyboardButtons[i]->holdHandler(noteHold);
    keyboardButtons[i]->releaseHandler(noteRelease);
  }

  // // other buttons
  optionKeySwitch1.pressHandler(optionKeySwitch1Press);
  optionKeySwitch2.pressHandler(optionKeySwitch2Press);

  encoderButton.pressHandler(encoderButtonPress);
  encoderButton.releaseHandler(encoderButtonRelease);

  //encoder
  rotaryEncoder1.rightClickHandler(encoderRightClick);
  rotaryEncoder1.leftClickHandler(encoderLeftClick);

  //pots/faders
  for (int i = 0; i < 4; i++) {
    potObjects[i]->turnHandler(potTurn);
    potObjects[i]->lock();
    lastPotVals[i] = potVals[i];
  }


  // time, millis(), stepper.h

  //oled
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 28);
  display.println(F("----System Online----"));
  display.println(F("         >_<         "));
  display.display();

  Serial.println("---system now online---");
}

void loop() {

  checkButtons();
  updatePots();
  rotaryEncoder1.process();
  stepper1.process();
}

void updatePots() {
  for (int i = 0; i < 4; i++) {
    potObjects[i]->update();
  }
}
void onStep(int k) {
  for (int i = 0; i < 4; i++) {
    lastPotVals[i] = potVals[i];
    if (abs(potVals[i] - lastPotVals[i] < 5)) {
      potObjects[i]->lock();
    }
  }
}

void onMaxStep() {
  displayReading(4, 0, 0, 0);
}

void potTurn(int number, int analogVal) {
  Serial.println("truning pot");
  int a = map(analogVal, 0, 1023, 0, 125);
  usbMIDI.sendControlChange(potChannels[number], a, 1);
}

void checkButtons() {
  optionKeySwitch1.process();
  optionKeySwitch2.process();
  encoderButton.process();

  for (int i = 0; i < 13; i++) {
    keyboardButtons[i]->process();
  }
}


void notePress(int num) {
  usbMIDI.sendNoteOn(midiNotes[num], 112, 1);
  Serial.print(num);
  Serial.println("note pressed");
}

void noteHold(int num) {
}

void noteRelease(int num) {
  usbMIDI.sendNoteOff(midiNotes[num], 60, 1);
  Serial.println("note released");
}

void allMidiNotesOff() {
  for (int i = 0; i < 13; i++) {
    usbMIDI.sendNoteOff(midiNotes[i], 60, 1);
  }
}

void optionKeySwitch1Press(int num) {

  Serial.println("option key 1 pressed");
  octaveCounter = octaveCounter + 1;
  allMidiNotesOff();
  for (int i = 0; i < 13; i++) {
    midiNotes[i] = defaultMidiNotes[i] + octaveCounter * 12;
  }
  Serial.print("Octave up/down for ");
  Serial.println(octaveCounter);
  displayReading(3, 0, 0, 0);
}

void optionKeySwitch2Press(int num) {
  Serial.println("option key 2 pressed");
  octaveCounter = octaveCounter - 1;
  allMidiNotesOff();
  for (int i = 0; i < 13; i++) {
    midiNotes[i] = defaultMidiNotes[i] + octaveCounter * 12;
  }
  Serial.print("Octave up/down for ");
  Serial.print(octaveCounter);
  displayReading(3, 0, 0, 0);
}
void encoderButtonPress(int num) {
  encoderCcValue = 63;
  usbMIDI.sendControlChange(10, 63, 1);
  Serial.print("encoderCcValue is ");
  Serial.println(63);
  displayReading(2, 0, 3, encoderCcValue);
}

void encoderButtonRelease(int num) {
}

void encoderRightClick() {
  encoderCcValue = encoderCcValue + 1;
  encoderCcValue = constrain(encoderCcValue, 0, 127);
  Serial.print("encoderCcValue is ");
  Serial.println(encoderCcValue);
  usbMIDI.sendControlChange(10, encoderCcValue, 1);
  displayReading(2, 0, 1, encoderCcValue);
}

void encoderLeftClick() {
  encoderCcValue = encoderCcValue - 1;
  encoderCcValue = constrain(encoderCcValue, 0, 127);
  Serial.print("encoderCcValue is ");
  Serial.println(encoderCcValue);
  usbMIDI.sendControlChange(10, encoderCcValue, 1);
  displayReading(2, 0, 2, encoderCcValue);
}

void displayReading(byte mode, byte potNum, byte encoderDirection, int encoderValue) {
  if (mode == 1) {  // screen print pot value
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 25);
    display.println(F("pot Value reading: "));
    int k = lastPotVals[potNum];
    display.println(k);
    display.display();
  } else if (mode == 2) {  // screen print encoder value
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 25);
    display.println(F("encoder reading~~~ "));
    display.print(F("Direction: "));
    if (encoderDirection == 1) {
      display.print(F("CW"));
    } else if (encoderDirection == 2) {
      display.print(F("ACW"));
    } else if (encoderDirection == 3) {
      display.println(F("click"));
    }
    display.println(F(" | Counter: "));
    display.print(encoderValue);
    display.display();
  } else if (mode == 3) {  // screen print  value
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 25);
    if (octaveCounter > 0) {
      display.println(F("Octave up: +"));
      display.print(octaveCounter);
    } else if (octaveCounter < 0) {
      display.println(F("Octave down: +"));
      display.print(octaveCounter);

    } else if (octaveCounter == 0) {
      display.println(F("Default Octave Position"));
    }

    display.display();
  } else if (mode == 4) {  // dispay something when there is no input
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 28);
    display.println(F("--waiting for input--"));
    display.println(F("        (-_- )       "));

    display.display();
  }
}
