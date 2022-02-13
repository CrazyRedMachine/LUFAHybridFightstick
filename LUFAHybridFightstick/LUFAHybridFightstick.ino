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

//use real analog sticks
//#define WITH_ANALOG
//#define DEADZONE     50

// Enable on-the-fly SOCD config. If disabled, it'll lock in
// the default configuration but still use the SOCD resolution code.
// #define ENABLE_SOCD_CONFIG

//make it so holding start+select triggers the HOME button
//#define HOME_HOTKEY
//delay in ms for start+select to become HOME in HOME_HOTKEY mode
#define HOME_DELAY 1000

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

typedef struct point_s {
  int x;
  int y;
} point_t;

typedef struct range_s {
  point_t min;
  point_t max;
  point_t center;
} range_t;

range_t g_range_l = {{400, 400}, {600, 600}, {511, 511}};
range_t g_range_r = {{400, 400}, {600, 600}, {511, 511}};

bool g_calibrating = false;
#endif

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
Bounce buttonLS = Bounce();
Bounce buttonRS = Bounce();
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

typedef enum {
  NEUTRAL,    // LEFT/UP + DOWN/RIGHT = NEUTRAL
  NEGATIVE,   // LEFT/UP beats DOWN/RIGHT
  POSITIVE,   // DOWN/RIGHT beats LEFT/UP
  LAST_INPUT, //Last input has priority; not a valid state if being used for initial_input
} Socd_t;
Socd_t x_socd_type = NEUTRAL; // controls left/right and up/down resolution type
Socd_t y_socd_type = NEGATIVE;
Socd_t x_initial_input, y_initial_input = NEUTRAL;

/* mode selectors */
bool xinput;
bool modeChanged;

void checkModeChange() {
  if (buttonStatus[BUTTONSTART] && buttonStatus[BUTTONSELECT])
  {
#ifdef ENABLE_SOCD_CONFIG
    if (buttonStatus[BUTTONL3] && buttonStatus[BUTTONR3])
    {
      if (!modeChanged)
      {
        // read inputs at time of press
        bool up = !joystickUP.read();
        bool down = !joystickDOWN.read();
        bool left = !joystickLEFT.read();
        bool right = !joystickRIGHT.read();

        if (up && down)
          y_socd_type = LAST_INPUT;
        else if (up)
          y_socd_type = NEGATIVE;
        else if (down)
          y_socd_type = POSITIVE;
        else if (!up && !down)
          y_socd_type = NEUTRAL;

        if (left && right)
          x_socd_type = LAST_INPUT;
        else if (left)
          x_socd_type = NEGATIVE;
        else if (right)
          x_socd_type = POSITIVE;
        else if (!left && !right)
          x_socd_type = NEUTRAL;

        EEPROM.put(4, x_socd_type);
        EEPROM.put(6, y_socd_type);
        modeChanged = true;
      }
    }
    else 
#endif   
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
    else
    {
      modeChanged = false;
    }
  }
}

void setupPins() {
  joystickUP.attach(PIN_UP, INPUT_PULLUP);
  joystickDOWN.attach(PIN_DOWN, INPUT_PULLUP);
  joystickLEFT.attach(PIN_LEFT, INPUT_PULLUP);
  joystickRIGHT.attach(PIN_RIGHT, INPUT_PULLUP);
  buttonA.attach(PIN_A, INPUT_PULLUP);     // XBOX B
  buttonB.attach(PIN_B, INPUT_PULLUP);     // XBOX A
  buttonX.attach(PIN_X, INPUT_PULLUP);     // XBOX Y
  buttonY.attach(PIN_Y, INPUT_PULLUP);    // XBOX X
  buttonL.attach(PIN_L, INPUT_PULLUP);    // XBOX LB
  buttonR.attach(PIN_R, INPUT_PULLUP);     // XBOX RB
  buttonZL.attach(PIN_ZL, INPUT_PULLUP);    // XBOX LT
  buttonZR.attach(PIN_ZR, INPUT_PULLUP);    // XBOX RT
  buttonLS.attach(PIN_LS, INPUT_PULLUP);       // XBOX LS
  buttonRS.attach(PIN_RS, INPUT_PULLUP);       // XBOX RS
  buttonPLUS.attach(PIN_PLUS, INPUT_PULLUP);   // XBOX START
  buttonMINUS.attach(PIN_MINUS, INPUT_PULLUP); // XBOX BACK
  buttonHOME.attach(PIN_HOME, INPUT_PULLUP);

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
  buttonLS.interval(MILLIDEBOUNCE);
  buttonRS.interval(MILLIDEBOUNCE);
  buttonPLUS.interval(MILLIDEBOUNCE);
  buttonMINUS.interval(MILLIDEBOUNCE);
  buttonHOME.interval(MILLIDEBOUNCE);
}

