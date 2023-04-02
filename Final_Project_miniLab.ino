int buttonPin = 32;
#define outputA 38
#define outputB 39
#define encoderClick 40

int counter = 0;
int currentStateA;
int lastStateA;
String currentDir = "";

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(outputA, INPUT);
  pinMode(outputB, INPUT);
  pinMode(encoderClick, INPUT_PULLUP);

  Serial.begin(9600);
  lastStateA = digitalRead(outputA);
  // put your setup code here, to run once:
}

void checkEncoder() {
  if (digitalRead(encoderClick) == LOW) {
    Serial.println("click pressed");
  }

  // Read the current state of outputA
  currentStateA = digitalRead(outputA);


  // If last and current state of outputA are different, then pulse occurred
  // React to only 1 state change to avoid double count
  if (currentStateA != lastStateA && currentStateA == 1) {

    // If the outputB state is different than the outputA state then
    // the encoder is rotating CCW so decrement
    if (digitalRead(outputB) != currentStateA) {
      counter++;
      currentDir = "CW";
    } else {
      // Encoder is rotating CW so increment
      counter--;
      currentDir = "ACW";
    }

    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter: ");
    Serial.println(counter);
  }

  // Remember last outputA state
  lastStateA = currentStateA;

  // Put in a slight delay to help debounce the reading
  delay(1);
}
}

void checkMechnicalSwitch() {
  if (digitalRead(buttonPin) == LOW) {
    Serial.println("buttonPressed");
  } else if (digitalRead(buttonPin) == HIGH) {
  }
}

void loop() {
  checkMechnicalSwitch();
  // put your main code here, to run repeatedly:
}
