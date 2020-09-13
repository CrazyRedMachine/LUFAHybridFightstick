## LUFA Switch/XInput hybrid Fightstick (SNES Edition)

This code can be used to make a SNES Gamepad work on Nintendo Switch and XBox360 (or PC in XInput mode).

I've put an arduino pro micro directly inside the controller, but you could also go non-destructive and use this code to make an external usb adapter.

## Acknowledgments

This work is based on [fluffymadness' ATMega32U4-Switch-Fightstick](https://github.com/fluffymadness/ATMega32U4-Switch-Fightstick)
 and [bootsector's XInputPadMicro](https://github.com/bootsector/XInputPadMicro), with a couple QoL improvements I needed.

The SNES pad read code is based on [Anthony Burkholder's USBSNES controller](https://github.com/burks10/Arduino-SNES-Controller).

## Features

### Gamepad modes

Hold "select" as you plug the controller to make it a Nintendo Switch controller.

Hold "start" as you plug the controller to make it an XInput controller.

Gamepad mode is persistent across plugging and unplugging the controller, so if you don't press anything while plugging the controller, it will stay in the last used mode. 

### DPAD modes

You can switch seamlessly between the 3 stick modes by pressing START+SELECT.

- DPAD
- Fake Left Analog (DPad is mapped to L-Analog)
- Fake Right Analog (DPad is mapped to R-Analog)

DPAD mode is also persistent.

### Simulated home button

Because the SNES pad doesn't have a home button, I also added some code so that holding start+select during more than 1 second presses the home button. You can customize the delay with #define HOME_DELAY 1000 in the .ino file.

## Building Instructions

- Download Arduino IDE, 
- Download Arduino Lufa from https://github.com/Palatis/Arduino-Lufa and follow the instructions to install and activate it
- Build and Flash for your ATMEGA32U4 Board
- Have Fun

## Pinout

When looking at the plug of the *controller cable* :
```
 1 2 3 4 | 5 6 7 >
 
 V C L D | x x G >
 
 1: 5V                    -> to arduino 5V
 2: clock (normally high) -> to arduino digital pin 6
 3: latch (normally LOW)  -> to arduino digital pin 3
 4: data  (normally high) -> to arduino digital pin 4
 7: GND                   -> to arduino GND

 ```

