## LUFA Switch/XInput hybrid Fightstick

I really like the "neogeo pad 2" (basically a neogeo cd pad with 6 face buttons + 2 triggers), and wanted to make one of mine work both on nintendo switch and PC so I modded an Arduino pro micro inside it and made some PoC code.

While [it's working great](https://www.instagram.com/p/B56IP1AIHOc/), it'd definitely need a lot of code cleanup ^^;

## Acknowledgments

This work is basically a very ugly patchwork of [fluffymadness' ATMega32U4-Switch-Fightstick](https://github.com/fluffymadness/ATMega32U4-Switch-Fightstick)
 and [bootsector's XInputPadMicro](https://github.com/bootsector/XInputPadMicro), with a couple QoL improvements I needed.
 
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

Because the neogeo pad 2 doesn't have a home button, I also added some code so that holding start+select during more than 1 second presses the home button. You can customize the delay with #define HOME_DELAY 1000 in the .ino file.

## Building Instructions

- Download Arduino IDE, 
- Download the Bounce2 Library inside the Arduino IDE
- Download Arduino Lufa from https://github.com/Palatis/Arduino-Lufa and follow the instructions to install and activate it
- Build and Flash for your ATMEGA32U4 Board
- Have Fun

## Pinout

Here's the pinout

	joystickUP.attach(A0,INPUT_PULLUP);
	
	joystickDOWN.attach(A2,INPUT_PULLUP);
	
	joystickLEFT.attach(A1,INPUT_PULLUP);
	
	joystickRIGHT.attach(A3,INPUT_PULLUP);
	
	buttonA.attach(5,INPUT_PULLUP);
	
	buttonB.attach(4,INPUT_PULLUP);
	
	buttonX.attach(3,INPUT_PULLUP);
	
	buttonY.attach(15,INPUT_PULLUP);
	
	buttonLB.attach(14,INPUT_PULLUP);
	
	buttonRB.attach(2,INPUT_PULLUP);
	
	buttonLT.attach(6,INPUT_PULLUP);
	
	buttonRT.attach(7,INPUT_PULLUP);
	
	buttonSTART.attach(16,INPUT_PULLUP);
	
	buttonSELECT.attach(10,INPUT_PULLUP);
	
	buttonHOME.attach(9,INPUT_PULLUP);
	