void setup() {

  modeChanged = false;
  EEPROM.get(0, state);
  EEPROM.get(2, xinput);
#ifdef ENABLE_SOCD_CONFIG
  EEPROM.get(4, x_socd_type);
  EEPROM.get(6, y_socd_type);
#endif
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

#ifdef WITH_ANALOG
  if (digitalRead(PIN_LS) == LOW && digitalRead(PIN_RS) == LOW)
  {
    g_calibrating = true;
  }

  g_range_l.center.x = analogRead(PIN_LANALOGX);
  g_range_l.center.y = analogRead(PIN_LANALOGY);
  g_range_r.center.x = analogRead(PIN_RANALOGX);
  g_range_r.center.y = analogRead(PIN_RANALOGY);

  if (!g_calibrating)
  {
    EEPROM.get(8, g_range_l.min.x);
    EEPROM.get(10, g_range_l.min.y);
    EEPROM.get(12, g_range_l.max.x);
    EEPROM.get(14, g_range_l.max.y);

    EEPROM.get(16, g_range_r.min.x);
    EEPROM.get(18, g_range_r.min.y);
    EEPROM.get(20, g_range_r.max.x);
    EEPROM.get(22, g_range_r.max.y);
  }
#endif

  SetupHardware(xinput);
  GlobalInterruptEnable();
}


void loop() {
  currTime = millis();
#ifdef WITH_ANALOG
  axisRead();
#endif
  buttonRead();
  checkModeChange();
  convert_dpad();
  send_pad_state();
}

