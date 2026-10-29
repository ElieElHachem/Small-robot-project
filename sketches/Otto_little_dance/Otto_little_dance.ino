// Otto - a little dance to check that all 4 servos work.
// Uses the trims saved with Otto_calibration_test.
// The dance plays once (~40 s), then Otto rests: press the Nano's RESET button to replay it.

#include <Servo.h>
#include <Oscillator.h>
#include <US.h>
#include <Otto9.h>

Otto9 Otto;

void setup() {
  Otto.init(2, 3, 4, 5, true, A6, 13, 8, 9);  // true = load the saved calibration
  Otto.home();
  delay(5000);  // time to put Otto on the floor

  Otto.sing(S_connection);
  Otto.swing(2, 1000, 20);               // sways
  Otto.tiptoeSwing(2, 900, 20);          // on tiptoes
  Otto.moonwalker(2, 1000, 25, LEFT);    // moonwalk to the left...
  Otto.moonwalker(2, 1000, 25, RIGHT);   // ...and to the right
  Otto.sing(S_happy);
  Otto.crusaito(2, 1000, 20, FORWARD);   // side step
  Otto.updown(2, 1000, 20);              // up / down
  Otto.shakeLeg(1, 1500, LEFT);          // shakes one leg...
  Otto.shakeLeg(1, 1500, RIGHT);         // ...then the other
  Otto.jitter(4, 500, 20);               // little shivers
  Otto.flapping(2, 1000, 20, FORWARD);   // flaps his feet
  Otto.bend(1, 1400, LEFT);              // leans over
  Otto.bend(1, 1400, RIGHT);
  Otto.ascendingTurn(2, 900, 20);
  Otto.jump(1, 1000);                    // finale!
  Otto.sing(S_superHappy);
  Otto.home();
}

void loop() {}
