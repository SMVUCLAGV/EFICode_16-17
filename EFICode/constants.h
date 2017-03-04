#ifndef CONST_H
#define CONST_H

//Auxiliary Functions
void countRev();
void handle_pulseTimerTimeout();
void enableINJ();
void disableINJ();

// Define all the pin numbers
const int HES_Pin = 19;
const int TPS_Pin = 2;
const int ECT_Pin = 1;
const int MAP_Pin = 5;
const int OIN1_Pin = 3;
const int OIN2_Pin = 3;
const int IAT_Pin = 0;
const int INJ_Pin = 45;
const int FP_Pin = 47;
const int FRS_Pin = 6;
const int SP1 = 7;
const int SP2 = 8;
const int SP3 = 39;
const int SP4 = 37;

// Multiple by this number to convert analog readings to voltages.
const double voltageConversion = .0049;

// Constants for calculating estimated injection times.
const double injectionConstant  = .000172064;   //volume
const double injectorFuelRate   = 2.1333333;    //grams per second
const int openTime              = 350;          // Estimated amount of time for injector to open in microseconds.

// Define the range of values for RPM and Manifold Air Pressure
const int maxRPM = 8000;    // In revolutions / minute
const int minRPM = 120;     // In revolutions / minute
const int maxMAP = 120;     // In kPa
const int minMAP = 20;      // In kPa

// Define the number of rows and number of columns for the AFR Table.
const int numTableRows = 11;
const int maxTableRowIndex = numTableRows - 1;
const int numTableCols = 16;
const int maxTableColIndex = numTableCols - 1;

// Define the range of values that an AFR table value can take.
const int MIN_AFR = 10;
const int MAX_AFR = 17;

// Define the range of values that the Intake air temperature can take.
const int MIN_IAT = 200;  // In Kelvin
const int MAX_IAT = 500;  // In Kelvin

// Define the range of values that the Throttle Position value can take.
const int MIN_TPS = 1;
const int MAX_TPS = 1;

// Define the BAUD_RATE to communicate with.
const unsigned long BAUD_RATE = 860400; // In bits per second;

// Engine is considered off if it has RPM less than the minimum RPM.
const int SHUTOFF_RPM = minRPM;

// Minimum time that must pass per revolution before the engine can be considered off.
// Given in microseconds.
// (60 sec / min) * (10^6 microsecond / sec) * (SHUTOFF_RPM min / revolution) = (SHUTOFF_DELAY microseconds / revolution)
const unsigned long int SHUTOFF_DELAY = 6E7 / SHUTOFF_RPM ; 








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

#endif
