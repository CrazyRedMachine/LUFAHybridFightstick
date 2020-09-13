#include "LUFAConfig.h"
#include <LUFA.h>
#include "XS_HID.h"
#include <EEPROM.h>
#include <inttypes.h>
//delay in ms for start+select to become HOME
#define HOME_DELAY 1000

/** PINS **/
int DATA_CLOCK    = 6;
int DATA_LATCH    = 3;
int DATA_SERIAL   = 4;
/** Data store for current state of buttons **/
byte buttons[16];

/* Buttons declarations */
unsigned long startAndSelTime = 0;
unsigned long currTime = 0;

byte internalButtonStatus[4];

/* MODE DECLARATIONS */
typedef enum {
  RIGHT_ANALOG_MODE,
  ANALOG_MODE,
  DIGITAL,
} State_t;
State_t state;

/* mode selectors */
bool xinput;

void setupPins(){
  /** Set DATA_CLOCK normally HIGH **/
  pinMode (DATA_CLOCK, OUTPUT);
  digitalWrite (DATA_CLOCK, HIGH);
  
  /** Set DATA_LATCH normally HIGH **/
  pinMode (DATA_LATCH, OUTPUT);
  digitalWrite (DATA_LATCH, HIGH);

  /** Set DATA_SERIAL normally HIGH **/
  pinMode (DATA_SERIAL, OUTPUT);
  digitalWrite (DATA_SERIAL, HIGH);
  pinMode (DATA_SERIAL, INPUT);  
}

void RXTXControllerData () {
  /** Latch for 12us **/
  digitalWrite(DATA_LATCH, LOW);
  delayMicroseconds(12);
  digitalWrite(DATA_LATCH, HIGH);
  delayMicroseconds(6);

  /** Read data bit by bit from SR **/
  for (int i = 0; i < 16; i++) {
    digitalWrite (DATA_CLOCK, LOW);
    delayMicroseconds (6);
      buttons[i] = !digitalRead (DATA_SERIAL);
    digitalWrite (DATA_CLOCK, HIGH);
    delayMicroseconds (6);
  }
}

void setup() {
  EEPROM.get(0, state);
  EEPROM.get(2, xinput);
  setupPins();
  delay(3000);
  RXTXControllerData();
// if select is held on boot, NSWitch mode
  if (buttons[6])
  {
    xinput = false;
    EEPROM.put(2, xinput);
  }
// if start is held on boot, XInput mode
  else {
    if (buttons[7])
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
RXTXControllerData();

buttonStatus[BUTTONRB] = buttons[0];
buttonStatus[BUTTONA] = buttons[1];
buttonStatus[BUTTONB] = buttons[2];
buttonStatus[BUTTONY] = buttons[3];
buttonStatus[BUTTONX] = buttons[4];
buttonStatus[BUTTONLB] = buttons[5];
buttonStatus[BUTTONSELECT] = buttons[6];
buttonStatus[BUTTONSTART] = buttons[7];
internalButtonStatus[BUTTONUP] = buttons[8];
internalButtonStatus[BUTTONRIGHT] = buttons[9];
internalButtonStatus[BUTTONDOWN] = buttons[10];
internalButtonStatus[BUTTONLEFT] = buttons[11];
byte modeDPAD = buttons[12];
byte modeAR = buttons[14];

if (modeDPAD)
  state = DIGITAL;
else
 if (modeAR) state = RIGHT_ANALOG_MODE;
  else state = ANALOG_MODE;

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
