#include "LUFAConfig.h"
#include <LUFA.h>
#include "XS_HID.h"
#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>
#include <EEPROM.h>
#include <inttypes.h>

/* in case you want to disable one type of gamepad */
//#define DISABLE_NSWITCH 
//#define DISABLE_XINPUT

//make it so holding start+select triggers the HOME button
//#define HOME_HOTKEY
//delay in ms for start+select to become HOME in HOME_HOTKEY mode
#define HOME_DELAY 1000

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
Bounce buttonL = Bounce();
Bounce buttonR = Bounce();
Bounce buttonZL = Bounce();
Bounce buttonZR = Bounce();
Bounce buttonPLUS = Bounce();
Bounce buttonMINUS = Bounce();
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
    joystickUP.attach(PIN_UP,INPUT_PULLUP);
    joystickDOWN.attach(PIN_DOWN,INPUT_PULLUP);
    joystickLEFT.attach(PIN_LEFT,INPUT_PULLUP);
    joystickRIGHT.attach(PIN_RIGHT,INPUT_PULLUP);
    buttonA.attach(PIN_A,INPUT_PULLUP);      // XBOX B
    buttonB.attach(PIN_B,INPUT_PULLUP);      // XBOX A
    buttonX.attach(PIN_X,INPUT_PULLUP);      // XBOX Y
    buttonY.attach(PIN_Y,INPUT_PULLUP);     // XBOX X
    buttonL.attach(PIN_L,INPUT_PULLUP);     // XBOX LB
    buttonR.attach(PIN_R,INPUT_PULLUP);      // XBOX RB
    buttonZL.attach(PIN_ZL,INPUT_PULLUP);     // XBOX LT
    buttonZR.attach(PIN_ZR,INPUT_PULLUP);     // XBOX RT
    buttonPLUS.attach(PIN_PLUS,INPUT_PULLUP);  // XBOX START
    buttonMINUS.attach(PIN_MINUS,INPUT_PULLUP); // XBOX BACK
    buttonHOME.attach(PIN_HOME,INPUT_PULLUP);

    joystickUP.interval(MILLIDEBOUNCE);
    joystickDOWN.interval(MILLIDEBOUNCE);
    joystickLEFT.interval(MILLIDEBOUNCE);
    joystickRIGHT.interval(MILLIDEBOUNCE);
    buttonA.interval(MILLIDEBOUNCE);
    buttonB.interval(MILLIDEBOUNCE);
    buttonX.interval(MILLIDEBOUNCE);
    buttonY.interval(MILLIDEBOUNCE);
    buttonL.interval(MILLIDEBOUNCE);
    buttonR.interval(MILLIDEBOUNCE);
    buttonZL.interval(MILLIDEBOUNCE);
    buttonZR.interval(MILLIDEBOUNCE);
    buttonPLUS.interval(MILLIDEBOUNCE);
    buttonMINUS.interval(MILLIDEBOUNCE);
    buttonHOME.interval(MILLIDEBOUNCE);
}
void setup() {

  modeChanged = false;
  EEPROM.get(0, state);
  EEPROM.get(2, xinput);
  setupPins();
  delay(500);

#ifdef DISABLE_NSWITCH
#warning "NSWITCH mode is disabled, will act only as an XInput controller"
/* force Xinput */
  xinput = true;
#else
#ifdef DISABLE_XINPUT
#warning "XINPUT mode is disabled, will act only as a Nintendo switch controller"
/* force nswitch */
  xinput = false;
#else
/* set xinput mode according to held button */
// if select is held on boot, NSWitch mode
  int value = digitalRead(PIN_MINUS);
  if (value == LOW)
  {
    xinput = false;
    EEPROM.put(2, xinput);
  }
// if start is held on boot, XInput mode
  else {
    value = digitalRead(PIN_PLUS);
    if (value == LOW)
      {
        xinput = true;
        EEPROM.put(2, xinput);
      }
  }
#endif
#endif  
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
    
    case ANALOG_MODE:
      /* fallthrough */
    default:  
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
  if (buttonL.update()) {buttonStatus[BUTTONLB] = buttonL.fell();}
  if (buttonR.update()) {buttonStatus[BUTTONRB] = buttonR.fell();}
  if (buttonZL.update()) {buttonStatus[BUTTONLT] = buttonZL.fell();}
  if (buttonZR.update()) {buttonStatus[BUTTONRT] = buttonZR.fell();}
  if (buttonPLUS.update()) {buttonStatus[BUTTONSTART] = buttonPLUS.fell();}
  if (buttonMINUS.update()) {buttonStatus[BUTTONSELECT] = buttonMINUS.fell();}
  if (buttonHOME.update()) { buttonStatus[BUTTONHOME] = buttonHOME.fell();}

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
