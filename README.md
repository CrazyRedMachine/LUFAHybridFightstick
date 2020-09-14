## LUFA Switch/XInput hybrid Fightstick (PC-FX Edition)

This code can be used to make a PC-FX Gamepad work on Nintendo Switch and XBox360 (or PC in XInput mode).

I've put an arduino pro micro directly inside the controller, but you could also go non-destructive and use this code to make an external usb adapter.

## Demo

https://www.instagram.com/p/CFIFupXFU3L/

## Acknowledgments

This work is based on [fluffymadness' ATMega32U4-Switch-Fightstick](https://github.com/fluffymadness/ATMega32U4-Switch-Fightstick)
 and [bootsector's XInputPadMicro](https://github.com/bootsector/XInputPadMicro), with a couple QoL improvements I needed.
 
The PC-FX pad read code is based on [Anthony Burkholder's USBSNES controller](https://github.com/burks10/Arduino-SNES-Controller) (as it turns out the PC-FX pad also uses 16bit serial protocol, like a SNES pad, except the data latch pin is normally HIGH instead of LOW).
 
## Features

### DPAD modes

You can use the latching switches on the PC-FX pad to switch between DPAD modes.

MODE 1 switch selects between Analog (A) or DPAD (B).

MODE 2 switch selects between LEFT Analog (A) or RIGHT Analog (B). 

When in DPAD mode, it can activate the gamepad mode change. 

### Gamepad modes

Because the PC-FX pad is too slow to boot, I couldn't implement the "hold a button on boot to select mode" in a user-friendly way.

When both MODE 1 and MODE 2 switches are on position B, you can swap mode with a simulated hotplug.

Hold "select" for 3 seconds to make the controller virtually unplug and come back as a Nintendo Switch controller.

Hold "run" for 3 seconds to make the controller virtually unplug and come back as an XInput controller.

You can customize the delay with `#define SWITCH_DELAY 3000` in the .ino file.

Gamepad mode is persistent across plugging and unplugging the controller, so it will stay in the last used mode.

### Simulated home button

Because the PC-FX pad doesn't have a home button, I also added some code so that holding select+run during more than 1 second presses the home button. You can customize the delay with `#define HOME_DELAY 1000` in the .ino file.

## Building Instructions

- Download Arduino IDE, 
- Download Arduino Lufa from https://github.com/Palatis/Arduino-Lufa and follow the instructions to install and activate it
- Build and Flash for your ATMEGA32U4 Board
- Have Fun

## Pinout

When looking at the plug of the *controller cable* :
```
___________
\ 1 2 3 4 /
 \ 5 6 7 /
  -------

  V D x L
   x C G
  
 1: 5V                    -> to arduino 5V
 2: data (normally high)  -> to arduino digital pin 4
 4: latch (normally high) -> to arduino digital pin 3
 6: clock (normally high) -> to arduino digital pin 6
 7: GND                   -> to arduino GND

 ```

