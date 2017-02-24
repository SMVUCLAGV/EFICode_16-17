#include "Controller.h"

#include "Arduino.h"
#include "constants.h"
#include "TimerThree.h"

Controller::Controller() {
  //Sets injector pin to output mode. All other pins default to input mode.
  pinMode(INJ_Pin, OUTPUT);
  //Initializes Serial input and output at the specified baud rate.
  Serial.begin(BAUD_RATE);
  Serial.write("Arduino Start\n");
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
  // Start at zero revolutions.
  revolutions = 0;
  totalRevolutions = 0;

  // Set the max speed at which data is reported
  maxTimePerSampleReported = 100000;  //Every 1ms

  // Initialize injector to disables mode.
  INJisDisabled = true;

  // NEEDS EXPLANATION
  delayCount = 4;

  // Amount of time in microseconds for the injector to open.
  openTime = 350;

  // Used to determine the amount of fuel used in each rpm range.
  for (int x = 0; x < numTableCols; x++) {
    totalPulse[x] = 0;
  }
  totalPulseTime = 0;

  // Start off not reporting any data.
  currentlySendingData = true;

  // Fill in fuel ratio table with 14.7 across the board.
  // Should be replaced with code that gets the last saved table from memory!
  for (int x = 0; x < numTableRows; x++) {
    for (int y = 0; y < numTableCols; y++) {
      fuelRatioTable[x][y] = 14.7;
    }
  }

  // Calculate base pulse times from fuel ratio table. Should actually
  // store the last table used and recall it from memory here!
  calculateBasePulseTime(false, 0, 0);

  realPulseTime = 0;
  lastRPMCalcTime = micros();
  lastInterrupt = millis();
}

void Controller::countRevolution() {
  revolutions++;
  totalRevolutions++;
  //Inject on every second revolution because this is a 4 stroke engine
  if (totalRevolutions % 2 == 1 || totalRevolutions < revsPerCalc) {
    digitalWrite(INJ_Pin, HIGH);
    Timer3.setPeriod(injectorPulseTime);
    Timer3.restart();
  }
}

// OBSOLETE! KEPT FOR REFERENCE!
//void Controller::calculatePulseTime() {
//  //THIS IS THE OLD METHOD OF CALCULATING PULSE TIMES
//  //(displ * rpm)/ 2 = cm^3/s air per power stroke
//  // ideal gas law PV = nRT
//  // T = IAT
//  // P = MAP
//  // mult n by mm of air, divide by 14.7
//  double TPSx = 1 + getTPS() - TPS;
//  TPS = getTPS();
//  //  if (totalRevolutions > 2 && TPS <= maxIdleTPS) {
//  //    //RPM-based feedback loop
//  //    if (RPM > desiredRPM) {
//  //      idleVal = idleVal - .00001;
//  //    }
//  //    else if (RPM < desiredRPM) {
//  //      idleVal = idleVal + .00001;
//  //    }
//  //    O2V = getOIN();
//  //    if (O2V > desiredO2) {
//  // +    idleVal = idleVal - .000002;
//  // +  }
//  // +  else if (O2V < desiredO2) {
//  // +    idleVal = idleVal + .000002;
//  // +  }
//  //  }
//  double val = getMAP() * injectionConstant / (getTemp(IAT_Pin) * fuelRatio * injectorFuelRate);
//  //Calculate pulse time
//  long pulseTime = 1000000 * val * startupVal * TPSx;
//  if (TPS <= maxIdleTPS) {
//    pulseTime *= idleVal;
//  }
//  totalPulseTime += pulseTime;
//  pulseTime += openTime;
//  injectorPulseTime = pulseTime;
//}

void Controller::lookupPulseTime() {
  // Map the MAP and RPM readings to a number between 0 and 1.
  double scaledMAP = map(MAP, minMAP, maxMAP, 0, 1); //number from 0 - 1
  double scaledRPM = map(RPM, minRPM, maxRPM, 0, 1); //number from 0 - 1

  // Clip out of bounds values to 0 and 1.
  scaledMAP = constrain(scaledMAP, 0, 1);
  scaledRPM = constrain(scaledRPM, 0, 1);

  // Get lower bounds for load and rpm indicies.
  int mapIndex = scaledMAP * (numTableRows - 1);
  int rpmIndex = scaledRPM * (numTableCols - 1);

  // Clip extrapolation to the max index. Otherwise, perform 2D interpolation to get
  // the base pulse time and then divide by the temperature
  if (mapIndex < numTableRows - 1 && rpmIndex < numTableCols) {
    // Interpolation case
    injectorPulseTime = interpolate2D(mapIndex, rpmIndex, scaledMAP, scaledRPM) / IAT;
  }
  else {
    // Clipped case
    injectorPulseTime = injectorBasePulseTimes[mapIndex][rpmIndex] / IAT;
  }
}

void Controller::calculateBasePulseTime(bool singleVal, int row, int col) {
  if (singleVal) {
    // Cover the range of pressure values from min - max inclusive.
    int pressure = map(row, 0, numTableRows - 1, minMAP, maxMAP);

    // Compute a base pulse time in units of microseconds * Kelvin. Temperature will be
    // divided on the fly to get the actual pulse time used.
    injectorBasePulseTimes[row][col] = openTime + 1000000 * pressure * injectionConstant /
                                       (fuelRatioTable[row][col] * injectorFuelRate);
    return;
  }

  for (int x = 0; x < numTableRows; x++) {
    for (int y = 0; y < numTableCols; y++) {
      // Cover the range of pressure values from min - max inclusive.
      int pressure = map(row, 0, numTableRows - 1, minMAP, maxMAP);

      // Compute a base pulse time in units of microseconds * Kelvin. Temperature will be
      // divided on the fly to get the actual pulse time used.
      injectorBasePulseTimes[x][y] = openTime + 1000000 * pressure * injectionConstant /
                                     (fuelRatioTable[x][y] * injectorFuelRate);
    }
  }
}

bool Controller::detectEngineOff() {
  if (micros() - lastRPMCalcTime >= SHUTOFF_DELAY)
  {
    return true;
  }
  return false;
}

