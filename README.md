## LUFA Switch/XInput hybrid Fightstick

I really like the "neogeo pad 2" (basically a neogeo cd pad with 6 face buttons + 2 triggers), and wanted to make one of mine work both on nintendo switch and PC so I modded an Arduino pro micro inside it and made some code.

While [it's working great](https://www.instagram.com/p/B56IP1AIHOc/), it'd definitely need a bit more code cleanup ^^;

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

### Simulated home button

Because the neogeo pad 2 doesn't have a home button, I also added some code so that holding start+select during more than 1 second presses the home button. You can customize the delay with #define HOME_DELAY 1000 in the .ino file.

## Building Instructions

- Download Arduino IDE, 
- Download the Bounce2 Library inside the Arduino IDE
- Add `https://github.com/CrazyRedMachine/Arduino-Lufa/raw/new-workflow/package_arduino-lufa_index.json` as an Additional Board Manager URL (in `File -> Preferences` menu)
- Install LUFA AVR Boards from the Board Manager
- Select Arduino Leonardo (LUFA) as your board type
- Upload the code and Have Fun

## Pinout

Configurable in the .ino

```
/* PINOUT (follows Nintendo naming (X=up, B=down)) */
#define PIN_UP    A0
#define PIN_DOWN  A2
#define PIN_LEFT  A1
#define PIN_RIGHT A3
#define PIN_A     5            //XBOX B
#define PIN_B     4            //XBOX A
#define PIN_X     3            //XBOX Y
#define PIN_Y     15           //XBOX X
#define PIN_L     14           //XBOX LB
#define PIN_R     2            //XBOX RB
#define PIN_ZL    6            //XBOX LT
#define PIN_ZR    7            //XBOX RT
#define PIN_PLUS  16           //XBOX START
#define PIN_MINUS 10           //XBOX BACK
#define PIN_HOME  9
```