#ifdef WITH_ANALOG
void axisRead()
{
  point_t curr;

  // left analog X
  curr.x = analogRead(PIN_LANALOGX);
  curr.y = analogRead(PIN_LANALOGY);

  if ((curr.x - g_range_l.center.x < DEADZONE) && (curr.x - g_range_l.center.x > -DEADZONE))
    buttonStatus[AXISLX] = 127;
  else if (curr.x < g_range_l.min.x) {
    g_range_l.min.x = curr.x - 10;

    if (g_calibrating)
      EEPROM.put(8, g_range_l.min.x);

    buttonStatus[AXISLX] = 0;
  }
  else if (curr.x > g_range_l.max.x) {
    g_range_l.max.x = curr.x + 10;
    if (g_calibrating)
      EEPROM.put(12, g_range_l.max.x);
    buttonStatus[AXISLX] = 255;
  } else if (curr.x > g_range_l.center.x) {
    buttonStatus[AXISLX] = map(curr.x, g_range_l.center.x, g_range_l.max.x, 127, 255);
  } else if (curr.x < g_range_l.center.x) {
    buttonStatus[AXISLX] = map(curr.x, g_range_l.min.x, g_range_l.center.x, 0, 127);
  }
  buttonStatus[AXISLX] *= -1;

  if ((curr.y - g_range_l.center.y < DEADZONE) && (curr.y - g_range_l.center.y > -DEADZONE))
    buttonStatus[AXISLY] = 127;
  else if (curr.y < g_range_l.min.y) {
    g_range_l.min.y = curr.y - 10;
    if (g_calibrating)
      EEPROM.put(10, g_range_l.min.y);
    buttonStatus[AXISLY] = 0;
  }
  else if (curr.y > g_range_l.max.y) {
    g_range_l.max.y = curr.y + 10;
    if (g_calibrating)
      EEPROM.put(14, g_range_l.max.y);
    buttonStatus[AXISLY] = 255;
  } else if (curr.y > g_range_l.center.y) {
    buttonStatus[AXISLY] = map(curr.y, g_range_l.center.y, g_range_l.max.y, 127, 255);
  } else if (curr.y < g_range_l.center.y) {
    buttonStatus[AXISLY] = map(curr.y, g_range_l.min.y, g_range_l.center.y, 0, 127);
  }
  buttonStatus[AXISLY] *= -1;


  // right analog

  curr.x = analogRead(PIN_RANALOGX);
  curr.y = analogRead(PIN_RANALOGY);

  if ((curr.x - g_range_r.center.x < 50) && (curr.x - g_range_r.center.x > -50))
    buttonStatus[AXISRX] = 127;
  else if (curr.x < g_range_r.min.x) {
    g_range_r.min.x = curr.x - 10;
    if (g_calibrating)
      EEPROM.put(16, g_range_r.min.x);
    buttonStatus[AXISRX] = 0;
  }
  else if (curr.x > g_range_r.max.x) {
    g_range_r.max.x = curr.x + 10;
    if (g_calibrating)
      EEPROM.put(20, g_range_r.max.x);
    buttonStatus[AXISRX] = 255;
  } else if (curr.x > g_range_r.center.x) {
    buttonStatus[AXISRX] = map(curr.x, g_range_r.center.x, g_range_r.max.x, 127, 255);
  } else if (curr.x < g_range_r.center.x) {
    buttonStatus[AXISRX] = map(curr.x, g_range_r.min.x, g_range_r.center.x, 0, 127);
  }
  buttonStatus[AXISRX] *= -1;


  if ((curr.y - g_range_r.center.y < 50) && (curr.y - g_range_r.center.y > -50))
    buttonStatus[AXISRY] = 127;
  else if (curr.y < g_range_r.min.y) {
    g_range_r.min.y = curr.y - 10;
    if (g_calibrating)
      EEPROM.put(18, g_range_r.min.y);
    buttonStatus[AXISRY] = 0;
  }
  else if (curr.y > g_range_r.max.y) {
    g_range_r.max.y = curr.y + 10;
    if (g_calibrating)
      EEPROM.put(22, g_range_r.max.y);
    buttonStatus[AXISRY] = 255;
  } else if (curr.y > g_range_r.center.y) {
    buttonStatus[AXISRY] = map(curr.y, g_range_r.center.y, g_range_r.max.y, 127, 255);
  } else if (curr.y < g_range_r.center.y) {
    buttonStatus[AXISRY] = map(curr.y, g_range_r.min.y, g_range_r.center.y, 0, 127);
  }
  buttonStatus[AXISRY] *= -1;

}
#endif

