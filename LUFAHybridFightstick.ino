#include "LUFAConfig.h"
#include <LUFA.h>
#include "XS_HID.h"
#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>
#include <EEPROM.h>
#include <inttypes.h>

//delay in ms for start+select to become HOME
#define HOME_DELAY 1000

/* Buttons declarations */
#define MILLIDEBOUNCE 1 //Debounce time in milliseconds
unsigned long startAndSelTime = 0;
unsigned long currTime = 0;

byte internalButtonStatus[4];


Bounce joystickUP = Bounce();
Bounce joystickDOWN = Bounce();
Bounce joystickLEFT = Bounce();
Bounce joystickRIGHT = Bounce();
Bounce buttonA = Bounce();
Bounce buttonB = Bounce();
Bounce buttonX = Bounce();
Bounce buttonY = Bounce();
Bounce buttonLB = Bounce();
Bounce buttonRB = Bounce();
Bounce buttonLT = Bounce();
Bounce buttonRT = Bounce();
Bounce buttonSTART = Bounce();
Bounce buttonSELECT = Bounce();
Bounce buttonHOME = Bounce();

/* MODE DECLARATIONS */
typedef enum {
  RIGHT_ANALOG_MODE,
  ANALOG_MODE,
  DIGITAL,
} State_t;
State_t state;

/* mode selectors */
bool xinput;
bool modeChanged;

void checkModeChange(){
    if (buttonStatus[BUTTONSTART] && buttonStatus[BUTTONSELECT])
    {
      if ( !modeChanged )
      {
        bool need_update = true;
        if (internalButtonStatus[BUTTONLEFT])
          state = ANALOG_MODE;
        else if (internalButtonStatus[BUTTONRIGHT])
          state = RIGHT_ANALOG_MODE;
        else if (internalButtonStatus[BUTTONUP])
          state = DIGITAL;
        else need_update = false;
        
        if (need_update) EEPROM.put(0, state);
        modeChanged = true;
      }
    }
    else 
    {
      modeChanged = false;
    }
}

void setupPins(){
    joystickUP.attach(A0,INPUT_PULLUP);
    joystickDOWN.attach(A2,INPUT_PULLUP);
    joystickLEFT.attach(A1,INPUT_PULLUP);
    joystickRIGHT.attach(A3,INPUT_PULLUP);
    buttonA.attach(5,INPUT_PULLUP);
    buttonB.attach(4,INPUT_PULLUP);;
    buttonX.attach(3,INPUT_PULLUP);
    buttonY.attach(15,INPUT_PULLUP);
    buttonLB.attach(14,INPUT_PULLUP);
    buttonRB.attach(2,INPUT_PULLUP);
    buttonLT.attach(6,INPUT_PULLUP);
    buttonRT.attach(7,INPUT_PULLUP);
    buttonSTART.attach(16,INPUT_PULLUP);
    buttonSELECT.attach(10,INPUT_PULLUP);
    buttonHOME.attach(9,INPUT_PULLUP);

    joystickUP.interval(MILLIDEBOUNCE);
    joystickDOWN.interval(MILLIDEBOUNCE);
    joystickLEFT.interval(MILLIDEBOUNCE);
    joystickRIGHT.interval(MILLIDEBOUNCE);
    buttonA.interval(MILLIDEBOUNCE);
    buttonB.interval(MILLIDEBOUNCE);
    buttonX.interval(MILLIDEBOUNCE);
    buttonY.interval(MILLIDEBOUNCE);
    buttonLB.interval(MILLIDEBOUNCE);
    buttonRB.interval(MILLIDEBOUNCE);
    buttonLT.interval(MILLIDEBOUNCE);
    buttonRT.interval(MILLIDEBOUNCE);
    buttonSTART.interval(MILLIDEBOUNCE);
    buttonSELECT.interval(MILLIDEBOUNCE);
    buttonHOME.interval(MILLIDEBOUNCE);
}
void setup() {

  modeChanged = false;
  EEPROM.get(0, state);
  EEPROM.get(2, xinput);
  setupPins();
  delay(500);
// if select is held on boot, NSWitch mode
  int value = digitalRead(10);
  if (value == LOW)
  {
    xinput = false;
    EEPROM.put(2, xinput);
  }
// if start is held on boot, XInput mode
  else {
    value = digitalRead(16);
    if (value == LOW)
      {
        xinput = true;
        EEPROM.put(2, xinput);
      }
  }
  
  SetupHardware(xinput);
  GlobalInterruptEnable();
}


void loop() {
    currTime = millis();
    buttonRead();
    checkModeChange();    
    convert_dpad();
    send_pad_state();
}

