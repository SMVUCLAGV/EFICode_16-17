#include "Controller.h"

#include "Arduino.h"
#include "Constants.h"
#include "TimerThree.h"
#include "NoiseReduced.h"

Controller::Controller() {
    //Sets injector pin to output mode. All other pins default to input mode.
    pinMode(INJ_Pin, OUTPUT);
  
    //Initializes Serial input and output at the specified baud rate.
    Serial.begin(BAUD_RATE);
  
    // Initializing message
    //Serial.write("Initializing...\n");
  
    // Initialize parameters with their starting values.
    initializeParameters();
  
    // Update sensors to their initial values.
    readSensors();
  
    // Perform quick diagnostics here...
    // runDiagnostics();
  
    // Indicate ready
    //Serial.write("Ready to go!\n");
}

bool Controller::readSensors() {
    TPS = constrain(getTPS(),MIN_TPS,MAX_TPS);
    ECT = getTemp(ECT_Pin);
    IAT = getTemp(IAT_Pin);
    MAP = getMAP();
    MAPAvg->addData(MAP);
    return true;
}

void Controller::initializeParameters() {
    // Start at zero revolutions.
    revolutions = 0;
    totalRevolutions = 0;
    magnetsHit = 0;

    // Set the max speed at which data is reported
    minTimePerSampleReported = 1E3;  //In microseconds

    // Number of revolutions that must pass before recalculating RPM.
    revsPerCalc = 5;
    
    // Initialize AFR values.
    AFR = 0;
    AFRVolts = new NoiseReduced(100);
    startupModifier = 1.0;

    // Initialize MAP averaging
    MAPAvg = new NoiseReduced(100);
    
    // Initialize MAP and RPM indicies to zero.
    mapIndex = 0;
    rpmIndex = 0;
    
    // Initialize injector to disabled mode.
    // Used to detach the timer interrupt for pulsing off
    // when the engine is not running.
    INJisDisabled = true;

    // Used to determine the amount of fuel used.
    totalPulseTime = 0;
    lastPulse = 0;

    // True   -> Start with data reporting on.
    // False  -> Start with data reporting off.
    currentlySendingData = false;

    // If false, doesn't use AFR feedback.
    AFRFeedbackisEnabled = false;

    // Used for RPM feedback loop
    prevDeltaRPM = 0;

    // If false, doesn't use RPM feedback;
    RPMFeedbackisEnabled = true;
    desiredRPM = 2000;

    // Calculate base pulse times from fuel ratio table. Should actually
    // store the last table used and recall it from memory here!
    calculateBasePulseTime(false, 0, 0);

    lastRPMCalcTime = micros();
}

void Controller::countRevolution() {
  if (magnetsHit >= numMagnets - 1) {
      // Enable the injector if it is disabled.
      if (INJisDisabled) {
        enableINJ();
      }
      
      // Increment the number of revolutions 
      revolutions++;
      totalRevolutions++;
      startingRevolutions++;
      //Lock guards seem unneccessary
      
      //Inject on every second revolution because this is a 4 stroke engine
      if (totalRevolutions % 2 == 1) {
        pulseOn();
      }
      magnetsHit = 0;
  }
  else {
      magnetsHit++;
  }
}


void Controller::enableINJ() {
  INJisDisabled = false;
}

void Controller::disableINJ() {
  Timer3.stop();
  digitalWrite(INJ_Pin, LOW);
  noInterrupts();
  INJisDisabled = true;
  interrupts();
}

void Controller::pulseOn() {
    Timer3.setPeriod(injectorPulseTime);
    digitalWrite(INJ_Pin, HIGH);
    Timer3.start();
    noInterrupts(); //To ensure when lastPulse is used in pulseOff(), it isn't read as lastPulse is getting modified
    lastPulse = micros(); //Race Conditions Problem
    interrupts();
}

void Controller::pulseOff() {
  // When it's time to turn the injector off, follow these steps and turn it off
  digitalWrite(INJ_Pin, LOW);
  Timer3.stop();

  // Save the amount of time the injector pin spent HIGH.
  totalPulseTime += (micros() - lastPulse);
}

