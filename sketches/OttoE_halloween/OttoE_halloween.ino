// Otto Eyes - Halloween run: a running buddy on your shoulder.
// Speed, cat, witch, magic book, ghosts, spooky, forest, ritual...
// bombastic side eye between animations, and "Courage, go go go!" every 15 min.
// Based on: OttoE_rolleyes (Otto DIY / Adafruit), for the single 8x16 matrix
// of Otto Eyes (I2C 0x70, SDA A4 / SCL A5).
//
// BSD license, all text above must be included in any redistribution.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_8x16matrix matrix = Adafruit_8x16matrix();

// true  = demo: every animation in a row, in order
// false = run: random animations
const bool DEMO = false;


// ---- Eyes, one per 8x8 half (rotation 0, same as rolleyes) -----------------
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

// ---- Full faces, 16 wide x 8 tall (rotation 1, left eye = left half) -------
static const uint8_t PROGMEM
  batUp[16] = {        // Bat, wings up (glowing eyes = holes in the head)
    B10000000, B00000001,
    B11000010, B01000011,
    B11100010, B01000111,
    B11111111, B11111111,
    B01111111, B11111110,
    B00100111, B11100100,
    B00000011, B11000000,
    B00000001, B10000000 },
  batDown[16] = {      // Bat, wings down
    B00000000, B00000000,
    B00000010, B01000000,
    B00011110, B01111000,
    B01111111, B11111110,
    B11111111, B11111111,
    B11000111, B11100011,
    B10000011, B11000001,
    B00000001, B10000000 },
  catEyes[16] = {      // Black cat: almond eyes, vertical slit pupils
    B00000000, B00000000,
    B00111100, B00111100,
    B01110110, B01110110,
    B11110110, B11110110,
    B11110110, B11110110,
    B01110110, B01110110,
    B00111100, B00111100,
    B00000000, B00000000 },
  catSquint[16] = {    // Same cat eyes, narrowed: locked on target
    B00000000, B00000000,
    B00000000, B00000000,
    B00000000, B00000000,
    B01110110, B01110110,
    B11110110, B11110110,
    B00111100, B00111100,
    B00000000, B00000000,
    B00000000, B00000000 },
  spookyEyes[16] = {   // Slanted evil eyes
    B11000000, B00000011,
    B11110000, B00001111,
    B01111100, B00111110,
    B00111110, B01111100,
    B00011100, B00111000,
    B00000000, B00000000,
    B00000000, B00000000,
    B00000000, B00000000 },
  speedEyes[16] = {    // Narrowed eyes, pupils pushed to the side
    B00000000, B00000000,
    B00000000, B00000000,
    B00000000, B00000000,
    B01111110, B01111110,
    B01110010, B01110010,
    B00111100, B00111100,
    B00000000, B00000000,
    B00000000, B00000000 },
  witchImg[16] = {     // Witch on her broom, flying left: big hat, cape, bristles
    B00000110, B00000000,
    B00001110, B00000000,
    B00011111, B00000000,
    B01111111, B11000000,
    B00001111, B00000000,
    B00011111, B10001110,
    B11111111, B11111111,
    B00000110, B00011111 },
  bookClosed[16] = {   // Spell book, cover with a glowing star
    B00000000, B00000000,
    B00001111, B11110000,
    B00001000, B00010000,
    B00001001, B10010000,
    B00001011, B11010000,
    B00001001, B10010000,
    B00001000, B00010000,
    B00001111, B11110000 },
  bookOpen[16] = {     // Open book at the bottom, room above for the magic
    B00000000, B00000000,
    B00000000, B00000000,
    B00000000, B00000000,
    B00000000, B00000000,
    B00111100, B00111100,
    B01000010, B01000010,
    B10000001, B10000001,
    B01111111, B11111110 },
  spiralA[16] = {      // Hypnotized spiral eyes (spell!), frame A
    B11111110, B01111111,
    B00000010, B00000001,
    B11111010, B01111101,
    B10001010, B01000101,
    B10111010, B01011101,
    B10000010, B01000001,
    B11111110, B01111111,
    B00000000, B00000000 },
  spiralB[16] = {      // Mirrored spiral, alternating makes it spin
    B11111110, B01111111,
    B10000000, B01000000,
    B10111110, B01011111,
    B10100010, B01010001,
    B10111010, B01011101,
    B10000010, B01000001,
    B11111110, B01111111,
    B00000000, B00000000 },
  booText[16] = {      // "BOO" in a tiny font so it fits the screen at once
    B00000000, B00000000,
    B01110001, B10001100,
    B01001010, B01010010,
    B01110010, B01010010,
    B01001010, B01010010,
    B01110001, B10001100,
    B00000000, B00000000,
    B00000000, B00000000 };