void convert_dpad(){
  
  switch (state)
  {
    case DIGITAL:
    buttonStatus[AXISLX] = 128;
    buttonStatus[AXISLY] = 128;
    buttonStatus[AXISRX] = 128;
    buttonStatus[AXISRY] = 128;
    buttonStatus[BUTTONUP] = internalButtonStatus[BUTTONUP];
    buttonStatus[BUTTONDOWN] = internalButtonStatus[BUTTONDOWN];
    buttonStatus[BUTTONLEFT] = internalButtonStatus[BUTTONLEFT];
    buttonStatus[BUTTONRIGHT] = internalButtonStatus[BUTTONRIGHT];
    break;

    case ANALOG_MODE:   
    buttonStatus[AXISRX] = 128;
    buttonStatus[AXISRY] = 128;
    buttonStatus[BUTTONUP] = 0;
    buttonStatus[BUTTONDOWN] = 0;
    buttonStatus[BUTTONLEFT] = 0;
    buttonStatus[BUTTONRIGHT] = 0;
    
    if ((internalButtonStatus[BUTTONUP]) && (internalButtonStatus[BUTTONRIGHT])){buttonStatus[AXISLY] = 0;buttonStatus[AXISLX] = 255;}
    else if ((internalButtonStatus[BUTTONDOWN]) && (internalButtonStatus[BUTTONRIGHT])) {buttonStatus[AXISLY] = 255;buttonStatus[AXISLX] = 255;}
    else if ((internalButtonStatus[BUTTONDOWN]) && (internalButtonStatus[BUTTONLEFT])) {buttonStatus[AXISLY] = 255;buttonStatus[AXISLX] = 0;}
    else if ((internalButtonStatus[BUTTONUP]) && (internalButtonStatus[BUTTONLEFT])){buttonStatus[AXISLY] = 0;buttonStatus[AXISLX] = 0;}
    else if (internalButtonStatus[BUTTONUP]) {buttonStatus[AXISLY] = 0;buttonStatus[AXISLX] = 128;}
    else if (internalButtonStatus[BUTTONDOWN]) {buttonStatus[AXISLY] = 255;buttonStatus[AXISLX] = 128;}
    else if (internalButtonStatus[BUTTONLEFT]) {buttonStatus[AXISLX] = 0;buttonStatus[AXISLY] = 128;}
    else if (internalButtonStatus[BUTTONRIGHT]) {buttonStatus[AXISLX] = 255;buttonStatus[AXISLY] = 128;}
    else {buttonStatus[AXISLX] = 128;buttonStatus[AXISLY] = 128;}

    break;
    
    case RIGHT_ANALOG_MODE:   
    buttonStatus[AXISLX] = 128;
    buttonStatus[AXISLY] = 128;
    buttonStatus[BUTTONUP] = 0;
    buttonStatus[BUTTONDOWN] = 0;
    buttonStatus[BUTTONLEFT] = 0;
    buttonStatus[BUTTONRIGHT] = 0;
    
    if ((internalButtonStatus[BUTTONUP]) && (internalButtonStatus[BUTTONRIGHT])){buttonStatus[AXISRY] = 0;buttonStatus[AXISRX] = 255;}
    else if ((internalButtonStatus[BUTTONUP]) && (internalButtonStatus[BUTTONLEFT])){buttonStatus[AXISRY] = 0;buttonStatus[AXISRX] = 0;}
    else if ((internalButtonStatus[BUTTONDOWN]) && (internalButtonStatus[BUTTONRIGHT])) {buttonStatus[AXISRY] = 255;buttonStatus[AXISRX] = 255;}
    else if ((internalButtonStatus[BUTTONDOWN]) && (internalButtonStatus[BUTTONLEFT])) {buttonStatus[AXISRY] = 255;buttonStatus[AXISRX] = 0;}
    else if (internalButtonStatus[BUTTONUP]) {buttonStatus[AXISRY] = 0;buttonStatus[AXISRX] = 128;}
    else if (internalButtonStatus[BUTTONDOWN]) {buttonStatus[AXISRY] = 255;buttonStatus[AXISRX] = 128;}
    else if (internalButtonStatus[BUTTONLEFT]) {buttonStatus[AXISRX] = 0;buttonStatus[AXISRY] = 128;}
    else if (internalButtonStatus[BUTTONRIGHT]) {buttonStatus[AXISRX] = 255;buttonStatus[AXISRY] = 128;}
    else {buttonStatus[AXISRX] = 128;buttonStatus[AXISRY] = 128;}

    break;
  }
}

void buttonRead()
{  
  if (joystickUP.update()) {internalButtonStatus[BUTTONUP] = joystickUP.fell();}
  if (joystickDOWN.update()) {internalButtonStatus[BUTTONDOWN] = joystickDOWN.fell();}
  if (joystickLEFT.update()) {internalButtonStatus[BUTTONLEFT] = joystickLEFT.fell();}
  if (joystickRIGHT.update()) {internalButtonStatus[BUTTONRIGHT] = joystickRIGHT.fell();}
  if (buttonA.update()) {buttonStatus[BUTTONA] = buttonA.fell();}
  if (buttonB.update()) {buttonStatus[BUTTONB] = buttonB.fell();}
  if (buttonX.update()) {buttonStatus[BUTTONX] = buttonX.fell();}
  if (buttonY.update()) {buttonStatus[BUTTONY] = buttonY.fell();}
  if (buttonLB.update()) {buttonStatus[BUTTONLB] = buttonLB.fell();}
  if (buttonRB.update()) {buttonStatus[BUTTONRB] = buttonRB.fell();}
  if (buttonLT.update()) {buttonStatus[BUTTONLT] = buttonLT.fell();}
  if (buttonRT.update()) {buttonStatus[BUTTONRT] = buttonRT.fell();}
  if (buttonSTART.update()) {buttonStatus[BUTTONSTART] = buttonSTART.fell();}
  if (buttonSELECT.update()) {buttonStatus[BUTTONSELECT] = buttonSELECT.fell();}
  if (buttonHOME.update()) { buttonStatus[BUTTONHOME] = buttonHOME.fell();}

#define HOME_HOTKEY
#ifdef HOME_HOTKEY  
  if(buttonStatus[BUTTONSTART] && buttonStatus[BUTTONSELECT]) {
   if (startAndSelTime == 0)
    startAndSelTime = millis();
   else if (currTime - startAndSelTime > HOME_DELAY)
   {
      buttonStatus[BUTTONHOME] = 1;
   }
 } else {
  startAndSelTime = 0;
  buttonStatus[BUTTONHOME] = 0;
 }
#endif
  
}
