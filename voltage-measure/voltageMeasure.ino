#define VOLTAGE_MEASURE_PIN A1

void setup() {
  Serial.begin(9600);
}

char measureResultsBuffer[30];
void printResults(int pin, float voltage, int sensorValue) {
  String vlotageStr = String(voltage);
  sprintf(measureResultsBuffer, "\nMiernik A%d\nV=%s  ADC=%d", pin, vlotageStr.c_str(), sensorValue);

  Serial.print(measureResultsBuffer);
}


int sensorValue;
float voltage;
void loop() {
  sensorValue = analogRead(VOLTAGE_MEASURE_PIN);
  voltage =  sensorValue * (5.0 / 1023.0);
  printResults(1, voltage, sensorValue);
  delay(50);
}
