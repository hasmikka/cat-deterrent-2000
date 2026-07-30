#include <Arduino.h>

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  digitalWrite(9, HIGH);
  Serial.println("Cat Deterrent 2000 is alive");
  delay(500);

  digitalWrite(9, LOW);
  delay(500);
}

