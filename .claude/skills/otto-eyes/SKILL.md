---
name: otto-eyes
description: Full context for this project's Otto DIY "Eyes" robot (Nano CH340, 8x16 LED matrix, servos, buzzer) and the sketches in sketches/. Use for any Otto request: new eye animations, movements, sounds, calibration, failed uploads, wiring, power.
---

# Otto Eyes — project context

The owner writes in casual French: answer in French, simply, and explain physical steps one by one
(beginner in electronics). **Everything in the repository is in English**: code, comments, serial
messages, README, this skill. Anything not in English gets translated or removed.
Never add a `Co-Authored-By: Claude` (or any AI attribution) line to commits or PRs.

## Hardware (confirmed with the owner)

- Otto DIY **Eyes** (2021 kit), manual `OttoDIYEyesmanualV13.pdf`.
- **Arduino Nano clone, CH340 USB chip, OLD bootloader** (57600 baud) on a Nano I/O shield
  (S / V / G pin rows).
- Servos: **2 = left leg, 3 = right leg, 4 = left foot, 5 = right foot**.
  Left/right = **the robot's** point of view: facing it, its left leg is on YOUR right.
  Wires: orange = S, red = V, brown = G.
- **Buzzer on 13**: + on S, − on G (V empty). Plugged on S+V = silent (already happened).
- **8x16 HT16K33 LED matrix**, I2C (A4/A5), address **0x70**, `Adafruit_8x16matrix`.
- **Touch sensor: dropped.** Once wired backwards (maybe damaged), it does not detect through the
  shell, and A0 floats (toggles every 5-10 s). `OttoE_halloween` uses no sensor at all.
- No ultrasonic sensor and no noise sensor. Pass `A6` as the "noise" pin to `Otto.init`
  (free, analog-only pin) rather than -1.

## Power (cause #1 of problems)

- Eyes only: a 9V battery is enough.
- Servos: AAA batteries / 9V alkaline → weak movements and **a reset on every move**
  (symptom: "beep, move, beep" = the startup sound replays). VIN needs >= ~7 V.
  Recommend a 9V Li-ion, a 2S 7.4 V Li-ion pack or 6xAA.
- Movements that "strain": power first, then over-tightened pivot screws, rubbing plastic,
  off-center servo horn (end stop).

## Compiling and uploading

- Arduino IDE 1.8.13 is installed. Check that a sketch compiles (do it after EVERY change):
  ```
  cd "/c/Program Files (x86)/Arduino" && ./arduino_debug.exe --verify --board arduino:avr:nano:cpu=atmega328old "C:/path/to/Sketch/Sketch.ino" 2>&1 | grep -E "error|Sketch uses"
  ```
  (use `/` in the path: `\\` gets lost inside a loop or `$(...)`).
- Libraries installed in `~/Documents/Arduino/libraries`: `OttoDIYLib-master` (v9, `Otto9.h`),
  `Adafruit_LED_Backpack_Library`, `Adafruit_GFX_Library`.
- Upload failures already met and their fix:
  - `protocol error` / `out of sync`: processor **ATmega328P (Old Bootloader)**.
  - port vanishing mid-upload: **USB cable/socket** (not the desk USB switch), battery OFF,
    servos unplugged if needed. The **COM number changes** with the socket (COM5, COM6...):
    `Get-CimInstance Win32_PnPEntity | ? Name -match 'CH340'`.
  - writing reaches 100 % then verification fails = the sketch is usually loaded fine.
- **One .ino per folder**: a tab added in the IDE creates a 2nd .ino → "redefinition" errors.

## Known code pitfalls

- `zero` is a macro from `Otto_mouth9.h`: don't use it as a variable name.
- `Otto.home()` detaches the servos and does nothing if Otto is already "resting";
  to HOLD a position: `Otto._moveServos(ms, positions)`.
- Otto9 movements are blocking: the eyes can't animate during a `walk`/`jump`
  (show a still image right before).
- Calibration lives in EEPROM (bytes 0-3), loaded by `Otto.init(..., true, ...)`.

## Drawing on the 8x16 matrix

- **Rotation 0**: 8 wide x 16 tall; each eye = 8x8 bitmap drawn at (0,0) and (0,8).
  That is the format of the `blinkImg` / `happyImg` eyes (from the Otto examples). In `showGaze(px, py)`,
  `py` = horizontal on screen, `px` = vertical (check on the real screen when in doubt).
- **Rotation 1**: 16 wide x 8 tall, like text (upright). All hand-drawn art
  (ghosts, witch, cat, etc.) is in rotation 1: left eye = columns 0-7.
  Always go back to `setRotation(0)` afterwards.
- `drawBitmap`: 1 byte per 8 pixels, most significant bit on the left.
- 16x8 = only 2 to 3 letters: a long phrase must scroll. For text that "drops at once",
  draw a short word in a tiny font (e.g. "BOO" as a 16x8 bitmap).
- Hard to read on this screen: detailed faces (Dracula), tiny shapes.
  Easy to read: solid silhouettes, lines, dots, circles, motion.

## Owner's taste (feedback already given)

- Loves: **ghosts**, **speed**, **cat eyes**, **spooky + BOO + scared**, **witch spell**,
  **magic book + spell**, **on the lookout**, **bombastic side eye + eye roll**,
  magic forest (but sparse), spider (small, never leaving the screen).
- Dislikes: pumpkin, devil, Dracula, "thinking" with 3 dots, "focused", left-to-right scrolling
  text for Halloween ("cringe"), hearts and fluff in the running version,
  "Metabolic Twin" (removed from Halloween, kept only in `OttoE_rolleyes`).
- Wants a **human-like** Otto: a living gaze, smooth transitions (lids lowering, pupils
  sliding), and plenty of normal-gaze time between animations.

## The sketches (`sketches/` folder)

- `OttoE_halloween`: running buddy (eyes only). `DEMO = true` → everything in a row.
  Weights `random(31)`: speed 4; witch, book, cat, spooky, ghosts 3;
  bats, content, forest, ritual, spider, lookout 2. Pause 10-20 s, side eye 1 pause out of 4,
  "Courage go go go" every 15 min (`CHEER_EVERY_MIN`). In demo mode `demoOrder[]` lists the
  `case` numbers: update it when renumbering.
- `Otto_happy_birthday`: walks, Happy Birthday, jumps, party popper, then "approves" every 45 s
  (`AGREE_EVERY_S`).
- `OttoE_rolleyes`: soft version (hearts, Metabolic Twin).
- `Otto_calibration_test`: interactive calibration (Serial Monitor 9600: q/a w/s e/d r/f, z, v, t).
- `Otto_zero_dance`, `Otto_little_dance`, `Otto_touch_test`, `servo_prof_otto`: tests.

When adding an animation: give it a weight, add it to `demoOrder`, compile,
and say honestly that the drawing has not been seen on the real screen.
