// Otto - zero the servos, then a simple dance.
// 1) All 4 servos go to 90 degrees WITHOUT trims and are held for 15 s:
//    check that legs and feet are straight, otherwise re-mount the servo horns (battery off).
// 2) A simple, slow dance, then rest. Press the Nano's RESET button to start again.
// The saved calibration (EEPROM) is not erased, just ignored here.

#include <Servo.h>
#include <Oscillator.h>
#include <US.h>
#include <Otto9.h>

Otto9 Otto;

void setup() {
  Otto.init(2, 3, 4, 5, false, A6, 13, 8, 9);  // false = no trims, true zero (90 degrees)

  int center[4] = { 90, 90, 90, 90 };
  Otto._moveServos(500, center);  // servos held at 90 degrees
  Otto.sing(S_connection);
  delay(15000);                   // 15 s to check / put Otto on the floor

  Otto.sing(S_happy);
  Otto.swing(4, 1500, 15);              // gentle sway
  Otto.updown(3, 1500, 15);             // up / down
  Otto.moonwalker(2, 1500, 20, LEFT);   // moonwalk to the left
  Otto.moonwalker(2, 1500, 20, RIGHT);  // and to the right
  Otto.swing(2, 1500, 15);
  Otto.sing(S_superHappy);
  Otto.home();
}

void loop() {}
