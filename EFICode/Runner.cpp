#include "Runner.h"

#include "Arduino.h"
#include "Constants.h"
#include "TimerThree.h"

Controller::Controller() {
  pinMode(INJ_Pin, OUTPUT); //Sets injector pin to output mode
  Serial.begin(9600); //Initializes Serial output
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