void convert_dpad() {
  byte cleanButtonStatus[4] = {0};
  // Prevent SOCD inputs (left+right or up+down) from making it to the logic below.
  clean_all_socd(internalButtonStatus, cleanButtonStatus, x_socd_type, y_socd_type);

#ifdef WITH_ANALOG
  // force digital mode for dpad (TODO: allow the other modes as well)
  buttonStatus[BUTTONUP] = cleanButtonStatus[BUTTONUP];
  buttonStatus[BUTTONDOWN] = cleanButtonStatus[BUTTONDOWN];
  buttonStatus[BUTTONLEFT] = cleanButtonStatus[BUTTONLEFT];
  buttonStatus[BUTTONRIGHT] = cleanButtonStatus[BUTTONRIGHT];
  return;
#endif

  switch (state)
  {
    case DIGITAL:
      buttonStatus[AXISLX] = 128;
      buttonStatus[AXISLY] = 128;
      buttonStatus[AXISRX] = 128;
      buttonStatus[AXISRY] = 128;
      buttonStatus[BUTTONUP] = cleanButtonStatus[BUTTONUP];
      buttonStatus[BUTTONDOWN] = cleanButtonStatus[BUTTONDOWN];
      buttonStatus[BUTTONLEFT] = cleanButtonStatus[BUTTONLEFT];
      buttonStatus[BUTTONRIGHT] = cleanButtonStatus[BUTTONRIGHT];
      break;

    case RIGHT_ANALOG_MODE:
      buttonStatus[AXISLX] = 128;
      buttonStatus[AXISLY] = 128;
      buttonStatus[BUTTONUP] = 0;
      buttonStatus[BUTTONDOWN] = 0;
      buttonStatus[BUTTONLEFT] = 0;
      buttonStatus[BUTTONRIGHT] = 0;

      if ((cleanButtonStatus[BUTTONUP]) && (cleanButtonStatus[BUTTONRIGHT])) {
        buttonStatus[AXISRY] = 0;
        buttonStatus[AXISRX] = 255;
      }
      else if ((cleanButtonStatus[BUTTONUP]) && (cleanButtonStatus[BUTTONLEFT])) {
        buttonStatus[AXISRY] = 0;
        buttonStatus[AXISRX] = 0;
      }
      else if ((cleanButtonStatus[BUTTONDOWN]) && (cleanButtonStatus[BUTTONRIGHT])) {
        buttonStatus[AXISRY] = 255;
        buttonStatus[AXISRX] = 255;
      }
      else if ((cleanButtonStatus[BUTTONDOWN]) && (cleanButtonStatus[BUTTONLEFT])) {
        buttonStatus[AXISRY] = 255;
        buttonStatus[AXISRX] = 0;
      }
      else if (cleanButtonStatus[BUTTONUP]) {
        buttonStatus[AXISRY] = 0;
        buttonStatus[AXISRX] = 128;
      }
      else if (cleanButtonStatus[BUTTONDOWN]) {
        buttonStatus[AXISRY] = 255;
        buttonStatus[AXISRX] = 128;
      }
      else if (cleanButtonStatus[BUTTONLEFT]) {
        buttonStatus[AXISRX] = 0;
        buttonStatus[AXISRY] = 128;
      }
      else if (cleanButtonStatus[BUTTONRIGHT]) {
        buttonStatus[AXISRX] = 255;
        buttonStatus[AXISRY] = 128;
      }
      else {
        buttonStatus[AXISRX] = 128;
        buttonStatus[AXISRY] = 128;
      }

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

      if ((cleanButtonStatus[BUTTONUP]) && (cleanButtonStatus[BUTTONRIGHT])) {
        buttonStatus[AXISLY] = 0;
        buttonStatus[AXISLX] = 255;
      }
      else if ((cleanButtonStatus[BUTTONDOWN]) && (cleanButtonStatus[BUTTONRIGHT])) {
        buttonStatus[AXISLY] = 255;
        buttonStatus[AXISLX] = 255;
      }
      else if ((cleanButtonStatus[BUTTONDOWN]) && (cleanButtonStatus[BUTTONLEFT])) {
        buttonStatus[AXISLY] = 255;
        buttonStatus[AXISLX] = 0;
      }
      else if ((cleanButtonStatus[BUTTONUP]) && (cleanButtonStatus[BUTTONLEFT])) {
        buttonStatus[AXISLY] = 0;
        buttonStatus[AXISLX] = 0;
      }
      else if (cleanButtonStatus[BUTTONUP]) {
        buttonStatus[AXISLY] = 0;
        buttonStatus[AXISLX] = 128;
      }
      else if (cleanButtonStatus[BUTTONDOWN]) {
        buttonStatus[AXISLY] = 255;
        buttonStatus[AXISLX] = 128;
      }
      else if (cleanButtonStatus[BUTTONLEFT]) {
        buttonStatus[AXISLX] = 0;
        buttonStatus[AXISLY] = 128;
      }
      else if (cleanButtonStatus[BUTTONRIGHT]) {
        buttonStatus[AXISLX] = 255;
        buttonStatus[AXISLY] = 128;
      }
      else {
        buttonStatus[AXISLX] = 128;
        buttonStatus[AXISLY] = 128;
      }

      break;


  }
}

