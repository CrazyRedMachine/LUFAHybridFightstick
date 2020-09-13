## LUFA Switch/XInput hybrid Fightstick (PC-FX Edition)

This code can be used to make 


## Acknowledgments

This work is based on [fluffymadness' ATMega32U4-Switch-Fightstick](https://github.com/fluffymadness/ATMega32U4-Switch-Fightstick)
 and [bootsector's XInputPadMicro](https://github.com/bootsector/XInputPadMicro), with a couple QoL improvements I needed.
 
## Features

### Gamepad modes

Hold "select" as you plug the controller to make it a Nintendo Switch controller.

Hold "start" as you plug the controller to make it an XInput controller.

Gamepad mode is persistent across plugging and unplugging the controller, so if you don't press anything while plugging the controller, it will stay in the last used mode. 

### DPAD modes

You can use the latching switches on the PC-FX pad to switch between DPAD modes.

MODE 1 switch selects between Analog (A) or DPAD (B).

MODE 2 switch selects between LEFT Analog (A) or RIGHT Analog (B). 
It is ignored when in DPAD mode. 

### Simulated home button

Because the PC-FX pad doesn't have a home button, I also added some code so that holding start+select during more than 1 second presses the home button. You can customize the delay with #define HOME_DELAY 1000 in the .ino file.

## Building Instructions

- Download Arduino IDE, 
- Download Arduino Lufa from https://github.com/Palatis/Arduino-Lufa and follow the instructions to install and activate it
- Build and Flash for your ATMEGA32U4 Board
- Have Fun

## Pinout

When looking at the plug of the *controller cable* :
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
 
