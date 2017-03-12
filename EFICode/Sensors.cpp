#include "Controller.h"

#include "Arduino.h"
#include "Constants.h"
#include "math.h"

// timepassed is in microseconds
int Controller::getRPM (long int timePassed, int rev) {
  return (60000000.0 * rev) / (timePassed);
}

//TPS Measurement
const double TPSConversion = .0019685;
const double TPSOffset = -.33746;

double Controller::getTPS() {
  // Gets throttle position based off of the percentage of throttle area open
  // TODO: Create sin lookup table
  return sin(TPSConversion * analogRead(TPS_Pin) + TPSOffset);
}

//Temperature Measurement
// Pre-computes the coefficient values in order to minimize operations
// during runtime. These pre-computed values are enumerated below. Uses
// the quadratic formula to calculate the temperature.
const double tempAlpha = 1.04E-4;     // Coefficient of T^2 term.
const double tempBeta = -1.01832E-1;  // Coefficient of T^1 term.
const double tempGamma = 2.18539E1;   // Coefficient of T^0 term.

// Coefficients resulting from combining alpha and beta in an efficient way.
const double tempA = -tempBeta/(2*tempAlpha);
const double tempB = 1/(pow(tempAlpha,2));
const double tempC = pow(tempBeta,2)/(2*tempAlpha);

// The input voltage to the voltage divider consisting of a 1k resistor and the
// thermistor in series.
const double tempInputVal = 5.00 / voltageConversion;
const double minTempVal = 0.5 / voltageConversion;
const double maxTempVal = 4.5 / voltageConversion;

double Controller::getTemp(int pin) {
  //return analogRead(pin)*voltageConversion;
  //Gets temperature reading from specified sensor by using calibration curve
  //return tempSlope * analogRead(pin) + tempIntercept;
  int mval = analogRead(pin);

  // Constrain values to this range. Values outside of this range will be useless because
  // the error in the curve fit becomes extremely large. This constraint also gets rid of
  // the possibility of having imaginary or infinite results.
  mval = constrain(mval, minTempVal, maxTempVal);

  // TODO: Create log lookup table.

  // Compute temperature from a curve fit to the exponential function with a quadratic
  // power with respect to the temperature.
  double x = log((tempInputVal-mval)/(mval));
  //return tempA - tempB * sqrt(tempC - (tempGamma - x));
  return ((-tempBeta)-sqrt(pow(tempBeta,2)-4*tempAlpha*(tempGamma-x)))/(2 * tempAlpha);
}

//MAP Measurement
const double MAPSlope = (20000 - 103000) / (0.5 - 4.9); // (y1 - y2) / (x1 - x2) Pascals / Volt
const double MAPOffset = ((-4.9 * 20000) + (0.5 * 103000)) / (0.5 - 4.9); // = Pascals
const double MAPConversion = MAPSlope * voltageConversion; // Pascals / 1023

double Controller::getMAP() {
  //Calculates MAP, outputs in Pa
  return MAPConversion * analogRead(MAP_Pin) + MAPOffset;
}

// Analog output 1 factory default settings for voltage ranges.
const double AO1minAFR = 7.35;     //grams air to grams fuel at zero Volts
const double AO1maxAFR = 22.39;    //grams air to grams fuel at five Volts
const double AO1slope = (AO1maxAFR - AO1minAFR) / (5 - 0);

// Analog output 2 factory default settings for voltage ranges
const double AO2minAFR = 14;    //grams air to grams fuel at 0.1 Volts
const double AO2maxAFR = 15;    //grams air to grams fuel at 1.1 Volts
const double AO2slope = (AO2maxAFR - AO2minAFR) / (1.1 - 0.1);

// IF O2 SENSOR IS ERRORING OR NOT READY, THE ANALOG OUTPUT IS SET TO BE EQUAL
// TO ZERO VOLTS. THEREFORE, I HAVE IMPOSED A 0.05 Volt LIMITATION ON VOLTAGES READ
// FROM THE O2 SENSOR. IF THE VOLTAGE READ IS LESS THAN 0.05 Volts, then the AFR
// FEEDBACK LOOP WILL DO NOTHING! 

// Returns the current measured AFR.
double Controller::getAFR () {
  // Gets Reading from O2 Sensor.
  
  // Calculate initial AFR reading.
  AFRVolts = voltageConversion * analogRead(OIN1_Pin);
  AFR = AFRVolts * AO1slope + AO1minAFR;
  
  // If AFR is close to stoich, use narrow band output with greater precision.
//  if (AFR <= 15 && AFR >= 14) {
//      AFR = voltageConversion * analogRead(OIN2_Pin) * AO2slope + AO2minAFR;
//  }
  
  return AFR;
}
