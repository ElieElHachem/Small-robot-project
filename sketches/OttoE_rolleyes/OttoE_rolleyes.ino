// Otto Eyes - emotional eyes: sympathy and love.
// Based on: OttoE_rolleyes (Otto DIY / Adafruit), adapted for the single
// 8x16 matrix of Otto Eyes (I2C address 0x70, SDA A4 / SCL A5).
// The first 8 rows = one eye, the next 8 = the other eye.
//
// Cycle: soft moving gaze -> tender wink -> smiling eyes
//        -> beating hearts -> "Metabolic Twin <3" -> hearts -> smile
//
// BSD license, all text above must be included in any redistribution.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_8x16matrix matrix = Adafruit_8x16matrix();

static const uint8_t PROGMEM // Bitmaps are stored in program memory
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
    B00000000,
    B00111100,
    B00000010,
    B00000010,
    B00000010,
    B00000010,
    B00111100,
    B00000000 },
  heartBig[8] = {      // Heart eye (from OttoE_ALLeyes)
    B00000000,
    B00001100,
    B00011110,
    B00111100,
    B00111100,
    B00011110,
    B00001100,
    B00000000 },
  heartSmall[8] = {    // Same heart, one size smaller (for the heartbeat)
    B00000000,
    B00000000,
    B00000100,
    B00001110,
    B00011100,
    B00001110,
    B00000100,
    B00000000 };

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
  randomSeed(analogRead(A0));
  matrix.begin(0x70);
  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
}

void loop() {
  lookAround(250);          // ~5 s of soft, moving gaze
  tenderBlink();
  showEyes(happyImg, 1500);
  heartbeat(3);
  scrollText("Metabolic Twin <3");
  heartbeat(2);
  showEyes(happyImg, 1500);
}

// Same image on both eyes
void showEyes(const uint8_t *img, uint16_t ms) {
  matrix.clear();
  matrix.drawBitmap(0, 0, img, 8, 8, LED_ON);
  matrix.drawBitmap(0, 8, img, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(ms);
}

// Slow, affectionate blink: eyes close gently, stay closed, reopen
void tenderBlink() {
  for (uint8_t i = 0; i < 5; i++) showEyes(blinkImg[i], 90);
  delay(400);
  for (int8_t i = 4; i >= 0; i--) showEyes(blinkImg[i], 90);
}

// "Ba-dum" heartbeat with the heart eyes
void heartbeat(uint8_t beats) {
  while (beats--) {
    showEyes(heartBig, 150);
    showEyes(heartSmall, 120);
    showEyes(heartBig, 150);
    showEyes(heartSmall, 500);
  }
  showEyes(heartBig, 800);
}

void scrollText(const char *txt) {
  matrix.setRotation(1);  // horizontal 16x8 for text
  int16_t width = strlen(txt) * 6;  // 6 px per character in the default font
  for (int16_t x = 16; x >= -width; x--) {
    matrix.clear();
    matrix.setCursor(x, 0);
    matrix.print(txt);
    matrix.writeDisplay();
    delay(70);
  }
  matrix.setRotation(0);
}

// Original rolleyes animation: blinking eyes with a moving pupil, ~50 FPS
void lookAround(uint16_t frames) {
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
        gazeFrames    = random(3, 15);           // Duration of eye movement
        gazeCountdown = random(gazeFrames, 120); // Count to end of next movement
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
