#include "math.h"

#define RED_BUTTON 2
#define GREEN_BUTTON 4

bool buttonRedPressed = false;
bool buttonRedReleased = false;
long btnRedStartTime = 0;
long btnRedEndTime = 0;
bool buttonGreenPressed = false;
bool buttonGreenReleased = false;
long btnGreenStartTime = 0;
long btnGreenEndTime = 0;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode (RED_BUTTON, INPUT);
}

void printTime(long millis) {
  long remainingMillis = millis % 1000;

  long allSeconds = floor(millis / 1000);

  int hours = allSeconds / 3600;

  int minutes = (allSeconds - (3600 * hours)) / 60;

  int seconds = (allSeconds - (3600 * hours) - (60 * minutes));

  char data[100];
  sprintf(data, "%d:%d:%d.%d", hours, minutes, seconds, remainingMillis);

  Serial.print(data);
}

void loop() {
  if (digitalRead(RED_BUTTON) == HIGH) {
    buttonRedReleased = false;
    if (!buttonRedPressed) {
      Serial.print("\nRed button pressed: ");
      btnRedStartTime = millis();
      Serial.print(btnRedStartTime);
      buttonRedPressed = true;
    }
  } else {
    buttonRedPressed = false;
    if (!buttonRedReleased && btnRedStartTime != 0) {
      Serial.print("\n Red button released: ");
      btnRedEndTime = millis();
      Serial.print(btnRedEndTime);
      buttonRedReleased = true;
      Serial.print("\nRed button time: ");
      printTime(btnRedEndTime - btnRedStartTime);
      btnRedStartTime = 0;
      btnRedEndTime = 0;
    }
  }

  if (digitalRead(GREEN_BUTTON) == HIGH) {
    buttonGreenReleased = false;
    if (!buttonGreenPressed) {
      Serial.print("\nGreen button pressed: ");
      btnGreenStartTime = millis();
      Serial.print(btnGreenStartTime);
      buttonGreenPressed = true;
    }
  } else {
    buttonGreenPressed = false;
    if (!buttonGreenReleased && btnGreenStartTime != 0) {
      Serial.print("\nGreen button released: ");
      btnGreenEndTime = millis();
      Serial.print(btnGreenEndTime);
      buttonGreenReleased = true;
      Serial.print("\nGreen button time: ");
      printTime(btnGreenEndTime - btnGreenStartTime);
      btnGreenStartTime = 0;
      btnGreenEndTime = 0;
    }
  }
}
