// Otto - Happy Birthday, with human-like eyes (like the Halloween sketch).
// 1) Wakes up happy and walks up to the person in front of him.
// 2) "Happy Birthday" scrolls, he jumps, a party popper bursts, happy sounds.
// 3) Then, eyes full of energy: he looks everywhere as if following
//    the conversation, smiles, blinks, approves... in a loop, without moving.

#include <Servo.h>
#include <Oscillator.h>
#include <US.h>
#include <Otto9.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Otto9 Otto;
Adafruit_8x16matrix matrix = Adafruit_8x16matrix();

// ---- Eyes, one per 8x8 half (rotation 0, same as OttoE_halloween) ----------
static const uint8_t PROGMEM
  blinkImg[][8] = {    // Eye animation frames
  { B00000000,         // Fully open eye
    B00111100,
    B01111110,
    B01111110,
    B01111110,
    B01111110,
    B00111100,
    B00000000 },
  { B00011000,
    B00011000,
    B00111110,
    B01111110,
    B01111110,
    B00111110,
    B00111100,
    B00011000 },
  { B00011000,
    B00011000,
    B00011100,
    B00111100,
    B00111100,
    B00011100,
    B00011000,
    B00011000 },
  { B00001000,
    B00011000,
    B00001100,
    B00001100,
    B00001100,
    B00001100,
    B00011000,
    B00001000 },
  { B00001000,         // Fully closed eye
    B00000100,
    B00000100,
    B00000100,
    B00000100,
    B00000100,
    B00000100,
    B00001000 } },
  happyImg[8] = {      // Smiling eye (from OttoE_ALLeyes)
    B00000000, B00111100, B00000010, B00000010,
    B00000010, B00000010, B00111100, B00000000 };

uint8_t
  blinkIndex[] = { 1, 2, 3, 4, 3, 2, 1 }, // Blink bitmap sequence
  blinkCountdown = 100, // Countdown to next blink (in frames)
  gazeCountdown  =  75, // Countdown to next eye movement
  gazeFrames     =  50; // Duration of eye movement (smaller = faster)
int8_t
  eyeX = 3, eyeY = 3,   // Current eye position
  newX = 3, newY = 3,   // Next eye position
  dX   = 0, dY   = 0;   // Distance from prior to new position

void setup() {
  Otto.init(2, 3, 4, 5, true, A6, 13, 8, 9);  // true = load the saved calibration
  Otto.home();
  randomSeed(analogRead(A1));
  matrix.begin(0x70);
  matrix.setTextWrap(false);
  matrix.setTextColor(LED_ON);

  lookAround(250);  // ~5 s: time to put him down, he looks around

  // He wakes up all happy
  showEyes(blinkImg[0], 400);  // wide eyes: oh!
  Otto.sing(S_connection);
  showEyes(happyImg, 800);

  // He walks up to the person in front (~8 s), eyes straight ahead
  showGaze(3, 3, 0);
  Otto.walk(8, 1000, FORWARD);
  Otto.home();

  // Arrives in front of them: little sound, smile, small hop
  Otto.sing(S_cuddly);
  showEyes(happyImg, 0);
  Otto.updown(1, 900, 10);
  Otto.home();
  slowBlink();

  // The message, then he jumps vigorously, party popper and joy
  scrollText("Happy Birthday");
  showEyes(happyImg, 0);
  Otto.sing(S_surprise);
  Otto.jump(1, 500);
  Otto.jump(1, 500);
  Otto.home();
  popper();
  showEyes(happyImg, 0);
  Otto.sing(S_superHappy);
  Otto.sing(S_happy);
  slowBlink();
}

// Every AGREE_EVERY_S seconds he "approves" the conversation (little sound + expression).
// Use 45UL * 60 for every 45 minutes.
const unsigned long AGREE_EVERY_S = 45;

// Full of energy and happy: he looks everywhere as if following the conversation
void loop() {
  static unsigned long lastAgree = millis();
  if (millis() - lastAgree >= AGREE_EVERY_S * 1000UL) {
    lastAgree = millis();
    agree();
  }
  lookAround(random(150, 300));
  switch (random(4)) {
    case 0: listening(); break;
    case 1: showEyes(happyImg, 1500); slowBlink(); break;
    case 2: quickBlink(); quickBlink(); break;  // cheerful double blink
    default: break;                             // just keep looking around
  }
}

// ---- Eye helpers (same as OttoE_halloween) -----------------------------------

