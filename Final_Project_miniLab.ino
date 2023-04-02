int buttonPin = 32;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  // put your setup code here, to run once:
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
