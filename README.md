## LUFA Switch/XInput hybrid Fightstick

I really like the "neogeo pad 2" (basically a neogeo cd pad with 6 face buttons + 2 triggers), and wanted to make one of mine work both on nintendo switch and PC so I modded an Arduino pro micro inside it and made some code.

[Demo](https://www.instagram.com/p/B56IP1AIHOc/)

I've also written some variants if you'd rather convert a [PSX pad](https://github.com/CrazyRedMachine/LUFAHybridFightstick/tree/PSX), a [SNES pad](https://github.com/CrazyRedMachine/LUFAHybridFightstick/tree/SNES) or a [PC-FX pad](https://github.com/CrazyRedMachine/LUFAHybridFightstick/tree/PCFX).

## Acknowledgments

This work is based on [fluffymadness' ATMega32U4-Switch-Fightstick](https://github.com/fluffymadness/ATMega32U4-Switch-Fightstick)
 and [bootsector's XInputPadMicro](https://github.com/bootsector/XInputPadMicro), with a couple QoL improvements I needed.
 
## Features

### Gamepad modes

Hold "select" as you plug the controller to make it a Nintendo Switch controller.

Hold "start" as you plug the controller to make it an XInput controller.

Gamepad mode is persistent across plugging and unplugging the controller, so if you don't press anything while plugging the controller, it will stay in the last used mode. 

### DPAD modes

You can switch seamlessly between the 3 stick modes by pressing START+SELECT+(LEFT/UP/RIGHT)

- UP: DPAD 
- LEFT: Fake Left Analog (DPad is mapped to L-Analog)
- RIGHT: Fake Right Analog (DPad is mapped to R-Analog)

DPAD mode is also persistent.

### Real analog support

Analog sticks can be used by wiring them to the arduino analog inputs.

In this case the DPAD modes are inactive (dpad will always remain in HAT mode regardless of DPAD mode).

**Important**: There is an auto-calibration of the analog ranges during execution, which means that you should move the sticks around before they respond properly.
By holding the 2 analog clicks while plugging the controller you enter "persistent calibration mode" which will write calibration values to the EEPROM.

So the workflow for first run should be :

- Plug the controller **while holding the two analog clicks** (this is only needed the first time you flash the firmware)
- Move both analogs around for several full rotations (so that the arduino can learn what are your analog ranges)
- Unplug the controller (to exit calibration mode)

Congrats, your analog sticks should respond properly from now on :)

### Simulated home button

Because the neogeo pad 2 doesn't have a home button, I also added some code so that holding start+select during more than 1 second presses the home button. You can customize the delay with #define HOME_DELAY 1000 in the .ino file.

### SOCD Cleaning

When simultaneous opposite cardinal direction (SOCD) inputs are detected, the default 
resolution follows the standard hitbox-style setup: LEFT + RIGHT = NEUTRAL and UP + DOWN = UP.

SOCD configuration can be enabled with `ENABLE_SOCD_CONFIG`. This allows the x (LEFT/RIGHT) and
y (UP/DOWN) axes to each have their own SOCD mode. These options are stored in persistent memory.

Changes are performed by holding down input directions corresponding to your desired setup and 
then pressing the SOCD configuration button combination, which is set to L3+R3 by default.

- If no buttons are held down, simultaneous inputs output neutral. This is the default L+R behavior for hitbox-style controllers.
- If one button is held down, that button will take priority when simultaneous inputs are made. This is the default U+D behavior for hitbox-style controllers — when both are held down, UP always has priority.
- If both buttons are held down, the last input will take priority. (Also known as second input priority.)

For example, to set the SOCD configuration to the default hitbox-style setup:
- Release all directional inputs, hold UP, and press the SOCD configuration button combination.

To make all SOCDs resolve to neutral:
- Release all directional inputs and press the config button.

To enable last input priority on both axes:
- Press and hold all directional inputs, and then press the config button.

## Building Instructions

- Download Arduino IDE, 
- Download the Bounce2 Library inside the Arduino IDE
- Add `https://github.com/CrazyRedMachine/Arduino-Lufa/raw/master/package_arduino-lufa_index.json` as an Additional Board Manager URL (in `File -> Preferences` menu)
- Install LUFA AVR Boards from the Board Manager
- Select Arduino Leonardo (LUFA) as your board type
- Upload the code and Have Fun

## Pinout

Configurable in the .ino

An Arduino Micro (not pro micro) should have enough gpio to connect a full gamepad. On a Leonardo you might need to solder an extra wire to retrieve the SS pin.

```
/* PINOUT (follows Nintendo naming (X=up, B=down)) */
#define PIN_UP    MOSI
#define PIN_DOWN  1
#define PIN_LEFT  SS
#define PIN_RIGHT 0
#define PIN_A     12           //XBOX B
#define PIN_B     11           //XBOX A  
#define PIN_X     9            //XBOX Y
#define PIN_Y     10           //XBOX X     
#define PIN_L     8            //XBOX LB
#define PIN_R     2            //XBOX RB
#define PIN_ZL    7            //XBOX LT
#define PIN_ZR    3            //XBOX RT
#define PIN_LS    A4           //XBOX LS (left analog click)
#define PIN_RS    A5           //XBOX RS (right analog click)
#define PIN_PLUS  4            //XBOX START
#define PIN_MINUS 6            //XBOX BACK
#define PIN_HOME  5

#ifdef WITH_ANALOG
#define PIN_LANALOGX    A0
#define PIN_LANALOGY    A1
#define PIN_RANALOGX    A2
#define PIN_RANALOGY    A3
```
