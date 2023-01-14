[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/donate?hosted_button_id=WT735CX4UMZ9U)

## ~~LUFA Switch/XInput hybrid~~ DORMANT Fightstick, arcade edition

Windows is notoriously annoying with xinput controller ordering.

This custom firmware allows a controller to remain "dormant", waiting for an hid packet from your computer to wake up and enumerate as xinput controller.

## Workflow

- Use the regular LUFAHybridFightstick firmware on your P1 side
- Use this dormant controller firmware on the P2 side
- Add a shortcut to run the wakeup tool at windows startup

This way P1 side will always be the 1st controller since it was the only controller at windows startup, whereas P2 side got "plugged" later on.

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

## Donation

If this project helps you and you want to give back, you can help me with my future projects.

While not necessary, donations are much appreciated and will only go towards funding future github projects (arcade hardware ain't cheap :( ).

Of course you'll also receive my gratitude and I'll remember you if you post a feature request ;)

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/donate?hosted_button_id=WT735CX4UMZ9U)