static const uint8_t PROGMEM  // Ghost, 8x8, two frames for a wavy tail
  ghostImg[][8] = {
  { B00111100,
    B01111110,
    B11011011,
    B11011011,
    B11111111,
    B11111111,
    B10101010,
    B00000000 },
  { B00111100,
    B01111110,
    B11011011,
    B11011011,
    B11111111,
    B11111111,
    B01010101,
    B00000000 } },
  pineBig[8] = {       // Big pine tree, 5x8
    B00100000,
    B01110000,
    B11111000,
    B01110000,
    B11111000,
    B11111000,
    B00100000,
    B00100000 },
  objectImg[][8] = {   // What the magic book can conjure (8x8, rotation 1)
  { B00011000,         // Potion bottle
    B00011000,
    B00100100,
    B01000010,
    B10111101,
    B11111111,
    B11111111,
    B01111110 },
  { B01100110,         // Frog (the classic spell result!)
    B10011001,
    B01111110,
    B11111111,
    B10000001,
    B01111110,
    B00100100,
    B00000000 },
  { B00111100,         // Crystal ball on its stand
    B01000010,
    B10010001,
    B10000001,
    B01000010,
    B00111100,
    B01111110,
    B11111111 },
  { B00111100,         // Mushroom
    B01101110,
    B11111011,
    B11111111,
    B00100100,
    B00100100,
    B00111100,
    B00000000 },
  { B00000000,         // Crown
    B10011001,
    B11011011,
    B11111111,
    B11111111,
    B00000000,
    B11111111,
    B00000000 } },
  spiderImg[][5] = {   // Small 5x5 spider, two frames so the legs wiggle
  { B10001000,
    B01110000,
    B11111000,
    B01110000,
    B10001000 },
  { B10001000,
    B11111000,
    B01110000,
    B11111000,
    B10001000 } };

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
  randomSeed(analogRead(A1));  // unused analog pin = random seed
  matrix.begin(0x70);
  bats();  // a bat right at power-on
}

// "Courage, go go go!" cheer every 15 minutes of running
// (every minute in demo mode, so you can hear it).
const unsigned long CHEER_EVERY_MIN = DEMO ? 1 : 15;
const int BUZZER_PIN = 13;

void loop() {
  static unsigned long lastCheer = millis();
  if (millis() - lastCheer >= CHEER_EVERY_MIN * 60000UL) {
    lastCheer = millis();
    cheer();
  }
  // Pause: "normal" Otto (moving gaze) between two animations,
  // sometimes with a bombastic side eye in the middle (1 pause out of 4)
  uint16_t pause = DEMO ? 300 : random(500, 1000);  // demo 6 s, run 10 to 20 s
  lookAround(pause / 2);
  if (!DEMO && random(4) == 0) sideEye();
  lookAround(pause / 2);

  // DEMO: each emotion once, in this order (31 = side eye, shown once in the demo)
  static const uint8_t demoOrder[] = { 19, 16, 0, 7, 10, 27, 21, 4, 23, 25, 13, 29, 31 };
  static uint8_t demoStep = 0;
  uint8_t pick = DEMO ? demoOrder[demoStep] : random(31);
  if (DEMO) demoStep = (demoStep + 1) % sizeof(demoOrder);

  switch (pick) {
    // Most frequent: speed (4/31)
    case 0: case 1: case 2: case 3: speed(); break;
    // Then: witch spell, magic book, cat, spooky, ghosts (3/31 each)
    case 4: case 5: case 6: witch(); break;
    case 7: case 8: case 9: book(); break;
    case 10: case 11: case 12: cat(); break;
    case 13: case 14: case 15: spooky(); break;
    case 16: case 17: case 18: ghosts(); break;
    // Rarer: everything else (2/31 each)
    case 19: case 20: bats(); break;
    case 21: case 22: content(); break;
    case 23: case 24: forest(); break;
    case 25: case 26: occult(); break;
    case 27: case 28: spider(); break;
    case 29: case 30: darting(); break;
    case 31: sideEye(); break;  // demo only
  }
}

