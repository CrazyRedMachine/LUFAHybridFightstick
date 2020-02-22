#include "LUFAConfig.h"
#include <LUFA.h>
#include "Joystick.h"
#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>
#include <EEPROM.h>
#include <inttypes.h>

/* BIT MANIPULATION MACRO */
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define bit_check(value, bit) (((value) >> (bit)) & 0x01)

/* Buttons declarations */
#define MILLIDEBOUNCE 1 //Debounce time in milliseconds
unsigned long startAndSelTime = 0;
unsigned long currTime = 0;
//delay in ms for start+select to become HOME
#define HOME_DELAY 1000
byte buttonStatus[15];

#define DPAD_UP_MASK_ON 0x00
#define DPAD_UPRIGHT_MASK_ON 0x01
#define DPAD_RIGHT_MASK_ON 0x02
#define DPAD_DOWNRIGHT_MASK_ON 0x03
#define DPAD_DOWN_MASK_ON 0x04
#define DPAD_DOWNLEFT_MASK_ON 0x05
#define DPAD_LEFT_MASK_ON 0x06
#define DPAD_UPLEFT_MASK_ON 0x07
#define DPAD_NOTHING_MASK_ON 0x08
#define A_MASK_ON 0x04
#define B_MASK_ON 0x02
#define X_MASK_ON 0x08
#define Y_MASK_ON 0x01
#define LB_MASK_ON 0x10
#define RB_MASK_ON 0x20
#define ZL_MASK_ON 0x40
#define ZR_MASK_ON 0x80
#define START_MASK_ON 0x200
#define SELECT_MASK_ON 0x100
#define HOME_MASK_ON 0x1000

#define BUTTONUP 0
#define BUTTONDOWN 1
#define BUTTONLEFT 2
#define BUTTONRIGHT 3
#define BUTTONA 4
#define BUTTONB 5
#define BUTTONX 6
#define BUTTONY 7
#define BUTTONLB 8
#define BUTTONRB 9
#define BUTTONLT 10
#define BUTTONRT 11
#define BUTTONSTART 12
#define BUTTONSELECT 13
#define BUTTONHOME 14

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
        if (buttonStatus[BUTTONLEFT])
          state = ANALOG_MODE;
        else if (buttonStatus[BUTTONRIGHT])
          state = RIGHT_ANALOG_MODE;
        else if (buttonStatus[BUTTONUP])
          state = DIGITAL;
        
        EEPROM.put(0, state);
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
  
  if (xinput){
    xbox_init(true);
  } else {
  SetupHardware();
  GlobalInterruptEnable();
  }
}


void loop() {
    currTime = millis();
    buttonRead();
    checkModeChange();
  if (xinput){
    loopX();
  } else {
    processButtons();
    HID_Task();
    USB_USBTask();
  }
}

uint8_t pad_up, pad_down, pad_left, pad_right, pad_y, pad_b, pad_x, pad_a, pad_black,
  pad_white, pad_start, pad_select, pad_l3, pad_r3, pad_l, pad_r, pad_left_analog_x,
  pad_left_analog_y, pad_right_analog_x, pad_right_analog_y;
  
