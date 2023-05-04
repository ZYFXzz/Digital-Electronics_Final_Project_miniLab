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
Adafruit_NeoPixel neopixel = Adafruit_NeoPixel(8, 32, NEO_RGB);  // neo_RGB

//for display， Wire1 used, save Wire for mcp23017   Makes things easier, Wire -> mcp23017, Wire1 -> display, instead of on a single i2c bus
#define SCREEN_WIDTH 128                                                    // OLED display width, in pixels
#define SCREEN_HEIGHT 64                                                    // OLED display height, in pixels
#define OLED_RESET -1                                                       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C                                                 ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32 // for the 4 dollar screen we are using, need to use 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);  // see:    for using multiple i2c, we can modify the display i2c bus, from Wire to Wire1 or Wire2
                                                                            // https://www.pjrc.com/teensy/td_libs_Wire.html

//Rotary Encodder Objects
StevesAwesomeRotaryEncoder rotaryEncoderMaster(29, 30, 0);

//buttons, using awesome button class
StevesAwesomeButton cButton(20, 0, INPUT_PULLUP);
StevesAwesomeButton dButton(15, 1, INPUT_PULLUP);
StevesAwesomeButton eButton(41, 2, INPUT_PULLUP);
StevesAwesomeButton fButton(40, 3, INPUT_PULLUP);
StevesAwesomeButton gButton(38, 4, INPUT_PULLUP);
StevesAwesomeButton aButton(36, 5, INPUT_PULLUP);
StevesAwesomeButton bButton(34, 6, INPUT_PULLUP);
StevesAwesomeButton c2Button(33, 7, INPUT_PULLUP);


StevesAwesomeButton *keyboardButtons[8] = {
  &cButton, &dButton, &eButton, &fButton,
  &gButton, &aButton, &bButton, &c2Button
};

StevesAwesomeButton optionKeySwitch1(22, 0, INPUT_PULLUP);
StevesAwesomeButton optionKeySwitch2(23, 0, INPUT_PULLUP);
StevesAwesomeButton encoderButtonMaster(28, 0, INPUT_PULLUP);  //encoder 0


//colors:
//on step color red:
//if note on: note on color, else: note off color
uint32_t red = 0x00FF00;  //neopixel color code position: ggrrbb, different than coventional color code:rrggbb
//channel-0 colors:
uint32_t color0a = 0x0000FF;  //blue
uint32_t color0b = 0x00FFFF;  //blue + red

//channel-1 colors:
uint32_t color1a = 0xFF0000;  // green
uint32_t color1b = 0xFFFF00;  // green+red


//channel-2 colors:
uint32_t color2a = 0xFF00FF;
uint32_t color2b = 0xFFFFFF;

// base color, no note played, not on current step, will be white
uint32_t emptyColor = 0x000000;  //

uint32_t channelNoteColor = 0x0000FF;
uint32_t currentStepColor = 0x00FFFF;

int previousStep;

// automatic control based on time
const int stepNumber = 8;
Stepper stepper(stepNumber, 350);    //stepper for smoothing the fader when we are pushing it, we call lock() to lock the pot value every 0.25 second
bool somethingJustDisplayed = true;  // a boolean for screen graph, once a displayReading() is called, it will return true
int timePerStep = 125;

int a;  // variable for pot reading

int groupStepIndex = 0;  // 0 for step 1-8, 1 for step 9-16
int note[3] = { 36, 38, 42 };

const int channelNumber = 3;  // how many drum channels
int channelDisplayed = 0;     // 0-1-2, 3 channels in total

bool buttonOn[channelNumber][stepNumber] = {
  { false, false, false, false, false, false, false, false },
  { false, false, false, false, false, false, false, false },
  { false, false, false, false, false, false, false, false },
};

int stepsPerGroup[3] = { 4, 4, 4 };
int indexUpTo;


