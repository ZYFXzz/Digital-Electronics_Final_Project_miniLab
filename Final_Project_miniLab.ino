#include "StevesAwesomeButton.h"
#include "StevesAwesomePotentiometer.h"
#include "StevesAwesomeRotaryEncoder.h"
#include "MichaelsAwesomeOledController.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// // oled
// #define i2cSpeed 1000000  // see
//                           // https://forum.pjrc.com/threads/68357-Maximum-Speed-for-using-I2C-with-SSD1306-Display
// Adafruit_SSD1306 display(128, 64, &Wire, -1, i2cSpeed);
// MichaelsAwesomeOledController oled(&display);

StevesAwesomeRotaryEncoder rotaryEncoder(39, 38);


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

StevesAwesomeButton optionKeySwitch1(32, 0, INPUT_PULLUP);
StevesAwesomeButton optionKeySwitch2(31, 0, INPUT_PULLUP);
StevesAwesomeButton encoderButton(40, 0, INPUT_PULLUP);

int defaultMidiNotes[13] = { 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72 };  // midi notes set up on the middle c octave
int midiNotes[13] = { 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72 };         // the notes array to be modified
int a;

int octaveCounter = 0;

int encoderCcValue = 64;

// int potPins[1] = { A17 };
// int potVals[1] = { 0 };
// int *pots = potVals;



// StevesAwesomePotentiometer pot1(A17, 0);  //pin 41
// StevesAwesomePotentiometer *potObjects[1] = {&pot1};
// // possible more fader,{ 0,1,2,3 }


void setup() {
  //Serial
  Serial.begin(9600);
  // Serial.println("Serial.begin()");

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
  rotaryEncoder.rightClickHandler(encoderRightClick);
  rotaryEncoder.leftClickHandler(encoderLeftClick);

  // //fader/pot
  //      for(int i =0; i<1;i++){
  //        potObjects[i]->turnHandler(potTurn);
  //      }

  Serial.println("---system now online---");
}

void loop() {

  checkButtons();
  // updatePots();
  rotaryEncoder.process();
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

void optionKeySwitch1Press(int num) {
  Serial.println("option key 1 pressed");
  octaveCounter = octaveCounter + 1;
  for (int i = 0; i < 13; i++) {
    midiNotes[i] = defaultMidiNotes[i] + octaveCounter * 12;
  }
  Serial.print("Octave up/down for ");
  Serial.println(octaveCounter);
}

void optionKeySwitch2Press(int num) {
  Serial.println("option key 2 pressed");
  octaveCounter = octaveCounter - 1;
  for (int i = 0; i < 13; i++) {
    midiNotes[i] = defaultMidiNotes[i] + octaveCounter * 12;
  }
  Serial.print("Octave up/down for ");
  Serial.print(octaveCounter);
}
void encoderButtonPress(int num) {
  encoderCcValue = 63;
  usbMIDI.sendControlChange(10, 63, 1);
  Serial.print("encoderCcValue is ");
  Serial.println(63);
}

void encoderButtonRelease(int num) {
}

void encoderRightClick() {
  encoderCcValue = encoderCcValue + 1;
  encoderCcValue = constrain(encoderCcValue, 0, 127);
  Serial.print("encoderCcValue is ");
  Serial.println(encoderCcValue);

  usbMIDI.sendControlChange(10, encoderCcValue, 1);
}

void encoderLeftClick() {
  encoderCcValue = encoderCcValue - 1;
  encoderCcValue = constrain(encoderCcValue, 0, 127);
  Serial.print("encoderCcValue is ");
  Serial.println(encoderCcValue);
  usbMIDI.sendControlChange(10, encoderCcValue, 1);
}
