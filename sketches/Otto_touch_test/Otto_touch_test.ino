// Otto - touch sensor test on A0.
// Touch the sensor: the whole screen lights up, and the Serial Monitor (9600) prints "TOUCHED".
// Release: the screen shows a small dot, and the monitor prints "released".

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_8x16matrix matrix = Adafruit_8x16matrix();
const int TOUCH_PIN = A0;
int last = -1;

void setup() {
  Serial.begin(9600);
  pinMode(TOUCH_PIN, INPUT);
  matrix.begin(0x70);
  Serial.println(F("Touch sensor test (A0): touch it!"));
}

void loop() {
  int state = digitalRead(TOUCH_PIN);
  if (state != last) {  // only react when it changes
    last = state;
    matrix.clear();
    if (state == HIGH) matrix.fillRect(0, 0, 8, 16, LED_ON);
    else matrix.fillRect(3, 7, 2, 2, LED_ON);
    matrix.writeDisplay();
    Serial.println(state == HIGH ? F("TOUCHED") : F("released"));
  }
  delay(20);
}
