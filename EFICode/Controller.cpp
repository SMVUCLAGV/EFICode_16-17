#include "Controller.h"

#include "Arduino.h"
#include "Constants.h"
#include "TimerThree.h"

Controller::Controller() {
    //Sets injector pin to output mode. All other pins default to input mode.
    pinMode(INJ_Pin, OUTPUT);
  
    //Initializes Serial input and output at the specified baud rate.
    Serial.begin(BAUD_RATE);
  
    // Initializing message
    Serial.write("Initializing...\n");
  
    // Initialize parameters with their starting values.
    initializeParameters();
  
    // Update sensors to their initial values.
    readSensors();
  
    // Perform quick diagnostics here...
    // runDiagnostics();
  
    // Indicate ready
    Serial.write("Ready to go!\n");
}

bool Controller::readSensors() {
    TPS = getTPS();
    ECT = getTemp(ECT_Pin);
    IAT = getTemp(IAT_Pin);
    MAP = getMAP();
}

void Controller::initializeParameters() {
    // Start at zero revolutions.
    revolutions = 0;
    totalRevolutions = 0;

    // Set the max speed at which data is reported
    minTimePerSampleReported = 1E3;  //In microseconds

    // Number of revolutions that must pass before recalculating RPM.
    revsPerCalc = 2;
    
    // Initialize AFR values.
    AFR = 0;
    AFRVolts = 0;
    
    // Initialize MAP and RPM indicies to zero.
    mapIndex = 0;
    rpmIndex = 0;
    
    // Initialize injector to disabled mode.
    // Used to detach the timer interrupt for pulsing off
    // when the engine is not running.
    INJisDisabled = true;

    // Used to determine the amount of fuel used.
    totalPulseTime = 0;

    // True   -> Start with data reporting on.
    // False  -> Start with data reporting off.
    currentlySendingData = false;

    // Fill in fuel ratio table with 14.7 across the board.
    // Should be replaced with code that gets the last saved table from memory!
    // 14.7 is the mass ratio of air to fuel for approximately stoichiometric combustion.
    for (int x = 0; x < numTableRows; x++) {
        for (int y = 0; y < numTableCols; y++) {
        fuelRatioTable[x][y] = 14.7;
        }
    }

    // Calculate base pulse times from fuel ratio table. Should actually
    // store the last table used and recall it from memory here!
    calculateBasePulseTime(false, 0, 0);

    lastRPMCalcTime = micros();
}

void Controller::countRevolution() {
  // Enable the injector if it is disabled.
  if (INJisDisabled) {
    enableINJ();
  }
  
  // Increment the number of revolutions 
  revolutions++;
  
  //Inject on every second revolution because this is a 4 stroke engine
  if (revolutions % 2 == 1) {
    pulseOn();
  }
}

void Controller::enableINJ() {
  Timer3.attachInterrupt(handle_pulseTimerTimeout, injectorPulseTime);
  INJisDisabled = false;
}

void Controller::disableINJ() {
  Timer3.detachInterrupt();
  pulseOff();
  INJisDisabled = true;
}

void Controller::pulseOn() {
  if (!INJisDisabled) {
    Timer3.setPeriod(injectorPulseTime);
    digitalWrite(INJ_Pin, HIGH);
    Timer3.restart();
  }
}

void Controller::pulseOff() {
  // When it's time to turn the injector off, follow these steps and turn it off
  digitalWrite(INJ_Pin, LOW);

  // Save the amount of time the injector pin spent HIGH.
  totalPulseTime += (micros() - lastRPMCalcTime) / 1E6;
}

void Controller::updateRPM() {
  if (revolutions >= revsPerCalc) {
    //Calculate RPM each cycle
    RPM = getRPM(micros() - lastRPMCalcTime, revolutions);
    totalRevolutions += revolutions;
    revolutions = 0;
    lastRPMCalcTime = micros();
    // Should also dynamically change revsPerCalc. At lower RPM
    // the revsPerCalc should be lower but at higher RPM it should be higher.
  }
}

long Controller::interpolate2D(int blrow, int blcol, double x, double y) {
    // Takes the coordinate of the bottom left corner of the square to perform 2D interpolation over.
    // x and y must be given in unit form. i.e., y = (yc-y1)/(y2-y1) and x = (xc-x1)/(x2-x1)
    // (0 <= y <= 1 and 0 <= x <= 1)
    return
    injectorBasePulseTimes[blrow][blcol]*(1-y)*(1-x)+
    injectorBasePulseTimes[blrow+1][blcol]*(y)*(1-x)+
    injectorBasePulseTimes[blrow][blcol+1]*(1-y)*(x)+
    injectorBasePulseTimes[blrow+1][blcol+1]*(y)*(x);
  }

void Controller::lookupPulseTime() {
    // Map the MAP and RPM readings to the scale of 
    double scaledMAP = map(MAP, minMAP, maxMAP, 0, maxTableRowIndex); //number from 0 - numTableRows-1
    double scaledRPM = map(RPM, minRPM, maxRPM, 0, maxTableColIndex); //number from 0 - numTableCols-1

    // Clip out of bounds values to 0 and 1.
    scaledMAP = constrain(scaledMAP, 0, maxTableRowIndex);
    scaledRPM = constrain(scaledRPM, 0, maxTableColIndex);

    // Get lower bounds for load and rpm indicies.
    mapIndex = scaledMAP;
    rpmIndex = scaledRPM;

    // Clip extrapolation to the value at the max index. Otherwise, perform 2D interpolation to get
    // the base pulse time and then divide by the temperature.
    if (rpmIndex < maxTableColIndex && mapIndex < maxTableRowIndex) {
        // Interpolation case
        injectorPulseTime = interpolate2D(mapIndex, rpmIndex, scaledMAP-mapIndex, scaledRPM-rpmIndex) / IAT;
    }
    else {
        // Clipped case
        injectorPulseTime = injectorBasePulseTimes[mapIndex][rpmIndex] / IAT;
    }
}

// IF O2 SENSOR IS ERRORING OR NOT READY, THE ANALOG OUTPUT IS SET TO BE EQUAL
// TO ZERO VOLTS. THEREFORE, I HAVE IMPOSED A 0.05 Volt LIMITATION ON VOLTAGES READ
// FROM THE O2 SENSOR. IF THE VOLTAGE READ IS LESS THAN 0.05 Volts, then the AFR
// FEEDBACK LOOP WILL DO NOTHING!
void Controller::AFRFeedback() {
    getAFR();   // Rename to "updateAFR"
    if (AFRVolts < 0.05) {
        return;
    }
    
    double deltaAFR = fuelRatioTable[mapIndex][rpmIndex] - AFR;    // Positive if rich, negative if lean.
    
    // With more data on how the engine responds to input, will be able to
    // fine tune this feedback to work more efficiently.
    injectorBasePulseTimes[mapIndex][rpmIndex] = 
    injectorBasePulseTimes[mapIndex][rpmIndex] * (1 - deltaAFR/AFR);
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

void Controller::checkEngineState() {
  if (detectEngineOff()) {
    totalRevolutions += revolutions;
    revolutions = 0;
    RPM = 0;
    lastRPMCalcTime = micros();
    if (!INJisDisabled) {
      disableINJ();
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

