# Otto Eyes — emotional eyes, Halloween & Happy Birthday

> 🛠️ **A utility for the Otto project**: a toolbox of ready-to-upload sketches
> (eye animations, calibration, tests and dances) to bring your Otto DIY robot to life.

Arduino sketches for an **[Otto DIY](https://www.ottodiy.com/) "Eyes"** robot (2021 kit):
a biped Otto with an 8x16 LED matrix instead of the ultrasonic "eyes".

The main sketch, `OttoE_halloween`, turns Otto into a **Halloween running buddy**:
clipped to your shoulder, it lives its own life with expressive eyes (ghosts, bats, a witch,
a black cat, a magic book…) and cheers you on every 15 minutes.

## The sketches

| Folder | What it does | Servos |
|---|---|---|
| `sketches/OttoE_halloween` | **The Halloween running buddy.** Human-like gaze (blinks, moving pupils), random animations, a *bombastic side eye* + eye roll between animations, "Courage, go go go!" every 15 min. | no |
| `sketches/Otto_happy_birthday` | Otto walks up to the person, "Happy Birthday" scrolls, he jumps, a party popper bursts, then he follows the conversation and approves ("Yes!", "Mm-hmm", "Ooh!") every 45 s. | yes |
| `sketches/OttoE_rolleyes` | Soft, cute version: gaze, tender blink, smiling eyes, hearts, "Metabolic Twin <3". | no |
| `sketches/Otto_calibration_test` | **Interactive calibration** of the 4 servos through the Serial Monitor + movement test. | yes |
| `sketches/Otto_zero_dance` | Holds the servos at 90° with no trim for 15 s (to center the servo horns), then a slow little dance. | yes |
| `sketches/Otto_little_dance` | Full dance (~40 s) to check the servos. | yes |
| `sketches/Otto_touch_test` | Touch sensor test on A0 (the screen lights up when touched). | no |
| `sketches/servo_prof_otto` | Single-servo test on pin 2 (10° → 170° → 90°). | yes |

### `OttoE_halloween` animations

Random pick out of 31:

| Odds | Animations |
|---|---|
| 4/31 | 🏃 **Speed** (squinted eyes + wind streaks) |
| 3/31 each | 🧙‍♀️ Witch + spell (hypnotized eyes) · 📖 Magic book + spell (random object: potion, frog, crystal ball, mushroom, crown) · 🐈‍⬛ Black cat · 👹 Spooky + "BOO" + scared · 👻 Ghosts |
| 2/31 each | 🦇 Bats · 😌 Content · 🌲 Magic forest · 🔺 Occult ritual (all-seeing eye) · 🕷️ Spider · 👀 On the lookout |

- Between two animations: 10 to 20 s of normal gaze, and **1 pause out of 4** a *bombastic side eye* followed by an eye roll.
- Every **15 minutes**: "Cou-ra-ge! Go go go!" on the buzzer, then the speed animation.
- `const bool DEMO = true;` at the top of the file: every animation in a row (to preview them), cheer every minute. Set it back to `false` for the run.

## Hardware and wiring

Board: **Arduino Nano** (clone, **CH340** USB chip, **old bootloader**) on a Nano I/O expansion shield.

| Part | Pin | Note |
|---|---|---|
| Left leg | 2 | "left" = **the robot's** left (on your right when it faces you) |
| Right leg | 3 | |
| Left foot | 4 | |
| Right foot | 5 | |
| Servo wires | S / V / G | orange = **S**, red = **V**, brown = **G** |
| Buzzer | 13 | **+ on S, − on G** (V row left empty) |
| 8x16 LED matrix (HT16K33) | I2C: SDA = A4, SCL = A5 | address **0x70** |
| Touch sensor (optional) | A0 | I/O → S, VCC → V, GND → G. Not used by `OttoE_halloween`. |
| Power | VIN / GND through the switch | see below |

### Power ⚠️
- **Eyes only** (Halloween): a regular 9V battery is enough.
- **With servos** (walking, dancing, jumping): it needs current. A 9V alkaline or **AAA** batteries sag → weak movements and **Otto rebooting on every move** ("beep, move, beep").
  Use a **rechargeable 9V Li-ion**, a **2S 7.4 V Li-ion** pack or **6 AA** batteries. VIN needs at least ~7 V.

## Setup

1. **Arduino IDE** (1.8 or 2.x).
2. Libraries:
   - **OttoDIYLib v9** (`Otto9.h`): [github.com/OttoDIY/OttoDIYLib](https://github.com/OttoDIY/OttoDIYLib) → *Sketch > Include Library > Add .ZIP Library*
   - **Adafruit LED Backpack Library** and **Adafruit GFX Library** (Library Manager)
3. Settings: *Board* **Arduino Nano**, *Processor* **ATmega328P (Old Bootloader)**, *Port*: the one labeled **CH340**.

### If the upload fails
- `stk500_getsync` / `protocol error` / `out of sync` → check **Old Bootloader**.
- `port does not exist` or a drop mid-upload → **cable straight into the PC** (no USB hub/switch), another cable, **battery OFF**, re-check the **COM port number** (it changes when you change USB socket).
- Still failing → unplug the servos during the upload.
- One `.ino` per folder: Arduino compiles every `.ino` in a folder together.

## Calibration (once)

1. Servo horns mounted at ~90° (see the Otto manual).
2. Upload `Otto_calibration_test`, keep USB plugged in, **switch the battery on**, Serial Monitor at **9600 baud**.
3. `z` = all to zero · `q/a` left leg · `w/s` right leg · `e/d` left foot · `r/f` right foot · `t` = test · `v` = **save** (EEPROM).
4. Saved trims are loaded by every sketch that calls `Otto.init(..., true, ...)`.

## Credits and license

Based on the [Otto DIY](https://www.ottodiy.com/) examples (OttoDIYLib, `OttoE_rolleyes`, `OttoE_ALLeyes`) and the Adafruit LED Backpack library.
**BSD** license, like the original code: keep the notices at the top of the files when redistributing.

The `.claude/skills/otto-eyes/` folder holds a Claude Code *skill* that sums up the whole setup and the project's choices.