// Same 8x8 image on both eyes
void showEyes(const uint8_t *img, uint16_t ms) {
  matrix.setRotation(0);
  matrix.clear();
  matrix.drawBitmap(0, 0, img, 8, 8, LED_ON);
  matrix.drawBitmap(0, 8, img, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(ms);
}

// Open eyes with the pupil held at (px, py)
void showGaze(int8_t px, int8_t py, uint16_t ms) {
  matrix.setRotation(0);
  matrix.clear();
  matrix.drawBitmap(0, 0, blinkImg[0], 8, 8, LED_ON);
  matrix.drawBitmap(0, 8, blinkImg[0], 8, 8, LED_ON);
  matrix.fillRect(px, py, 2, 2, LED_OFF);
  matrix.fillRect(px, 8 + py, 2, 2, LED_OFF);
  matrix.writeDisplay();
  delay(ms);
}

// Slow blink: eyes close gently, stay closed, reopen
void slowBlink() {
  for (uint8_t i = 0; i < 5; i++) showEyes(blinkImg[i], 90);
  delay(300);
  for (int8_t i = 4; i >= 0; i--) showEyes(blinkImg[i], 90);
}

// Quick blink
void quickBlink() {
  showEyes(blinkImg[2], 40);
  showEyes(blinkImg[4], 60);
  showEyes(blinkImg[2], 40);
}

// Follows a conversation: looks at one person, then the other, then back
void listening() {
  showGaze(3, 1, random(1000, 2000));
  quickBlink();
  showGaze(3, 5, random(1000, 2000));
  showGaze(3, 3, 500);
  showEyes(happyImg, 900);
}

// "Approves" what is being said, with a little sound and matching eyes
void agree() {
  switch (random(3)) {
    case 0:  // "Yes!" : two rising notes and a big smile
      showEyes(happyImg, 0);
      Otto._tone(1318, 90, 30);
      Otto._tone(1760, 150, 0);
      delay(1200);
      break;
    case 1:  // "Mm-hmm, agreed" : low-high hum while the eyes nod
      showGaze(5, 3, 0);
      Otto._tone(880, 120, 40);
      showGaze(1, 3, 0);
      Otto._tone(1175, 160, 0);
      showGaze(3, 3, 300);
      showEyes(happyImg, 900);
      break;
    default:  // "Ooh, interesting!" : wide eyes and a rising whistle
      showEyes(blinkImg[0], 0);
      Otto.bendTones(800, 1800, 1.03, 10, 5);
      delay(800);
      quickBlink();
      break;
  }
}

void scrollText(const char *txt) {
  matrix.setRotation(1);  // horizontal 16x8 for text
  int16_t width = strlen(txt) * 6;  // 6 px per character in the default font
  for (int16_t x = 16; x >= -width; x--) {
    matrix.clear();
    matrix.setCursor(x, 0);
    matrix.print(txt);
    matrix.writeDisplay();
    delay(60);
  }
  matrix.setRotation(0);
}

// Party popper: a cone in the bottom-left corner bursts confetti toward the top-right
void popper() {
  static const int8_t vx[] = { 2, 3, 4, 3, 5, 4, 2, 5, 3, 4 };   // confetti speeds
  static const int8_t vy[] = { -3, -2, -1, -3, -2, 0, -1, -1, 0, -2 };
  matrix.setRotation(1);
  for (uint8_t t = 0; t <= 6; t++) {  // the burst
    matrix.clear();
    matrix.fillTriangle(0, 7, 2, 2, 5, 5, LED_ON);
    for (uint8_t k = 0; k < 10; k++) {
      matrix.drawPixel(5 + vx[k] * t / 2, 3 + vy[k] * t / 2, LED_ON);
    }
    matrix.writeDisplay();
    delay(110);
  }
  for (uint8_t f = 0; f < 15; f++) {  // confetti twinkling where they landed
    matrix.clear();
    matrix.fillTriangle(0, 7, 2, 2, 5, 5, LED_ON);
    for (uint8_t k = 0; k < 10; k++) {
      if (random(3)) matrix.drawPixel(5 + vx[k] * 3, 3 + vy[k] * 3, LED_ON);
    }
    matrix.writeDisplay();
    delay(120);
  }
  matrix.setRotation(0);
}

// Rolleyes animation: blinking eyes with a moving pupil, ~50 FPS.
// Gaze changes more often than in Halloween: he's curious about everything.
void lookAround(uint16_t frames) {
  matrix.setRotation(0);
  while (frames--) {
    matrix.clear();
    const uint8_t *eye = blinkImg[
      (blinkCountdown < sizeof(blinkIndex)) ? // Currently blinking?
      blinkIndex[blinkCountdown] :            // Yes, look up bitmap #
      0                                       // No, show bitmap 0
    ];
    matrix.drawBitmap(0, 0, eye, 8, 8, LED_ON);
    matrix.drawBitmap(0, 8, eye, 8, 8, LED_ON);
    // Decrement blink counter.  At end, set random time for next blink.
    if (--blinkCountdown == 0) blinkCountdown = random(5, 180);

    // Add a pupil (2x2 black square) atop the blinky eyeball bitmap.
    if (--gazeCountdown <= gazeFrames) {
      // Eyes are in motion - draw pupil at interim position
      int8_t px = newX - (dX * gazeCountdown / gazeFrames);
      int8_t py = newY - (dY * gazeCountdown / gazeFrames);
      matrix.fillRect(px, py, 2, 2, LED_OFF);
      matrix.fillRect(px, 8 + py, 2, 2, LED_OFF);
      if (gazeCountdown == 0) {   // Last frame?
        eyeX = newX; eyeY = newY; // Yes.  What's new is old, then...
        do { // Pick random positions until one is within the eye circle
          newX = random(7); newY = random(7);
          dX   = newX - 3;  dY   = newY - 3;
        } while ((dX * dX + dY * dY) >= 10);     // Thank you Pythagoras
        dX            = newX - eyeX;             // Horizontal distance to move
        dY            = newY - eyeY;             // Vertical distance to move
        gazeFrames    = random(3, 12);           // Duration of eye movement
        gazeCountdown = random(gazeFrames, 60);  // curious: moves twice as often
      }
    } else {
      // Not in motion yet -- draw pupil at current static position
      matrix.fillRect(eyeX, eyeY, 2, 2, LED_OFF);
      matrix.fillRect(eyeX, 8 + eyeY, 2, 2, LED_OFF);
    }

    matrix.writeDisplay();
    delay(20); // ~50 FPS
  }
}