void setup() {

  //Serial
  Serial.begin(9600);
  // Serial.println("Serial.begin()");
  stepper.stepHandler(onStep);


  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  //keyboard buttons
  for (int i = 0; i < 8; i++) {
    keyboardButtons[i]->pressHandler(notePress);
  }

  // // other buttons
  optionKeySwitch1.pressHandler(optionKeySwitch1Press);
  optionKeySwitch2.pressHandler(optionKeySwitch2Press);
  encoderButtonMaster.pressHandler(encoderButtonMasterPress);

  //encoder
  rotaryEncoderMaster.rightClickHandler(encoderMasterRightClick);
  rotaryEncoderMaster.leftClickHandler(encoderMasterLeftClick);

  //neopixels
  neopixel.begin();
  neopixel.clear();
  neopixel.setBrightness(50);
  neopixel.setPixelColor(0, 30, 30, 30);
  delay(125);
  neopixel.show();
  neopixel.setPixelColor(1, 60, 0, 15);
  delay(125);
  neopixel.show();
  neopixel.setPixelColor(2, 90, 0, 10);
  delay(125);
  neopixel.show();
  neopixel.setPixelColor(3, 120, 0, 5);
  delay(125);
  neopixel.show();
  neopixel.setPixelColor(4, 150, 0, 0);
  delay(125);
  neopixel.show();
  neopixel.setPixelColor(5, 180, 0, 0);
  delay(125);
  neopixel.show();
  neopixel.setPixelColor(6, 210, 0, 0);
  delay(125);
  neopixel.show();
  neopixel.setPixelColor(7, 240, 0, 0);
  delay(125);
  neopixel.show();
  delay(2000);
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

  stepper.stepReset();
}

void loop() {
  rotaryEncoderMaster.process();  //check step number for the sequence
  checkTimePerStep();             //modify time per setp by pushing the fader(pot)
  checkButtons();
  checkGroupSteps();
  stepper.process();
}

void encoderMasterLeftClick(int k) {
}

void encoderMasterRightClick(int k) {
  // if (stepNumber == maxStepNumber) {
  //   return;  // if stepNumber is at max in the range, stop excuting the mutator
  // }
  // allNotesOff();  //stop all currently sending midi notes;
  // int newStepNumber = stepNumber + 4;
  // newStepNumber = constrain(newStepNumber, minStepNumber, maxStepNumber);
  // //update the array
  // for (int i = 0; i < channelNumber; i++) {
  //   for (int j = stepNumber; j < newStepNumber; j++) {
  //     buttonOn[i][j] = false;
  //   }
  // }

  // //reset the stepper
  // stepper.stepChange(newStepNumber);
  // stepper.stepReset();
}

void allNotesOff() {
  for (int i = 0; i < channelNumber; i++) {
    for (int j = 0; j < stepNumber; j++) {
      if (buttonOn[i][j] == true) {
        usbMIDI.sendNoteOff(note[i], 0, 1);
      }
    }
  }
}

void allNeopixelOff() {
  for (int i = 0; i < 8; i++) {
    neopixel.setPixelColor(i, 0, 0, 0);
  }
}


void checkTimePerStep() {
  a = analogRead(A10);
  a = map(a, 0, 1023, 100, 1000);
  stepper.setTimePerStep(a);
}

void checkButtons() {
  for (int i = 0; i < 8; i++) {
    keyboardButtons[i]->process();
  }

  optionKeySwitch1.process();
  optionKeySwitch2.process();
  encoderButtonMaster.process();
}

void notePress(int num) {
  if (groupStepIndex == 1) {
    buttonOn[channelDisplayed][num + 8] = !buttonOn[channelDisplayed][num + 8];
  } else {
    buttonOn[channelDisplayed][num] = !buttonOn[channelDisplayed][num];
  }
}

void optionKeySwitch1Press(int num) {
}

void optionKeySwitch2Press(int num) {
  for (int i = 0; i < channelNumber; i++) {
    for (int j = 0; j < stepNumber; j++) {
      buttonOn[i][j] = false;
    }
  }
  allNeopixelOff();
  allNotesOff();
}


void encoderButtonMasterPress(int num) {
  channelDisplayed = channelDisplayed + 1;
  if (channelDisplayed == 3) {
    channelDisplayed = 0;
  }

  Serial.print("changed to channel of index: ");
  Serial.println(channelDisplayed);
  if (channelDisplayed == 0) {
    channelNoteColor = color0a;
    currentStepColor = color0b;
  }
  if (channelDisplayed == 1) {
    channelNoteColor = color1a;
    currentStepColor = color1b;
  }
  if (channelDisplayed == 2) {
    channelNoteColor = color2a;
    currentStepColor = color2b;
  }
  allNeopixelOff();
}


