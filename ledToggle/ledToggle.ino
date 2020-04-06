#define BUTTON 2
#define LED 3

void setup() {
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);
}

// In case you press & hold the button it'll return false
bool checkButtonPressed(int pin, bool* buttonPressedPreviously) {
  if (digitalRead(pin) == HIGH) {
    if (!*buttonPressedPreviously) {
      *buttonPressedPreviously = true;
      return true;
    }

    return false;
  }
  
  *buttonPressedPreviously = false;
  return false;
}


bool buttonPressedPreviously = false;
int ledState = LOW;
void loop() {
  if(checkButtonPressed(BUTTON, &buttonPressedPreviously)) {
    ledState = ledState == LOW ? HIGH : LOW;
    digitalWrite(LED, ledState);
  }

  delay(50);
}
