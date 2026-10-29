// Otto - servo calibration + movement test, through the Serial Monitor (9600 baud).
// Classic Otto pinout: left leg 2, right leg 3, left foot 4, right foot 5.
//
// Keys (then Enter):
//   q / a : left leg   +1 / -1      w / s : right leg  +1 / -1
//   e / d : left foot  +1 / -1      r / f : right foot +1 / -1
//   v : save the trims (EEPROM)     t : movement test
//   p : print the trims             ? : help
//   z : reset all 4 trims to zero (not saved until you press v)
// Goal: legs straight and feet flat, Otto standing perfectly upright.

#include <Servo.h>
#include <Oscillator.h>
#include <US.h>
#include <Otto9.h>
#include <EEPROM.h>

Otto9 Otto;
int trim[4];
const char *names[4] = { "Left leg (2)", "Right leg (3)", "Left foot (4)", "Right foot (5)" };

void setup() {
  Serial.begin(9600);
  Otto.init(2, 3, 4, 5, true, A6, 13, 8, 9);  // A6 = free pin (no noise sensor)
  for (int i = 0; i < 4; i++) {
    trim[i] = (int8_t)EEPROM.read(i);
    if (abs(trim[i]) > 40) trim[i] = 0;  // never-written EEPROM = nonsense values
  }
  applyTrims();
  help();
}

void loop() {
  if (!Serial.available()) return;
  char c = Serial.read();
  const char *up = "qwer", *down = "asdf";
  for (int i = 0; i < 4; i++) {
    if (c == up[i])   { trim[i] = min(trim[i] + 1, 40);  applyTrims(); }
    if (c == down[i]) { trim[i] = max(trim[i] - 1, -40); applyTrims(); }
  }
  if (c == 'v') { Otto.saveTrimsOnEEPROM(); Serial.println(F(">> Trims saved! Every Otto sketch will load them.")); }
  if (c == 't') testMoves();
  if (c == 'z') { for (int i = 0; i < 4; i++) trim[i] = 0; applyTrims(); }  // all to zero
  if (c == 'p') printTrims();
  if (c == '?') help();
}

// Applies the trims and puts the 4 servos back to 90 degrees (+ trim)
void applyTrims() {
  Otto.setTrims(trim[0], trim[1], trim[2], trim[3]);
  int center[4] = { 90, 90, 90, 90 };
  Otto._moveServos(300, center);
  printTrims();
}

void printTrims() {
  for (int i = 0; i < 4; i++) {
    Serial.print(names[i]); Serial.print(F(": ")); Serial.print(trim[i]); Serial.print(i < 3 ? F("   ") : F("\n"));
  }
}

void testMoves() {
  Serial.println(F(">> Test: walk forward")); Otto.walk(4, 1000, FORWARD);
  Serial.println(F(">> Test: walk backward")); Otto.walk(4, 1000, BACKWARD);
  Serial.println(F(">> Test: turn left")); Otto.turn(3, 2000, LEFT);
  Serial.println(F(">> Test: turn right")); Otto.turn(3, 2000, RIGHT);
  Serial.println(F(">> Test: up / down")); Otto.updown(2, 1500, 20);
  Serial.println(F(">> Test: swing")); Otto.swing(2, 1000, 20);
  Serial.println(F(">> Test: moonwalk")); Otto.moonwalker(2, 1000, 25, LEFT);
  Serial.println(F(">> Test: tiptoes")); Otto.tiptoeSwing(2, 900, 20);
  applyTrims();  // back to standing straight
  Serial.println(F(">> Test done"));
}

void help() {
  Serial.println(F("\n=== Otto calibration ==="));
  Serial.println(F("q/a left leg  +/-   w/s right leg  +/-"));
  Serial.println(F("e/d left foot +/-   r/f right foot +/-"));
  Serial.println(F("v save   t test the moves   p print   z all to zero   ? help"));
}