void loopX(){
  xbox_reset_watchdog();

    pad_y = buttonStatus[BUTTONY];
    pad_b = buttonStatus[BUTTONB];
    pad_x = buttonStatus[BUTTONX];
    pad_a = buttonStatus[BUTTONA];
    pad_black =  buttonStatus[BUTTONLB];
    pad_white =  buttonStatus[BUTTONRB];
    pad_start =  buttonStatus[BUTTONSTART];
    pad_select =  buttonStatus[BUTTONSELECT];
    pad_l3 =  0;
    pad_r3 =  0;
    pad_l = buttonStatus[BUTTONLT];
    pad_r = buttonStatus[BUTTONRT];

    pad_up = pad_down = pad_left = pad_right = 0;
    pad_left_analog_x = pad_left_analog_y = pad_right_analog_x = pad_right_analog_y = 0x7F;

 switch (state)
  {
    case DIGITAL:
      pad_up = buttonStatus[BUTTONUP];
      pad_down = buttonStatus[BUTTONDOWN];
      pad_left = buttonStatus[BUTTONLEFT];
      pad_right = buttonStatus[BUTTONRIGHT];
    break;

    case ANALOG_MODE:   
       if(buttonStatus[BUTTONLEFT]) {
      pad_left_analog_x = 0x00;
    } else if(buttonStatus[BUTTONRIGHT]) {
      pad_left_analog_x = 0xFF;
    }

    if(buttonStatus[BUTTONUP]) {
      pad_left_analog_y = 0x00;
    } else if(buttonStatus[BUTTONDOWN]) {
      pad_left_analog_y = 0xFF;
    }
    break;
    
    case RIGHT_ANALOG_MODE:   
       if(buttonStatus[BUTTONLEFT]) {
      pad_right_analog_x = 0x00;
    } else if(buttonStatus[BUTTONRIGHT]) {
      pad_right_analog_x = 0xFF;
    }

    if(buttonStatus[BUTTONUP]) {
      pad_right_analog_y = 0x00;
    } else if(buttonStatus[BUTTONDOWN]) {
      pad_right_analog_y = 0xFF;
    }
    break;
  }

    pad_up    ? bit_set(gamepad_state.digital_buttons_1, XBOX_DPAD_UP)    : bit_clear(gamepad_state.digital_buttons_1, XBOX_DPAD_UP);
    pad_down  ? bit_set(gamepad_state.digital_buttons_1, XBOX_DPAD_DOWN)  : bit_clear(gamepad_state.digital_buttons_1, XBOX_DPAD_DOWN);
    pad_left  ? bit_set(gamepad_state.digital_buttons_1, XBOX_DPAD_LEFT)  : bit_clear(gamepad_state.digital_buttons_1, XBOX_DPAD_LEFT);
    pad_right ? bit_set(gamepad_state.digital_buttons_1, XBOX_DPAD_RIGHT) : bit_clear(gamepad_state.digital_buttons_1, XBOX_DPAD_RIGHT);

    pad_start  ? bit_set(gamepad_state.digital_buttons_1, XBOX_START)       : bit_clear(gamepad_state.digital_buttons_1, XBOX_START);
    pad_select ? bit_set(gamepad_state.digital_buttons_1, XBOX_BACK)        : bit_clear(gamepad_state.digital_buttons_1, XBOX_BACK);
    pad_l3     ? bit_set(gamepad_state.digital_buttons_1, XBOX_LEFT_STICK)  : bit_clear(gamepad_state.digital_buttons_1, XBOX_LEFT_STICK);
    pad_r3     ? bit_set(gamepad_state.digital_buttons_1, XBOX_RIGHT_STICK) : bit_clear(gamepad_state.digital_buttons_1, XBOX_RIGHT_STICK);

    pad_a ? bit_set(gamepad_state.digital_buttons_2, XBOX_A)    : bit_clear(gamepad_state.digital_buttons_2, XBOX_A);
    pad_b ? bit_set(gamepad_state.digital_buttons_2, XBOX_B)  : bit_clear(gamepad_state.digital_buttons_2, XBOX_B);
    pad_x ? bit_set(gamepad_state.digital_buttons_2, XBOX_X)  : bit_clear(gamepad_state.digital_buttons_2, XBOX_X);
    pad_y ? bit_set(gamepad_state.digital_buttons_2, XBOX_Y) : bit_clear(gamepad_state.digital_buttons_2, XBOX_Y);

    pad_black ? bit_set(gamepad_state.digital_buttons_2, XBOX_LB)    : bit_clear(gamepad_state.digital_buttons_2, XBOX_LB);
    pad_white ? bit_set(gamepad_state.digital_buttons_2, XBOX_RB)    : bit_clear(gamepad_state.digital_buttons_2, XBOX_RB);

    if(pad_start && pad_select) {
      if (startAndSelTime == 0)
       startAndSelTime = millis();
      else if (currTime - startAndSelTime > HOME_DELAY)
      {
        bit_set(gamepad_state.digital_buttons_2, XBOX_HOME);
        bit_clear(gamepad_state.digital_buttons_1, XBOX_START);
        bit_clear(gamepad_state.digital_buttons_1, XBOX_BACK);
      }
    } else {
      startAndSelTime = 0;
      bit_clear(gamepad_state.digital_buttons_2, XBOX_HOME);
    }

    gamepad_state.l_x = pad_left_analog_x * 257 + -32768;
    gamepad_state.l_y = pad_left_analog_y * -257 + 32767;
    gamepad_state.r_x = pad_right_analog_x * 257 + -32768;
    gamepad_state.r_y = pad_right_analog_y * -257 + 32767;

    gamepad_state.lt = pad_l * 0xFF;
    gamepad_state.rt = pad_r * 0xFF;

    xbox_send_pad_state();
}