void onStep(int currentTimeStep) {  // on step, check current drum array, and send notes is it is true state

  int currentStep = currentTimeStep;
  if (currentStep >= stepsPerGroup[channelDisplayed]) {
    currentStep = currentTimeStep - stepsPerGroup[channelDisplayed];
  }


  previousStep = currentStep - 1;
  if (previousStep < 0) {
    previousStep = stepsPerGroup[channelDisplayed] - 1;
  }

  for (int i = 0; i < channelNumber; i++) {
    if (buttonOn[i][currentStep] == true) {
      usbMIDI.sendNoteOff(note[i], 0, 1);
      usbMIDI.sendNoteOn(note[i], 127, 1);
    } else {
      usbMIDI.sendNoteOff(note[i], 0, 1);
    }
  }

  if (buttonOn[channelDisplayed][currentStep] == true) {
    neopixel.setPixelColor(currentStep, currentStepColor);
  } else {
    neopixel.setPixelColor(currentStep, red);
  }

  if (buttonOn[channelDisplayed][previousStep] == true) {
    neopixel.setPixelColor(previousStep, channelNoteColor);
  } else {
    neopixel.setPixelColor(previousStep, emptyColor);
  }

  neopixel.show();
}
void checkGroupSteps() {
  for (int j = 0; j < 8; j++) {
    if (buttonOn[channelDisplayed][j] == true) {
      indexUpTo = j;
    }
  }
  int a = 3;
  if (indexUpTo > a) {
    stepsPerGroup[channelDisplayed] = 8;
  } else {
    stepsPerGroup[channelDisplayed] = 4;
    for (int i = 4; i < 8; i++) {
      neopixel.setPixelColor(i, 0, 0, 0);
    }
  }
}





// void onMaxStep() {
//   // displayReading(4, 0, 0, 0, 0);
// }





// void displayReading(byte mode, byte potNum, byte encoderDirection, int encoderValue, int noteName) {
//   if (mode == 1) {  // screen print pot value
//     display.clearDisplay();

//     display.setTextSize(1);
//     display.setTextColor(SSD1306_WHITE);
//     display.setCursor(0, 25);
//     display.println(F("pot Value reading: "));
//     int k = lastPotVals[potNum];
//     display.println(k);

//     display.display();
//     somethingJustDisplayed = true;
//   } else if (mode == 2) {  // screen print encoder value
//     display.clearDisplay();

//     display.setTextSize(1);
//     display.setTextColor(SSD1306_WHITE);
//     display.setCursor(0, 25);
//     display.println(F("encoder reading~~~ "));
//     display.print(F("Direction: "));
//     if (encoderDirection == 1) {
//       display.println(F("CW"));
//     } else if (encoderDirection == 2) {
//       display.println(F("ACW"));
//     } else if (encoderDirection == 3) {
//       display.println(F("click"));
//     }
//     display.print(F("Counter: "));
//     display.print(encoderValue);

//     display.display();
//     somethingJustDisplayed = true;
//   } else if (mode == 3) {  // screen print  value
//     // display.clearDisplay();

//     // display.setTextSize(1);
//     // display.setTextColor(SSD1306_WHITE);
//     // display.setCursor(0, 25);
//     // if (octaveCounter > 0) {
//     //   display.print(F("Octave up: +"));
//     //   display.print(octaveCounter);
//     // } else if (octaveCounter < 0) {
//     //   display.print(F("Octave down: "));
//     //   display.print(octaveCounter);

//     // } else if (octaveCounter == 0) {
//     //   display.println(F("Default Octave Position"));
//     // }

//     // display.display();
//     // somethingJustDisplayed = true;
//   } else if (mode == 4) {  // dispay something when there is no input
//     display.clearDisplay();

//     display.setTextSize(1);
//     display.setTextColor(SSD1306_WHITE);
//     display.setCursor(0, 28);
//     display.println(F("--waiting for input--"));
//     display.println(F("        (-_- )       "));

//     display.display();
//     somethingJustDisplayed = true;
//   } else if (mode == 5) {  //display midi note
//     display.clearDisplay();

//     display.setTextSize(1);
//     display.setTextColor(SSD1306_WHITE);
//     display.setCursor(0, 28);
//     display.print(F("  press note  "));
//     display.println(noteName);

//     display.display();
//     somethingJustDisplayed = true;
//   }
// }