void buttonRead()
{
  // for SOCD cleaning to work properly we need directions to update
  // on any change instead of on fall
  joystickUP.update(); joystickDOWN.update(); joystickLEFT.update(); joystickRIGHT.update();
  if (joystickUP.changed() || joystickDOWN.changed() || joystickLEFT.changed() || joystickRIGHT.changed())
  {
    internalButtonStatus[BUTTONUP] = !joystickUP.read();
    internalButtonStatus[BUTTONDOWN] = !joystickDOWN.read();
    internalButtonStatus[BUTTONLEFT] = !joystickLEFT.read();
    internalButtonStatus[BUTTONRIGHT] = !joystickRIGHT.read();
  }
  if (buttonA.update()) {
    buttonStatus[BUTTONA] = buttonA.fell();
  }
  if (buttonB.update()) {
    buttonStatus[BUTTONB] = buttonB.fell();
  }
  if (buttonX.update()) {
    buttonStatus[BUTTONX] = buttonX.fell();
  }
  if (buttonY.update()) {
    buttonStatus[BUTTONY] = buttonY.fell();
  }
  if (buttonL.update()) {
    buttonStatus[BUTTONLB] = buttonL.fell();
  }
  if (buttonR.update()) {
    buttonStatus[BUTTONRB] = buttonR.fell();
  }
  if (buttonZL.update()) {
    buttonStatus[BUTTONLT] = buttonZL.fell();
  }
  if (buttonZR.update()) {
    buttonStatus[BUTTONRT] = buttonZR.fell();
  }
  if (buttonLS.update()) {
    buttonStatus[BUTTONL3] = buttonLS.fell();
  }
  if (buttonRS.update()) {
    buttonStatus[BUTTONR3] = buttonRS.fell();
  }
  if (buttonPLUS.update()) {
    buttonStatus[BUTTONSTART] = buttonPLUS.fell();
  }
  if (buttonMINUS.update()) {
    buttonStatus[BUTTONSELECT] = buttonMINUS.fell();
  }
  if (buttonHOME.update()) {
    buttonStatus[BUTTONHOME] = buttonHOME.fell();
  }

#ifdef HOME_HOTKEY
  if (buttonStatus[BUTTONSTART] && buttonStatus[BUTTONSELECT]) {
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


void clean_all_socd(byte *internalButtonStatus, byte *cleanButtonStatus, Socd_t x_socd_type, Socd_t y_socd_type)
{
  clean_socd(internalButtonStatus[BUTTONLEFT], &cleanButtonStatus[BUTTONLEFT], internalButtonStatus[BUTTONRIGHT], &cleanButtonStatus[BUTTONRIGHT], x_socd_type, &x_initial_input);
  clean_socd(internalButtonStatus[BUTTONUP], &cleanButtonStatus[BUTTONUP], internalButtonStatus[BUTTONDOWN], &cleanButtonStatus[BUTTONDOWN], y_socd_type, &y_initial_input);
}
/**
   Cleans the given (possible) simultaneous opposite cardinal direction inputs according to the preferences provided.

   @note Given two simultaneous opposite cardinal direction inputs, clean_socd will
   make sure that both are not actually sent. The method used to resolve this conflict
   is determined by input_priority. The x (LEFT/RIGHT) and y (UP/DOWN) axes can be
   handled with the same logic as long as the negative and positive inputs are correctly
   arranged, so pointers are used to make the same function handle both.

   @param[in] negative_in The LEFT/UP input variable.
   @param[out] negative_out The LEFT/UP resulting value.
   @param[in] positive_in  The DOWN/RIGHT input variable.
   @param[out] positive_out  The DOWN/RIGHT resulting value.
   @param[in] input_priority Determines the SOCD resolution method used. @see Socd_t for how each resolution method works.
   @param[in,out] initial_input If input_priority = LAST_INPUT and SOCD cleaning is needed, this is used to determine
    which input was made last. If only one input is made, this variable is set to that input, even if input_priority != LAST_INPUT.
*/
void clean_socd(byte negative_in, byte *negative_out, byte positive_in, byte *positive_out, Socd_t input_priority, Socd_t *initial_input)
{
  if (negative_in && positive_in) // SOCD that needs to be resolved
  {
    switch (input_priority)
    {
      case NEUTRAL:
        *negative_out = *positive_out = false;
        break;
      case NEGATIVE:
        *negative_out = true;
        *positive_out = false;
        break;
      case POSITIVE:
        *negative_out = false;
        *positive_out = true;
        break;
      case LAST_INPUT:
        // Check which input was made first to figure out which input was made last, which wins.
        switch (*initial_input)
        {
          case NEGATIVE:
            *negative_out = false;
            *positive_out = true;
            break;
          case POSITIVE:
            *negative_out = true;
            *positive_out = false;
            break;
          // This is a fallback case for when there hasn't been an input since starting up.
          case NEUTRAL:
            *negative_out = *positive_out = false;
            break;
        }
    }
  }
  else // no SOCD to resolve, which means our current input (if any) can be output as is, but also should be remembered as the initial input.
  {
    *positive_out = positive_in;
    *negative_out = negative_in;
    if (negative_in && !positive_in)
      *initial_input = NEGATIVE;
    if (positive_in && !negative_in)
      *initial_input = POSITIVE;
  }
}