void buttonRead()
{
  if (joystickUP.update()) {buttonStatus[BUTTONUP] = joystickUP.fell();}
  if (joystickDOWN.update()) {buttonStatus[BUTTONDOWN] = joystickDOWN.fell();}
  if (joystickLEFT.update()) {buttonStatus[BUTTONLEFT] = joystickLEFT.fell();}
  if (joystickRIGHT.update()) {buttonStatus[BUTTONRIGHT] = joystickRIGHT.fell();}
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
  if (buttonHOME.update()) {buttonStatus[BUTTONHOME] = buttonHOME.fell();}
}


void processDPAD(){
    ReportData.LX = 128;
    ReportData.LY = 128;
    ReportData.RX = 128;
    ReportData.RY = 128;
 
    if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])){ReportData.HAT = DPAD_UPRIGHT_MASK_ON;}
    else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {ReportData.HAT = DPAD_DOWNRIGHT_MASK_ON;} 
    else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {ReportData.HAT = DPAD_DOWNLEFT_MASK_ON;}
    else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])){ReportData.HAT = DPAD_UPLEFT_MASK_ON;}
    else if (buttonStatus[BUTTONUP]) {ReportData.HAT = DPAD_UP_MASK_ON;}
    else if (buttonStatus[BUTTONDOWN]) {ReportData.HAT = DPAD_DOWN_MASK_ON;}
    else if (buttonStatus[BUTTONLEFT]) {ReportData.HAT = DPAD_LEFT_MASK_ON;}
    else if (buttonStatus[BUTTONRIGHT]) {ReportData.HAT = DPAD_RIGHT_MASK_ON;}
    else{ReportData.HAT = DPAD_NOTHING_MASK_ON;}
}
void processLANALOG(){
    ReportData.HAT = DPAD_NOTHING_MASK_ON;
    ReportData.RX = 128;
    ReportData.RY = 128;

    if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])){ReportData.LY = 0;ReportData.LX = 255;}
    else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {ReportData.LY = 255;ReportData.LX = 255;}
    else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {ReportData.LY = 255;ReportData.LX = 0;}
    else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])){ReportData.LY = 0;ReportData.LX = 0;}
    else if (buttonStatus[BUTTONUP]) {ReportData.LY = 0;ReportData.LX = 128;}
    else if (buttonStatus[BUTTONDOWN]) {ReportData.LY = 255;ReportData.LX = 128;}
    else if (buttonStatus[BUTTONLEFT]) {ReportData.LX = 0;ReportData.LY = 128;}
    else if (buttonStatus[BUTTONRIGHT]) {ReportData.LX = 255;ReportData.LY = 128;}
    else {ReportData.LX = 128;ReportData.LY = 128;}
}
void processLANALOGSmash(){
    ReportData.HAT = DPAD_NOTHING_MASK_ON;
    ReportData.RX = 128;
    ReportData.RY = 128;
    
    if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])){ReportData.LY = 64;ReportData.LX = 192;}
    else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {ReportData.LY = 192;ReportData.LX = 192;}
    else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {ReportData.LY = 192;ReportData.LX = 64;}  
    else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])){ReportData.LY = 64;ReportData.LX = 64;}
    else if (buttonStatus[BUTTONUP]) {ReportData.LY = 64;ReportData.LX = 128;}
    else if (buttonStatus[BUTTONDOWN]) {ReportData.LY = 192;ReportData.LX = 128;}
    else if (buttonStatus[BUTTONLEFT]) {ReportData.LX = 64;ReportData.LY = 128;}
    else if (buttonStatus[BUTTONRIGHT]) {ReportData.LX = 192;ReportData.LY = 128;}
    else{ReportData.LX = 128;ReportData.LY = 128;}
}
void processRANALOG(){
    ReportData.HAT = 0x08;
    ReportData.LX = 128;
    ReportData.LY = 128;
    
    if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])){ReportData.RY = 0;ReportData.RX = 255;}
    else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])){ReportData.RY = 0;ReportData.RX = 0;}
    else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {ReportData.RY = 255;ReportData.RX = 255;}
    else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {ReportData.RY = 255;ReportData.RX = 0;}
    else if (buttonStatus[BUTTONUP]) {ReportData.RY = 0;ReportData.RX = 128;}
    else if (buttonStatus[BUTTONDOWN]) {ReportData.RY = 255;ReportData.RX = 128;}
    else if (buttonStatus[BUTTONLEFT]) {ReportData.RX = 0;ReportData.RY = 128;}
    else if (buttonStatus[BUTTONRIGHT]) {ReportData.RX = 255;ReportData.RY = 128;}
    else {ReportData.RX = 128;ReportData.RY = 128;}
    
}
void processButtons(){
  //state gets set with checkModeChange
  switch (state)
  {
    case DIGITAL:
        processDPAD();
        buttonProcessing();
    break;

    case ANALOG_MODE:   
       processLANALOG();
       buttonProcessing();
    break;
    
    case RIGHT_ANALOG_MODE:   
       processRANALOG();
       buttonProcessing();
    break;
  }
}
void buttonProcessing(){
  if (buttonStatus[BUTTONA]) {ReportData.Button |= A_MASK_ON;}
  if (buttonStatus[BUTTONB]) {ReportData.Button |= B_MASK_ON;}
  if (buttonStatus[BUTTONX]) {ReportData.Button |= X_MASK_ON;}
  if (buttonStatus[BUTTONY]) {ReportData.Button |= Y_MASK_ON;}
  if (buttonStatus[BUTTONLB]) {ReportData.Button |= LB_MASK_ON;}
  if (buttonStatus[BUTTONRB]) {ReportData.Button |= RB_MASK_ON;}
  if (buttonStatus[BUTTONLT]) {ReportData.Button |= ZL_MASK_ON;}
  if (buttonStatus[BUTTONRT]) {ReportData.Button |= ZR_MASK_ON;}
  if (buttonStatus[BUTTONSTART]){ReportData.Button |= START_MASK_ON;}
  if (buttonStatus[BUTTONSELECT]){ReportData.Button |= SELECT_MASK_ON;}
  if (buttonStatus[BUTTONHOME]){ReportData.Button |= HOME_MASK_ON;}

  if(buttonStatus[BUTTONSTART] && buttonStatus[BUTTONSELECT]) {
   if (startAndSelTime == 0)
    startAndSelTime = millis();
   else if (currTime - startAndSelTime > HOME_DELAY)
   {
      ReportData.Button |= HOME_MASK_ON;
      ReportData.Button &= ~START_MASK_ON;
      ReportData.Button &= ~SELECT_MASK_ON;
   }
 } else {
  startAndSelTime = 0;
 }
  
}
