#include "Controller.h"

#include "Arduino.h"
#include "Constants.h"
#include "TimerThree.h"

Controller::Controller() {
  pinMode(INJ_Pin, OUTPUT); //Sets injector pin to output mode
  Serial.begin(860800); //Initializes Serial output
  Serial.println("Time, ECT,    IAT,    MAP,     TPS,  OIN,  RPM,  DesPW,PWRe,Revs,Start,AFR"); //Prints headers for our data
  initializeParameters();
  readSensors();
}

bool Controller::readSensors() {
  TPS = getTPS();
  ECT = getTemp(ECT_Pin);
  IAT = getTemp(IAT_Pin);
  MAP = getMAP();
  OIN = getOIN();
}

void Controller::initializeParameters() {
  revolutions = 0;
  totalRevolutions = 0;

  desiredRPM = 2500;
  desiredOIN = 0;

  delayCount = 4;
  totalPulseTime = 0;
  openTime = 350;

  fuelRatio = 14.7;
  idleVal = 1.5;
  resetVal = 0.5;
  startupVal = 0.5;
  for (int x = 0; x < 32; x++) {
    totalPulse[x] = 0; //Initial fuel usage in each RPM range - the ranges are shown in parameters.h
  }
  currentlySendingData = false;
  calculatePulseTime(false, 0, 0);
}

void Controller::countRevolution() {
  revolutions++;
  totalRevolutions++;
  if (totalRevolutions % 2 == 1 || totalRevolutions < 25) {
    //&& RPM > desiredRPM)
    //Inject on every second revolution because this is a 4 stroke engine
    digitalWrite(INJ_Pin, HIGH);
    lastRPMCalcTime = millis();
    delayCount = 0;
    Timer3.setPeriod(injectorPulseTime);
    Timer3.restart();
  }
}

void Controller::calculatePulseTime() {
  //THIS IS THE OLD METHOD OF CALCULATING PULSE TIMES
  //(displ * rpm)/ 2 = cm^3/s air per power stroke
  // ideal gas law PV = nRT
  // T = IAT
  // P = MAP
  // mult n by mm of air, divide by 14.7
  double TPSx = 1 + getTPS() - TPS;
  TPS = getTPS();
  if (totalRevolutions > 2 && TPS <= maxIdleTPS) {
    //RPM-based feedback loop
    if (RPM > desiredRPM) {
      idleVal = idleVal - .00001;
    }
    else if (RPM < desiredRPM) {
      idleVal = idleVal + .00001;
    }
    O2V = getOIN();
    if (O2V > desiredO2) {
 +    idleVal = idleVal - .000002;
 +  }
 +  else if (O2V < desiredO2) {
 +    idleVal = idleVal + .000002;
 +  }
  }
  double val = getMAP() * injectionConstant / (getTemp(IAT_Pin) * fuelRatio * injectorFuelRate);
  //Calculate pulse time
  long pulseTime = 1000000 * val * startupVal * TPSx;
  if (TPS <= maxIdleTPS) {
    pulseTime *= idleVal;
  }
  totalPulseTime += pulseTime;
  pulseTime += openTime;
  injectorPulseTime = pulseTime;
}

void Controller::lookupPulseTime() {
  double loadPCT = .05; //TODO: Calculate loadPCT from relevant parameters
  int loadIndex = ((int) (loadPCT * 100)) / 10;
  int RPMIndex = RPM / 32;
  injectorPulseTime = injectorPulseTimes[loadIndex][RPMIndex];
  //TODO: write additional calculations necessary
  //TODO: handle linear interpolation
}

void Controller::calculatePulseTime(bool singleVal, int row, int col) {
  //TODO: Find/Code equation used to relate pulse time to engine load
  if (singleVal) {
    injectorPulseTimes[row][col] = 5;
    return;
  }
  for (int x = 0; x < 10; x++) {
    for (int y = 0; y < 32; y++) {
      //TODO: Find/Code equation used to relate pulse time to engine load
      injectorPulseTimes[x][y] = 5;
    }
  }
  //1: Lookup Pulse Time
  //2: Inject for that pulse time
  //3: Read from O2 sensor to see if we are hitting desired AFR
  //4: Adjust pulse time, too much fuel = decrease pulse time and vice versa
}
