#include "PSXPad.h"

#include <SPI.h>

class PSXPads PSXPads;

const byte PSX_CMD_POLL[]         = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const byte PSX_CMD_ENTER_CFG[]    = {0x01, 0x43, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
const byte PSX_CMD_EXIT_CFG[]     = {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
const byte PSX_CMD_ENABLE_MOTOR[] = {0x01, 0x4D, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const byte PSX_CMD_ALL_PRESSURE[] = {0x01, 0x4F, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00};
const byte PSX_CMD_AD_MODE[]      = {0x01, 0x44, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

PSXPad::PSXPad(const byte i_bAttPinNo) {
  byte bLoc;

  // set Attention pin
  bAttPinNo = i_bAttPinNo;
  pinMode(bAttPinNo, OUTPUT);
  digitalWrite(bAttPinNo, HIGH);

  // set Command buffers
  for (bLoc = 0; bLoc < sizeof(PSX_CMD_AD_MODE); bLoc++) {
    lbADMode[bLoc] = PSX_CMD_AD_MODE[bLoc];
  }
  for (bLoc = 0; bLoc < sizeof(PSX_CMD_POLL); bLoc++) {
    lbPoolCmd[bLoc] = PSX_CMD_POLL[bLoc];
  }
  for (bLoc = 0; bLoc < sizeof(PSX_CMD_ENABLE_MOTOR); bLoc++) {
    lbEnableMotor[bLoc] = PSX_CMD_ENABLE_MOTOR[bLoc];
  }

  // motor OFF
  setMotorLevel(0, 0, false);
  setEnableMotor(false, false);
}

void PSXPad::pool(void) {
  command(lbPoolCmd, sizeof(PSX_CMD_POLL));
}

void PSXPad::setADMode(const PSXPad_KeyType_t i_vType, const boolean i_bLock) {
  byte bLoc;
  PSXPad_KeyState_t tKeyState;

  if (i_vType >= PSXPAD_KEYTYPE_UNKNOWN)
    return;

  vType = i_vType;
  bLock = i_bLock;

  switch (i_vType) {
    case PSXPAD_KEYTYPE_DIGITAL:
      lbADMode[3] = 0x00;
      break;
    case PSXPAD_KEYTYPE_ANALOG1:
    case PSXPAD_KEYTYPE_ANALOG2:
      lbADMode[3] = 0x01;
      break;
  }
  lbADMode[4] = i_bLock ? 0x03 : 0x00;

  //  if(i_vType == PSXPAD_KEYTYPE_ANALOG2) {
  //  } else {
  command(PSX_CMD_ENTER_CFG, sizeof(PSX_CMD_ENTER_CFG));
  command(lbADMode, sizeof(PSX_CMD_AD_MODE));
  command(PSX_CMD_ALL_PRESSURE, sizeof(PSX_CMD_ALL_PRESSURE));
  command(PSX_CMD_EXIT_CFG, sizeof(PSX_CMD_EXIT_CFG));
  //  }
}

void PSXPad::setEnableMotor(const boolean i_bMotor1Enable, const boolean i_bMotor2Enable) {
  byte bLoc;

  bMotor1Enable = i_bMotor1Enable;
  bMotor2Enable = i_bMotor1Enable;

  lbEnableMotor[3] = (bMotor1Enable == 0) ? 0x00 : 0xFF;
  lbEnableMotor[4] = (bMotor2Enable == 0) ? 0x01 : 0xFF;

  command(PSX_CMD_ENTER_CFG, sizeof(PSX_CMD_ENTER_CFG));
  command(lbEnableMotor, sizeof(PSX_CMD_ENABLE_MOTOR));
  command(PSX_CMD_EXIT_CFG, sizeof(PSX_CMD_EXIT_CFG));
}

void PSXPad::setMotorLevel(const byte i_bMotor1Level, const byte i_bMotor2Level, const boolean i_bPool) {
  bMotor1Level = i_bMotor1Level;
  bMotor2Level = i_bMotor2Level;

  lbPoolCmd[3] = bMotor1Level ? 0xFF : 0x00;
  lbPoolCmd[4] = bMotor2Level;

  if (i_bPool)
    pool();
}

void PSXPad::command(const byte i_lbSendCmd[], const byte i_bSendCmdLen) {
  byte bPtr;
  byte bCmd;

  digitalWrite(bAttPinNo, LOW);
  delayMicroseconds(10);

  for (bPtr = 0; bPtr < 21; bPtr++) {
    bCmd = (bPtr < i_bSendCmdLen) ? i_lbSendCmd[bPtr] : 0x00;
    lbResponse[bPtr] = SPI.transfer(bCmd);
    delayMicroseconds(10);
  }

  digitalWrite(bAttPinNo, HIGH);
}

void PSXPad::getKeyState(PSXPad_KeyState_t* o_ptKeyState) {
  if (!o_ptKeyState)
    return;

  o_ptKeyState->vType = PSXPAD_KEYTYPE_UNKNOWN;

  switch (lbResponse[1]) {
    case 0x79:
      o_ptKeyState->vType = PSXPAD_KEYTYPE_ANALOG2;
      o_ptKeyState->bAR   = lbResponse[ 9];
      o_ptKeyState->bAL   = lbResponse[10];
      o_ptKeyState->bAU   = lbResponse[11];
      o_ptKeyState->bAD   = lbResponse[12];
      o_ptKeyState->bATri = lbResponse[13];
      o_ptKeyState->bACir = lbResponse[14];
      o_ptKeyState->bACrs = lbResponse[15];
      o_ptKeyState->bASqr = lbResponse[16];
      o_ptKeyState->bAL1  = lbResponse[17];
      o_ptKeyState->bAR1  = lbResponse[18];
      o_ptKeyState->bAL2  = lbResponse[19];
      o_ptKeyState->bAR2  = lbResponse[20];
    case 0x73:
      if (o_ptKeyState->vType == PSXPAD_KEYTYPE_UNKNOWN)
        o_ptKeyState->vType = PSXPAD_KEYTYPE_ANALOG1;
      o_ptKeyState->bRX = lbResponse[5];
      o_ptKeyState->bRY = lbResponse[6];
      o_ptKeyState->bLX = lbResponse[7];
      o_ptKeyState->bLY = lbResponse[8];
      o_ptKeyState->bL3  = (lbResponse[3] & 0x02U) ? false : true;
      o_ptKeyState->bR3  = (lbResponse[3] & 0x04U) ? false : true;
    case 0x41:
      if (o_ptKeyState->vType == PSXPAD_KEYTYPE_UNKNOWN)
        o_ptKeyState->vType = PSXPAD_KEYTYPE_DIGITAL;
      o_ptKeyState->bSel = (lbResponse[3] & 0x01U) ? false : true;
      o_ptKeyState->bStt = (lbResponse[3] & 0x08U) ? false : true;
      o_ptKeyState->bU   = (lbResponse[3] & 0x10U) ? false : true;
      o_ptKeyState->bR   = (lbResponse[3] & 0x20U) ? false : true;
      o_ptKeyState->bD   = (lbResponse[3] & 0x40U) ? false : true;
      o_ptKeyState->bL   = (lbResponse[3] & 0x80U) ? false : true;
      o_ptKeyState->bL2  = (lbResponse[4] & 0x01U) ? false : true;
      o_ptKeyState->bR2  = (lbResponse[4] & 0x02U) ? false : true;
      o_ptKeyState->bL1  = (lbResponse[4] & 0x04U) ? false : true;
      o_ptKeyState->bR1  = (lbResponse[4] & 0x08U) ? false : true;
      o_ptKeyState->bTri = (lbResponse[4] & 0x10U) ? false : true;
      o_ptKeyState->bCir = (lbResponse[4] & 0x20U) ? false : true;
      o_ptKeyState->bCrs = (lbResponse[4] & 0x40U) ? false : true;
      o_ptKeyState->bSqr = (lbResponse[4] & 0x80U) ? false : true;
  }
}

void PSXPads::begin(const byte i_bPadNum, const byte i_lbAttPinNos[]) {
  byte bNo;

  bPadNum = i_bPadNum;

  SPI.begin();
  // 250kHz, LSB first, CPOL=1/CPHA=1
  SPI.beginTransaction(SPISettings(250000, LSBFIRST, SPI_MODE3));

  lpcPads = new class PSXPad*[bPadNum];

  for (bNo = 0; bNo < bPadNum; bNo++)
    lpcPads[bNo] = new class PSXPad(i_lbAttPinNos[bNo]);
}

PSXPads::~PSXPads(void) {
  byte bNo;

  for (bNo = 0; bNo < bPadNum; bNo++)
    delete lpcPads[bNo];

  delete lpcPads;
}

void PSXPads::pool(void) {
  byte bNo;

  for (bNo = 0; bNo < bPadNum; bNo++)
    lpcPads[bNo]->pool();
}