void Controller::updateRPM() {
  noInterrupts();
  int tempRev = revolutions; //Prevents revolutions being read while it is being modified by the 
  //countRevolution() function associated with the interrupt
  interrupts();
  if (tempRev >= revsPerCalc) { 
    RPM = getRPM(micros() - lastRPMCalcTime, tempRev); //Uses the previously determined value of revolutions to reduce
    //amount of noInterrupts() calls
    lastRPMCalcTime = micros();
    noInterrupts(); //To ensure that the interrupt of countRev doesn't get lost in case of bad timing of threads
    revolutions = 0; //Race Conditions Modification Problem
    interrupts();
    
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
    double scaledMAP = map(MAPAvg->getData(), minMAP, maxMAP, 0, maxTableRowIndex); //number from 0 - numTableRows-1
    double scaledRPM = map(RPM, minRPM, maxRPM, 0, maxTableColIndex); //number from 0 - numTableCols-1

    // Clip out of bounds to the min or max value, whichever is closer.
    scaledMAP = constrain(scaledMAP, 0, maxTableRowIndex);
    scaledRPM = constrain(scaledRPM, 0, maxTableColIndex);

    // Get lower bounds for load and rpm indicies.
    mapIndex = scaledMAP;
    rpmIndex = scaledRPM;

    // Clip extrapolation to the value at the max index. Otherwise, perform 2D interpolation to get
    // the base pulse time and then divide by the temperature.
    long tempPulseTime;
    if (rpmIndex < maxTableColIndex && mapIndex < maxTableRowIndex) {
        // Interpolation case
        tempPulseTime = openTime + interpolate2D(mapIndex, rpmIndex, scaledMAP-mapIndex, scaledRPM-rpmIndex) / IAT;
    }
    else {
        // Clipped case
        tempPulseTime = openTime + injectorBasePulseTimes[mapIndex][rpmIndex] / IAT;
    }
    // Add extra fuel for starting
    if (startingRevolutions <= numRevsForStart)
    {
        tempPulseTime *= startupModifier;
    }
    noInterrupts();
    injectorPulseTime = tempPulseTime;
    interrupts();
}

// IF O2 SENSOR IS ERRORING OR NOT READY, THE ANALOG OUTPUT IS SET TO BE EQUAL
// TO ZERO VOLTS. THEREFORE, I HAVE IMPOSED A 0.05 Volt LIMITATION ON VOLTAGES READ
// FROM THE O2 SENSOR. IF THE VOLTAGE READ IS LESS THAN 0.05 Volts, then the AFR
// FEEDBACK LOOP WILL DO NOTHING!
void Controller::AFRFeedback() {
    getAFR();   // Rename to "updateAFR"
    // If the engine is off, definitely do not want the feedback loop
    // to be active. If the O2 sensor is reading extremely high or low
    // AFR then the reading is probably off and we do not want the feedback
    // loop to be active based on bad values.
    if (AFRVolts->getData() < 0.05 || detectEngineOff() || AFRVolts->getData() > 4.95) {
        return;
    }

    // Stores the desired AFR value in a temporary location.
    // Just meant for readability of code.
    double dAFR = fuelRatioTable[mapIndex][rpmIndex];
    
    // With more data on how the engine responds to input, will be able to
    // fine tune this feedback to work more efficiently.
    // The ratio of the new pulse time to the old pulse time should be
    // equal to the ratio of the measured AFR to the desired AFR.
    if (AFRFeedbackisEnabled)
    {
        // Stores the desired AFR value in a temporary location.
        // Just meant for readability of code.
        double dAFR = fuelRatioTable[mapIndex][rpmIndex];
        double deltaAFR = dAFR - AFR;
        unsigned long pressure = map(mapIndex, 0, numTableRows - 1, minMAP, maxMAP);
        
        // With more data on how the engine responds to input, will be able to
        // fine tune this feedback to work more efficiently.
        // The ratio of the new pulse time to the old pulse time should be
        // equal to the ratio of the measured AFR to the desired AFR.
        injectorBasePulseTimes[mapIndex][rpmIndex] = 1E6 * pressure * injectionConstant / dAFR * (1 - (deltaAFR) / dAFR);
    }
}

const double K_p_RPM = 3;
const double K_d_RPM = 0.5;
void Controller::idleRPMFeedback() {
  if (detectEngineOff())
      return;

  if (RPMFeedbackisEnabled)
  {
      long deltaRPM = desiredRPM - RPM;
      injectorBasePulseTimes[mapIndex][rpmIndex] = injectorBasePulseTimes[mapIndex][rpmIndex] - K_p_RPM * deltaRPM + K_d_RPM * (prevDeltaRPM - deltaRPM);
      prevDeltaRPM = deltaRPM;
  }
}

void Controller::calculateBasePulseTime(bool singleVal, int row, int col) {
  if (singleVal) {
    // Cover the range of pressure values from min - max inclusive.
    unsigned long pressure = map(row, 0, numTableRows - 1, minMAP, maxMAP);

    // Compute a base pulse time in units of microseconds * Kelvin. Temperature will be
    // divided on the fly to get the actual pulse time used.
    injectorBasePulseTimes[row][col] = 1E6 * pressure * injectionConstant /
                                     (fuelRatioTable[row][col]);
    return;
  }

  for (int x = 0; x < numTableRows; x++) {
    for (int y = 0; y < numTableCols; y++) {
      // Cover the range of pressure values from min - max inclusive.
      unsigned long pressure = map(x, 0, numTableRows - 1, minMAP, maxMAP);

      // Compute a base pulse time in units of microseconds * Kelvin. Temperature will be
      // divided on the fly to get the actual pulse time used.
      injectorBasePulseTimes[x][y] = 1E6 * pressure * injectionConstant /
                                     (fuelRatioTable[x][y]);
    }
  }
}

void Controller::checkEngineState() {
  if (detectEngineOff()) {
    if (revsPerCalc > 0)
    {
        revolutions = 0;
        startingRevolutions = 0;
        RPM = 0;
        lastRPMCalcTime = micros();
    }
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