// ---- Drawing helpers --------------------------------------------------------

// Same 8x8 image on both eyes
void showEyes(const uint8_t *img, uint16_t ms) {
  matrix.clear();
  matrix.drawBitmap(0, 0, img, 8, 8, LED_ON);
  matrix.drawBitmap(0, 8, img, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(ms);
}

// Full 16x8 face at offset (x, y)
void showFace(const uint8_t *img, int8_t x, int8_t y, uint16_t ms) {
  matrix.setRotation(1);
  matrix.clear();
  matrix.drawBitmap(x, y, img, 16, 8, LED_ON);
  matrix.writeDisplay();
  matrix.setRotation(0);
  delay(ms);
}

// Open eyes with the pupil held at (px, py)
void showGaze(int8_t px, int8_t py, uint16_t ms) {
  matrix.clear();
  matrix.drawBitmap(0, 0, blinkImg[0], 8, 8, LED_ON);
  matrix.drawBitmap(0, 8, blinkImg[0], 8, 8, LED_ON);
  matrix.fillRect(px, py, 2, 2, LED_OFF);
  matrix.fillRect(px, 8 + py, 2, 2, LED_OFF);
  matrix.writeDisplay();
  delay(ms);
}

// "Cou-ra-ge ! Go go go !" : three firm notes, then three quick rising ones,
// with speed eyes for the "go go go"
void cheer() {
  static const int courage[] = { 784, 988, 1175 };  // Cou - ra - ge !
  static const int gogogo[] = { 1319, 1568, 2093 }; // go  - go - go !
  showEyes(happyImg, 0);
  for (uint8_t i = 0; i < 3; i++) { tone(BUZZER_PIN, courage[i], 180); delay(230); }
  delay(250);
  showFace(speedEyes, 0, 0, 0);
  for (uint8_t i = 0; i < 3; i++) { tone(BUZZER_PIN, gogogo[i], 110); delay(150); }
  delay(400);
  speed();  // and off he goes!
}

// Slow blink: eyes close gently, stay closed, reopen
void slowBlink() {
  for (uint8_t i = 0; i < 5; i++) showEyes(blinkImg[i], 90);
  delay(400);
  for (int8_t i = 4; i >= 0; i--) showEyes(blinkImg[i], 90);
}

// Quick blink
void quickBlink() {
  showEyes(blinkImg[2], 40);
  showEyes(blinkImg[4], 60);
  showEyes(blinkImg[2], 40);
}

// ---- Run emotions -------------------------------------------------------------

// Speed: narrowed eyes with wind streaks rushing past
void speed() {
  for (uint8_t f = 0; f < 220; f++) {  // ~9 s
    matrix.setRotation(1);
    matrix.clear();
    matrix.drawBitmap(0, 0, speedEyes, 16, 8, LED_ON);
    matrix.drawFastHLine(15 - (f * 3) % 20, 0, 3, LED_ON);
    matrix.drawFastHLine(15 - (f * 3 + 9) % 20, 1, 2, LED_ON);
    matrix.drawFastHLine(15 - (f * 3 + 5) % 20, 6, 3, LED_ON);
    matrix.drawFastHLine(15 - (f * 3 + 14) % 20, 7, 2, LED_ON);
    matrix.writeDisplay();
    matrix.setRotation(0);
    delay(40);
  }
}

// Witch at (x, y) with a sparkle trail behind her broom
void drawWitch(int8_t x, int8_t y, uint16_t ms) {
  matrix.setRotation(1);
  matrix.clear();
  matrix.drawBitmap(x, y, witchImg, 16, 8, LED_ON);
  for (uint8_t s = 0; s < 2; s++) matrix.drawPixel(x + 16 + random(4), 5 + random(3), LED_ON);
  matrix.writeDisplay();
  delay(ms);
}

// All-seeing eye inside the occult triangle.
// open: 0 = closed line, 1 = half open, 2 = open. Pupil at px, px+1 (row 4).
void drawOccultEye(uint8_t open, int8_t px, uint16_t ms) {
  matrix.setRotation(1);
  matrix.clear();
  matrix.drawLine(7, 0, 0, 7, LED_ON);   // triangle
  matrix.drawLine(8, 0, 15, 7, LED_ON);
  matrix.drawFastHLine(0, 7, 16, LED_ON);
  matrix.drawFastHLine(6, 4, 4, LED_ON);  // the eye
  if (open >= 1) { matrix.drawFastHLine(5, 4, 6, LED_ON); }
  if (open >= 2) {
    matrix.drawFastHLine(6, 3, 4, LED_ON);
    matrix.drawFastHLine(6, 5, 4, LED_ON);
    matrix.drawPixel(px, 4, LED_OFF);
    matrix.drawPixel(px + 1, 4, LED_OFF);
  }
  matrix.writeDisplay();
  matrix.setRotation(0);
  delay(ms);
}

// Occult ritual: candles light up in a circle, the triangle appears,
// the all-seeing eye opens, looks around, stares at you... and fades
void occult() {
  static const int8_t candles[][2] = { {7, 0}, {11, 1}, {14, 3}, {11, 6}, {7, 7}, {3, 6}, {1, 3}, {3, 1} };
  matrix.setRotation(1);
  for (uint8_t n = 1; n <= 8; n++) {  // candles light one by one
    for (uint8_t fl = 0; fl < 3; fl++) {           // flickering
      matrix.clear();
      for (uint8_t c = 0; c < n; c++) {
        if (c < n - 1 || fl != 1) matrix.drawPixel(candles[c][0], candles[c][1], LED_ON);
      }
      matrix.writeDisplay();
      delay(110);
    }
  }
  for (uint8_t i = 0; i < 20; i++) {  // the circle flickers, power rising
    matrix.clear();
    for (uint8_t c = 0; c < 8; c++) if (random(5)) matrix.drawPixel(candles[c][0], candles[c][1], LED_ON);
    matrix.writeDisplay();
    delay(80);
  }
  matrix.fillRect(0, 0, 16, 8, LED_ON);  // flash
  matrix.writeDisplay();
  delay(80);
  matrix.clear();
  matrix.drawLine(7, 0, 0, 7, LED_ON);  // triangle traces itself
  matrix.writeDisplay();
  delay(350);
  matrix.drawLine(8, 0, 15, 7, LED_ON);
  matrix.writeDisplay();
  delay(350);
  matrix.drawFastHLine(0, 7, 16, LED_ON);
  matrix.writeDisplay();
  matrix.setRotation(0);
  delay(700);

  drawOccultEye(0, 7, 800);  // the eye opens
  drawOccultEye(1, 7, 200);
  drawOccultEye(2, 7, 1000);
  static const int8_t look[] = { 6, 8, 6, 8, 7 };  // looks around...
  for (uint8_t i = 0; i < 5; i++) drawOccultEye(2, look[i], 600);
  for (uint8_t i = 0; i < 3; i++) {    // ...and stares at you, pulsing
    for (int8_t b = 15; b >= 5; b -= 2) { matrix.setBrightness(b); drawOccultEye(2, 7, 50); }
    for (int8_t b = 5; b <= 15; b += 2) { matrix.setBrightness(b); drawOccultEye(2, 7, 50); }
  }
  drawOccultEye(2, 7, 800);
  drawOccultEye(1, 7, 150);  // closes
  drawOccultEye(0, 7, 600);
  for (int8_t b = 15; b >= 0; b--) { matrix.setBrightness(b); drawOccultEye(0, 7, 70); }
  matrix.clear();
  matrix.writeDisplay();
  matrix.setBrightness(15);
  delay(600);
}

// Magic book: glowing cover, it opens, pages flip, magic sparkles rise out
void book() {
  for (uint8_t i = 0; i < 20; i++) {  // the cover glows
    matrix.setBrightness(4 + (i % 10 < 5 ? i % 5 : 5 - i % 5) * 2);
    showFace(bookClosed, 0, 0, 100);
  }
  matrix.setBrightness(15);
  showFace(bookOpen, 0, 0, 600);
  matrix.setRotation(1);
  for (uint8_t p = 0; p < 3; p++) {  // three pages flip right to left
    for (int8_t x = 14; x >= 1; x--) {
      matrix.clear();
      matrix.drawBitmap(0, 0, bookOpen, 16, 8, LED_ON);
      matrix.drawLine(7, 6, x, 3 + abs(x - 7) / 3, LED_ON);  // the turning page
      matrix.writeDisplay();
      delay(45);
    }
  }
  int8_t sx[5], sy[5];  // sparkles rising out of the book
  for (uint8_t k = 0; k < 5; k++) { sx[k] = random(5, 11); sy[k] = random(4); }
  for (uint8_t f = 0; f < 50; f++) {
    matrix.clear();
    matrix.drawBitmap(0, 0, bookOpen, 16, 8, LED_ON);
    for (uint8_t k = 0; k < 5; k++) {
      if (f % 2 == 0 && --sy[k] < 0) { sx[k] = random(5, 11); sy[k] = 3; }
      if (f % 2 == 0) sx[k] = constrain(sx[k] + random(-1, 2), 0, 15);
      matrix.drawPixel(sx[k], sy[k], LED_ON);
    }
    matrix.writeDisplay();
    delay(80);
  }
  matrix.setRotation(0);
  castSpell();
}

// The spell from the book: a beam shoots up, bursts like a firework,
// stars rain down, and Otto's eyes turn into twinkling stars
void castSpell() {
  matrix.setRotation(1);
  for (int8_t y = 4; y >= 0; y--) {  // beam rises from the book
    matrix.clear();
    matrix.drawBitmap(0, 0, bookOpen, 16, 8, LED_ON);
    matrix.drawFastVLine(7, y, 5 - y, LED_ON);
    matrix.drawFastVLine(8, y, 5 - y, LED_ON);
    matrix.writeDisplay();
    delay(70);
  }
  static const int8_t dir[][2] = { {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1} };
  for (uint8_t len = 1; len <= 6; len++) {  // firework burst
    matrix.clear();
    for (uint8_t d = 0; d < 8; d++) {
      int8_t tx = 7 + dir[d][0] * len, ty = 3 + dir[d][1] * len;
      if (len > 2) matrix.drawPixel(tx, ty, LED_ON);  // only sparkling tips
      else matrix.drawLine(7, 3, tx, ty, LED_ON);     // rays at first
    }
    matrix.writeDisplay();
    delay(90);
  }
  int8_t rx[8], ry[8];  // star rain
  for (uint8_t k = 0; k < 8; k++) { rx[k] = random(16); ry[k] = -random(8); }
  for (uint8_t f = 0; f < 30; f++) {
    matrix.clear();
    for (uint8_t k = 0; k < 8; k++) {
      if (++ry[k] > 7) { rx[k] = random(16); ry[k] = 0; }
      if (ry[k] >= 0 && random(5)) matrix.drawPixel(rx[k], ry[k], LED_ON);
    }
    matrix.writeDisplay();
    delay(90);
  }
  matrix.setRotation(0);
  summonObject();
  showEyes(happyImg, 1500);
}

// The spell conjures a random little object, which appears with a pop
// and sparkles around it, then vanishes in a poof
void summonObject() {
  uint8_t o = random(5);
  matrix.setRotation(1);
  for (uint8_t len = 1; len <= 3; len++) {  // pop
    matrix.clear();
    matrix.drawCircle(7, 3, len, LED_ON);
    matrix.writeDisplay();
    delay(70);
  }
  for (uint8_t f = 0; f < 35; f++) {  // the object, with sparkles on both sides
    matrix.clear();
    matrix.drawBitmap(4, 0, objectImg[o], 8, 8, LED_ON);
    matrix.drawPixel(random(3), random(8), LED_ON);
    matrix.drawPixel(13 + random(3), random(8), LED_ON);
    matrix.writeDisplay();
    delay(100);
  }
  for (uint8_t len = 1; len <= 5; len++) {  // poof, it vanishes
    matrix.clear();
    if (len < 3) matrix.drawBitmap(4, 0, objectImg[o], 8, 8, LED_ON);
    matrix.drawCircle(7, 3, len + 2, LED_ON);
    matrix.writeDisplay();
    delay(80);
  }
  matrix.setRotation(0);
}

// Decided, black-cat style: cat eyes, a blink, then they narrow and lock on
void cat() {
  showFace(catEyes, 0, 0, 2000);
  quickBlink();
  showFace(catEyes, 0, 0, 1200);
  showFace(catSquint, 0, 0, 2500);
  quickBlink();
  showFace(catSquint, 0, 0, 1500);
}

// Content: smiling eyes and a calm blink
void content() {
  showEyes(happyImg, 2000);
  slowBlink();
  showEyes(happyImg, 1500);
}

// Bombastic side eye: heavy lids, pupils slammed to one side, held... judging.
// Then an exasperated eye roll. Otto the bad bitch.
void sideEye() {
  bool left = random(2);
  int8_t side = left ? 1 : 5;
  drawRollEyes(3, 3, 0, 300);                                    // normal look
  for (int8_t p = 3; p != side; p += left ? -1 : 1) drawRollEyes(p, 3, 0, 90);  // pupils slide over
  for (uint8_t lid = 1; lid <= 2; lid++) drawRollEyes(side, 3, lid, 150);       // lids drop slowly
  drawRollEyes(side, 3, 2, 2200);                                // judging...
  drawRollEyes(side, 3, 5, 80);                                  // blink
  drawRollEyes(side, 3, 2, 1000);                                // still judging
  for (int8_t lid = 1; lid >= 0; lid--) drawRollEyes(side, 3, lid, 70);  // lids lift for the roll
  eyeRoll(left);
}

// Round eyes (rotation 1), pupil at (px, py) inside each eye.
// lid = rows covered from the top by a flat eyelid (0 = wide open, 5+ = shut)
void drawRollEyes(int8_t px, int8_t py, uint8_t lid, uint16_t ms) {
  matrix.setRotation(1);
  matrix.clear();
  for (uint8_t e = 0; e < 2; e++) {
    int8_t ox = e * 8;
    matrix.fillRoundRect(ox + 1, 1, 6, 6, 2, LED_ON);
    matrix.fillRect(ox + px, py, 2, 2, LED_OFF);
    if (lid > 0) {
      matrix.fillRect(ox, 0, 8, lid, LED_OFF);
      matrix.drawFastHLine(ox + 1, lid, 6, LED_ON);
    }
  }
  matrix.writeDisplay();
  matrix.setRotation(0);
  delay(ms);
}

// Eye roll: pupils sweep from the side, up over the top, to the other side,
// stay up a beat ("ugh"), then the lids sink, a sigh, and back to normal
void eyeRoll(bool fromLeft) {
  static const int8_t path[][2] = { {1, 3}, {1, 2}, {2, 1}, {3, 1}, {4, 1}, {5, 2}, {5, 3} };
  for (uint8_t i = 0; i < 7; i++) {
    uint8_t s = fromLeft ? i : 6 - i;
    drawRollEyes(path[s][0], path[s][1], 0, 110);
  }
  drawRollEyes(3, 1, 0, 700);                                      // ugh...
  for (uint8_t lid = 1; lid <= 3; lid++) drawRollEyes(3, lid + 1, lid, 160);  // lids sink, gaze drops
  drawRollEyes(3, 4, 3, 1800);                                     // so done
  for (uint8_t lid = 4; lid <= 5; lid++) drawRollEyes(3, 4, lid, 150);   // long sigh-blink
  delay(300);
  for (int8_t lid = 4; lid >= 0; lid--) drawRollEyes(3, lid > 2 ? 4 : 3, lid, 130);  // eyes reopen
  drawRollEyes(3, 3, 0, 500);
}

// Quick glances: the gaze jumps from spot to spot, as if something moved
void darting() {
  static const int8_t spots[][2] = { {1, 1}, {5, 1}, {1, 5}, {5, 5}, {3, 1}, {3, 5}, {1, 3}, {5, 3} };
  for (uint8_t i = 0; i < 12; i++) {
    uint8_t s = random(8);
    showGaze(spots[s][0], spots[s][1], random(150, 400));
  }
  showGaze(3, 3, 800);
}

// ---- Halloween ----------------------------------------------------------------

// The eyes turn into a flapping bat, which flies away and comes back
void bats() {
  for (uint8_t i = 0; i < 6; i++) {
    showFace(batUp, 0, 0, 180);
    showFace(batDown, 0, 0, 180);
  }
  for (int8_t x = 0; x >= -16; x--) showFace((x & 2) ? batUp : batDown, x, 0, 60);
  for (int8_t x = 16; x >= 0; x--) showFace((x & 2) ? batUp : batDown, x, 0, 60);
}

// A parade of `count` ghosts floating across, bobbing with wavy tails
void ghostParade(uint8_t count, uint16_t ms) {
  int8_t last = -(int8_t)((count - 1) * 10 + 8);
  for (int8_t x = 16; x >= last; x--) {
    matrix.clear();
    for (uint8_t g = 0; g < count; g++) {
      matrix.drawBitmap(x + g * 10, ((x >> 2) + g) & 1, ghostImg[((x >> 1) + g) & 1], 8, 8, LED_ON);
    }
    matrix.writeDisplay();
    delay(ms);
  }
}

// Two ghosts at (0, y1) and (8, y2); y = 8 means hidden below
void twoGhosts(int8_t y1, int8_t y2, uint8_t f, uint16_t ms) {
  matrix.clear();
  matrix.drawBitmap(0, y1, ghostImg[f], 8, 8, LED_ON);
  matrix.drawBitmap(8, y2, ghostImg[!f], 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(ms);
}

// Ghost party: a parade, two ghosts pop up and hover, then a faster parade
void ghosts() {
  matrix.setRotation(1);
  ghostParade(3, 110);
  for (int8_t t = 0; t <= 12; t++) {  // rise one after the other
    twoGhosts(max(8 - t, 0), min(8, max(12 - t, 0)), t & 1, 110);
  }
  for (uint8_t i = 0; i < 30; i++) {  // hover side by side
    twoGhosts((i >> 2) & 1, ((i >> 2) + 1) & 1, (i >> 1) & 1, 100);
  }
  for (int8_t y = 0; y <= 8; y++) twoGhosts(y, y, y & 1, 90);  // sink
  ghostParade(4, 80);
  matrix.setRotation(0);
}

// Magic forest: running through pines (near trees pass faster than far ones),
// fireflies blink around, then gather into a glow that becomes Otto's eyes
void forest() {
  static const int8_t bigAt[] = { 0, 16 };
  int8_t fx[2], fy[2];
  for (uint8_t k = 0; k < 2; k++) { fx[k] = random(16); fy[k] = random(5); }

  matrix.setRotation(1);
  for (uint8_t f = 0; f < 160; f++) {  // ~10 s
    matrix.clear();
    for (uint8_t t = 0; t < 2; t++) {
      int8_t xb = (bigAt[t] - f / 2 + 320) % 32 - 6;  // calm pace: 1 px every 2 frames
      matrix.drawBitmap(xb, 0, pineBig, 5, 8, LED_ON);
    }
    for (uint8_t k = 0; k < 2; k++) {  // fireflies drift and blink
      if (f % 4 == 0) {
        fx[k] = constrain(fx[k] + random(-1, 2), 0, 15);
        fy[k] = constrain(fy[k] + random(-1, 2), 0, 5);
      }
      if (random(4)) matrix.drawPixel(fx[k], fy[k], LED_ON);
    }
    matrix.writeDisplay();
    delay(60);
  }
  for (uint8_t s = 0; s < 8; s++) {  // fireflies gather in the middle
    matrix.clear();
    for (uint8_t k = 0; k < 2; k++) {
      fx[k] += (fx[k] < 7) - (fx[k] > 8);
      fy[k] += (fy[k] < 3) - (fy[k] > 4);
      matrix.drawPixel(fx[k], fy[k], LED_ON);
    }
    matrix.writeDisplay();
    delay(120);
  }
  for (uint8_t r = 0; r < 9; r++) {  // magic glow
    matrix.clear();
    matrix.drawCircle(7, 3, r, LED_ON);
    matrix.writeDisplay();
    delay(60);
  }
  matrix.setRotation(0);
  for (int8_t i = 4; i >= 0; i--) showEyes(blinkImg[i], 150);  // eyes open slowly
}

// A witch flies by leaving sparkles, casts a spell: burst of magic,
// twinkling stars, and Otto ends up hypnotized with spiral eyes
void witch() {
  matrix.setRotation(1);
  for (int8_t x = 16; x > 0; x--) drawWitch(x, 0, 130);   // flies in
  for (uint8_t i = 0; i < 16; i++) drawWitch(0, (i >> 2) & 1, 130);  // hovers
  for (int8_t x = 0; x >= -16; x--) drawWitch(x, 0, 110);  // flies off
  delay(400);
  for (uint8_t r = 0; r < 10; r++) {  // the spell: a burst of magic
    matrix.clear();
    matrix.drawCircle(7, 3, r, LED_ON);
    if (r > 2) matrix.drawCircle(7, 3, r - 3, LED_ON);
    matrix.writeDisplay();
    delay(70);
  }
  for (uint8_t i = 0; i < 30; i++) {  // stars twinkling down
    matrix.clear();
    for (uint8_t s = 0; s < 6; s++) matrix.drawPixel(random(16), random(8), LED_ON);
    matrix.writeDisplay();
    delay(80);
  }
  matrix.setRotation(0);
  for (uint8_t i = 0; i < 24; i++) {  // hypnotized
    showFace((i & 1) ? spiralB : spiralA, 0, 0, 150);
  }
  quickBlink();  // snaps out of it
  quickBlink();
}

// A spider drops down on its thread, dangles, then climbs back up
void spider() {
  matrix.setRotation(1);
  for (int8_t y = -5; y <= 2; y++) drawSpider(5, y, y & 1, 150);  // drop (stays fully on screen)
  for (uint8_t i = 0; i < 24; i++) {                              // dangle
    static const int8_t swing[] = { 5, 6, 6, 5, 4, 4 };
    drawSpider(swing[i % 6], 2, i & 1, 130);
  }
  for (int8_t y = 2; y >= -5; y--) drawSpider(5, y, y & 1, 90);   // climb back up
  matrix.setRotation(0);
}

void drawSpider(int8_t x, int8_t y, uint8_t f, uint16_t ms) {
  matrix.clear();
  if (y > 0) matrix.drawFastVLine(x + 2, 0, y, LED_ON);  // thread above it
  matrix.drawBitmap(x, y, spiderImg[f], 5, 5, LED_ON);
  matrix.writeDisplay();
  delay(ms);
}


// Evil eyes flickering in the dark, "BOO" drops in, then he scares himself
void spooky() {
  for (uint8_t i = 0; i < 8; i++) {
    showFace(spookyEyes, 0, 0, random(400, 900));
    matrix.clear();
    matrix.writeDisplay();
    delay(random(100, 300));
  }
  showFace(spookyEyes, 0, 0, 2000);
  for (int8_t y = -7; y <= 1; y++) showFace(booText, 0, y, 25);  // drops in at once
  showFace(booText, 0, 0, 80);                                     // tiny bounce
  showFace(booText, 0, 1, 1500);
  for (int8_t y = 2; y <= 8; y++) showFace(booText, 0, y, 25);   // and falls away
  scared();
}

// Scared: wide eyes, trembling pupils, a nervous blink
void scared() {
  for (uint8_t i = 0; i < 60; i++) {
    showGaze(2 + random(3), 2 + random(3), 50);
  }
  quickBlink();
  for (uint8_t i = 0; i < 30; i++) {
    showGaze(2 + random(3), 2 + random(3), 50);
  }
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
