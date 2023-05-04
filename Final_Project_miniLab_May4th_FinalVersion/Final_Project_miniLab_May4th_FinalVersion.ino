//10th comment: basic button worker, copy test code from mcp23_button_tested, it worked
//just need to find out how to create object with mcp

// other note, need to write class for displayReading

// steve's classes
#include "StevesAwesomeButton.h"
#include "StevesAwesomePotentiometer.h"
#include "StevesAwesomeRotaryEncoder.h"
#include "Stepper.h"
//other libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel neopixel = Adafruit_NeoPixel(8, 32, NEO_RGB);

//mcp buttons, 12 keys, from cs2 to c3, not objects
#include <Adafruit_MCP23X17.h>
#define cs2ButtonPin 0
#define d2ButtonPin 1
#define ds2ButtonPin 2
#define e2ButtonPin 3
#define f2ButtonPin 4
#define fs2ButtonPin 5
#define g2ButtonPin 6
#define gs2ButtonPin 7
#define a2ButtonPin 8
#define as2ButtonPin 9
#define b2ButtonPin 10
#define c3ButtonPin 11
#define optionButtonKey3Pin 12
#define optionButtonKey4Pin 13
Adafruit_MCP23X17 mcp;

bool buttonStates[12] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
bool lastButtonStates[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int buttonPins[12] = { cs2ButtonPin, d2ButtonPin, ds2ButtonPin, e2ButtonPin,
                       f2ButtonPin, fs2ButtonPin, g2ButtonPin, gs2ButtonPin,
                       a2ButtonPin, as2ButtonPin, b2ButtonPin, c3ButtonPin };
int buttonNum[12] = { 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };

//for display， Wire1 used, save Wire for mcp23017   Makes things easier, Wire -> mcp23017, Wire1 -> display, instead of on a single i2c bus
#define SCREEN_WIDTH 128                                                    // OLED display width, in pixels
#define SCREEN_HEIGHT 64                                                    // OLED display height, in pixels
#define OLED_RESET -1                                                       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C                                                 ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32 // for the 4 dollar screen we are using, need to use 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);  // see:    for using multiple i2c, we can modify the display i2c bus, from Wire to Wire1 or Wire2
                                                                            // https://www.pjrc.com/teensy/td_libs_Wire.html

//encoders, encoder buttons see below in buttons section
StevesAwesomeRotaryEncoder rotaryEncoderMaster(29, 30, 0);
StevesAwesomeRotaryEncoder rotaryEncoder1(10, 11, 1);
StevesAwesomeRotaryEncoder rotaryEncoder2(7, 8, 2);
StevesAwesomeRotaryEncoder rotaryEncoder3(4, 5, 3);
StevesAwesomeRotaryEncoder rotaryEncoder4(1, 2, 4);
StevesAwesomeRotaryEncoder *rotaryEncoderObjects[5] = { &rotaryEncoderMaster, &rotaryEncoder1, &rotaryEncoder2, &rotaryEncoder3, &rotaryEncoder4 };

//buttons, using awesome button class
StevesAwesomeButton cButton(20, 0, INPUT_PULLUP);
StevesAwesomeButton csButton(21, 1, INPUT_PULLUP);
StevesAwesomeButton dButton(15, 2, INPUT_PULLUP);
StevesAwesomeButton dsButton(14, 3, INPUT_PULLUP);
StevesAwesomeButton eButton(41, 4, INPUT_PULLUP);
StevesAwesomeButton fButton(40, 5, INPUT_PULLUP);
StevesAwesomeButton fsButton(39, 6, INPUT_PULLUP);
StevesAwesomeButton gButton(38, 7, INPUT_PULLUP);
StevesAwesomeButton gsButton(37, 8, INPUT_PULLUP);
StevesAwesomeButton aButton(36, 9, INPUT_PULLUP);
StevesAwesomeButton asButton(35, 10, INPUT_PULLUP);
StevesAwesomeButton bButton(34, 11, INPUT_PULLUP);
StevesAwesomeButton c2Button(33, 12, INPUT_PULLUP);


StevesAwesomeButton *keyboardButtons[13] = {
  &cButton, &csButton, &dButton, &dsButton, &eButton, &fButton, &fsButton,
  &gButton, &gsButton, &aButton, &asButton, &bButton, &c2Button
};

StevesAwesomeButton optionKeySwitch1(23, 0, INPUT_PULLUP);
StevesAwesomeButton optionKeySwitch2(22, 0, INPUT_PULLUP);
StevesAwesomeButton encoderButtonMaster(28, 0, INPUT_PULLUP);  //encoder 0
StevesAwesomeButton encoderButton1(9, 1, INPUT_PULLUP);
StevesAwesomeButton encoderButton2(6, 2, INPUT_PULLUP);
StevesAwesomeButton encoderButton3(3, 3, INPUT_PULLUP);
StevesAwesomeButton encoderButton4(0, 4, INPUT_PULLUP);
StevesAwesomeButton *encoderButtonObjects[5] = { &encoderButtonMaster, &encoderButton1, &encoderButton2, &encoderButton3, &encoderButton4 };

//midi notes, 2 octave, from middle c1 to c3
int defaultMidiNotes[25] = { 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84 };  // midi notes set up on the middle c octave
int midiNotes[25] = { 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84 };         // the notes array to be modified, notes we are actually using


int octaveCounter = 0;  // for octave up/down

int encoderCcValues[4] = { 63, 63, 63, 63 };  //for encoder 1,2,3,4
int encoderCcChannels[4] = { 11, 12, 13, 14 };

// default value can be 63 or 64, depends on daw, there is no 63.5 in midi value
// for fl studio, it will read 63(or 64), and did a small math on it, and output "center"("63.5") to the desired parameter
// for fl studio, reading 63 or 64, cannot put a pan parameter to center, but to 1% right/left

// automatic control based on time
Stepper stepper(20, 250);            //stepper for smoothing the fader when we are pushing it, we call lock() to lock the pot value every 0.25 second
bool somethingJustDisplayed = true;  // a boolean for screen graph, once a displayReading() is called, it will return true
// pots
int potPins[4] = { A13, A12, A11, A10 };
int potVals[4] = { 0, 0, 0, 0 };
int potChannels[4] = { 1, 2, 3, 4 };
int *pots = potVals;
int lastPotVals[4] = { 0, 0, 0, 0 };


StevesAwesomePotentiometer pot0(A13, 0);  //pin 41
StevesAwesomePotentiometer pot1(A12, 1);
StevesAwesomePotentiometer pot2(A11, 2);
StevesAwesomePotentiometer pot3(A10, 3);
StevesAwesomePotentiometer *potObjects[4] = { &pot0, &pot1, &pot2, &pot3 };


void setup() {

  //Serial
  Serial.begin(9600);
  // Serial.println("Serial.begin()");
  stepper.stepHandler(onStep);
  stepper.maxStepHandler(onMaxStep);
  if (!mcp.begin_I2C()) {
    //if (!mcp.begin_SPI(CS_PIN)) {
    Serial.println("Error.");
    while (1)
      ;
  }
  mcp.begin_I2C();
  for (int i = 0; i < 12; i++) {
    mcp.pinMode(buttonPins[i], INPUT_PULLUP);
  }


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
  encoderButtonMaster.pressHandler(encoderButtonMasterPress);
  encoderButtonMaster.releaseHandler(encoderButtonMasterRelease);

  for (int i = 1; i < 5; i++) {
    encoderButtonObjects[i]->pressHandler(encoderButtonPress);
    encoderButtonObjects[i]->releaseHandler(encoderButtonRelease);
  }

  //encoder
  rotaryEncoderMaster.rightClickHandler(encoderMasterRightClick);
  rotaryEncoderMaster.leftClickHandler(encoderMasterLeftClick);
  for (int i = 1; i < 5; i++) {
    rotaryEncoderObjects[i]->rightClickHandler(encoderRightClick);
    rotaryEncoderObjects[i]->leftClickHandler(encoderLeftClick);
  }


  //pots/faders
  for (int i = 0; i < 4; i++) {
    potObjects[i]->turnHandler(potTurn);
    potObjects[i]->lock();
    lastPotVals[i] = potVals[i];
  }

  //neopixels
  neopixel.begin();
  neopixel.clear();
  neopixel.show();


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
  checkMcpButtons();
  checkButtons();
  updatePots();
  checkRotaryEncoders();
  stepper.process();
}

void checkMcpButtons() {
  for (int i = 0; i < 12; i++) {
    lastButtonStates[i] = buttonStates[i];
    buttonStates[i] = mcp.digitalRead(buttonPins[i]);

    if (buttonStates[i] == LOW && lastButtonStates[i] == HIGH) {
      notePress(buttonNum[i]);
      delay(5);
    } else if (buttonStates[i] == HIGH && lastButtonStates[i] == LOW) {
      noteRelease(buttonNum[i]);
      delay(5);
    }
  }
}
void checkButtons() {
  for (int i = 0; i < 13; i++) {
    keyboardButtons[i]->process();
  }
  for (int i = 0; i < 5; i++) {
    encoderButtonObjects[i]->process();
  }
  optionKeySwitch1.process();
  optionKeySwitch2.process();
}

void notePress(int num) {
  usbMIDI.sendNoteOn(midiNotes[num], 112, 1);
  Serial.print(num);
  Serial.println("note pressed");
  displayReading(5, 0, 0, 0, midiNotes[num]);
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
  Serial.println("option key 2 pressed");
  octaveCounter = octaveCounter - 1;
  allMidiNotesOff();//turn off all previous notes 
  for (int i = 0; i < 25; i++) {
    midiNotes[i] = defaultMidiNotes[i] + octaveCounter * 12;
  }
  Serial.print("Octave up/down for ");
  Serial.print(octaveCounter);
  displayReading(3, 0, 0, 0, 0);
}

void optionKeySwitch2Press(int num) {
  Serial.println("option key 1 pressed");
  octaveCounter = octaveCounter + 1;
  allMidiNotesOff();
  for (int i = 0; i < 25; i++) {
    midiNotes[i] = defaultMidiNotes[i] + octaveCounter * 12;
  }
  Serial.print("Octave up/down for ");
  Serial.println(octaveCounter);
  displayReading(3, 0, 0, 0, 0);
}

void encoderButtonMasterPress(int num) {
}
void encoderButtonMasterRelease(int num) {
}


void encoderButtonPress(int num) {
  encoderCcValues[num - 1] = 63;
  usbMIDI.sendControlChange(encoderCcChannels[num - 1], 63, 1);
  Serial.print("encoderCcValue is ");
  Serial.println(63);
  displayReading(2, 0, 3, encoderCcValues[num - 1], 0);
}

void encoderButtonRelease(int num) {
}

void updatePots() {
  for (int i = 0; i < 4; i++) {
    potObjects[i]->update();
  }
}


void potTurn(int number, int analogVal) {
  Serial.println("truning pot");
  int a = map(analogVal, 0, 1023, 0, 125);
  usbMIDI.sendControlChange(potChannels[number], a, 1);
}

void checkRotaryEncoders() {
  for (int i = 0; i < 5; i++) {
    rotaryEncoderObjects[i]->process();
  }
}

void encoderMasterLeftClick(int num) {
}

void encoderMasterRightClick(int num) {
}

void encoderRightClick(int num) {
  encoderCcValues[num - 1] = encoderCcValues[num - 1] + 5;
  encoderCcValues[num - 1] = constrain(encoderCcValues[num - 1], 0, 127);
  Serial.print("encoderCcValue is ");
  Serial.println(encoderCcValues[num + 1]);
  usbMIDI.sendControlChange(encoderCcChannels[num - 1], encoderCcValues[num - 1], 1);
  displayReading(2, 0, 1, encoderCcValues[num - 1], 0);
}

void encoderLeftClick(int num) {
  encoderCcValues[num - 1] = encoderCcValues[num - 1] - 5;
  encoderCcValues[num - 1] = constrain(encoderCcValues[num - 1], 0, 127);
  Serial.print("encoderCcValue is ");
  Serial.println(encoderCcValues[num - 1]);
  usbMIDI.sendControlChange(encoderCcChannels[num - 1], encoderCcValues[num - 1], 1);
  displayReading(2, 0, 2, encoderCcValues[num - 1], 0);
}

void onStepCheckPotPushed() {
  for (int i = 0; i < 4; i++) {
    lastPotVals[i] = potVals[i];

    if (abs(potVals[i] - lastPotVals[i] < 5)) {
      potObjects[i]->lock();
    }
  }
}

void onStep(int k) {
  onStepCheckSomethingDisplayed();
  onStepCheckPotPushed();
  onStepBlinkNeopixel();
}
void onStepCheckSomethingDisplayed() {
  if (somethingJustDisplayed == true) {
    somethingJustDisplayed = false;
    stepper.stepReset();
  }
}

void onStepBlinkNeopixel() {
  neopixel.setPixelColor(0, random(0, 256), random(0, 256), random(0, 256));
  neopixel.setPixelColor(1, random(0, 256), random(0, 256), random(0, 256));
  neopixel.setPixelColor(2, random(0, 256), random(0, 256), random(0, 256));
  neopixel.setPixelColor(3, random(0, 256), random(0, 256), random(0, 256));
  neopixel.setPixelColor(4, random(0, 256), random(0, 256), random(0, 256));
  neopixel.setPixelColor(5, random(0, 256), random(0, 256), random(0, 256));
  neopixel.setPixelColor(6, random(0, 256), random(0, 256), random(0, 256));
  neopixel.setPixelColor(7, random(0, 256), random(0, 256), random(0, 256));
  neopixel.setBrightness(30);
  neopixel.show();
}

void onMaxStep() {
  displayReading(4, 0, 0, 0, 0);
}





void displayReading(byte mode, byte potNum, byte encoderDirection, int encoderValue, int noteName) {
  if (mode == 1) {  // screen print pot value    // pot value printing is harder, mode 1 is temporarily abandonded and disabled
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 25);
    display.println(F("pot Value reading: "));
    int k = lastPotVals[potNum];
    display.println(k);

    display.display();
    somethingJustDisplayed = true;
  } else if (mode == 2) {  // screen print encoder value
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 25);
    display.println(F("encoder reading~~~ "));
    display.print(F("Direction: "));
    if (encoderDirection == 1) {
      display.println(F("CW"));
    } else if (encoderDirection == 2) {
      display.println(F("ACW"));
    } else if (encoderDirection == 3) {
      display.println(F("click"));
    }
    display.print(F("Counter: "));
    display.print(encoderValue);

    display.display();
    somethingJustDisplayed = true;
  } else if (mode == 3) {  // screen print  value
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 25);
    if (octaveCounter > 0) {
      display.print(F("Octave up: +"));
      display.print(octaveCounter);
    } else if (octaveCounter < 0) {
      display.print(F("Octave down: "));
      display.print(octaveCounter);

    } else if (octaveCounter == 0) {
      display.println(F("Default Octave Position"));
    }

    display.display();
    somethingJustDisplayed = true;
  } else if (mode == 4) {  // dispay something when there is no input
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 28);
    display.println(F("--waiting for input--"));
    display.println(F("        (-_- )       "));

    display.display();
    somethingJustDisplayed = true;
  } else if (mode == 5) {  //display midi note
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 28);
    display.print(F("  press note  "));
    display.println(noteName);

    display.display();
    somethingJustDisplayed = true;
  }
}
