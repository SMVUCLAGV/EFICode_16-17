#include "Controller.h"

#include "Arduino.h"
#include "Constants.h"
#include "math.h"

int Controller::getRPM (long int timePassed, int rev) {
  return (60000000.0 * rev) / (timePassed);
}

//TPS Measurement
//oldTPSConversion = 0.401738191;
//oldTPSOffset = -.84
//TPSConversion = oldTPSConversion * voltageConversion
//TPSOffset = oldTPSOffset * oldtpsConversion
const double TPSConversion = .0019685;
const double TPSOffset = -.33746;

double Controller::getTPS() {
  //gets throttle position based off of the percentage of throttle area open
  //TODO: Create sin lookup table
  return sin(TPSConversion * analogRead(TPS_Pin) + TPSOffset);
}

//Temperature Measurement
//oldTempSlope = -19.451
//oldTempIntercept = 89.135;
//tempSlope = oldTempSlope * voltageConversion
//tempIntercept = oldTempIntercept + 273
const double tempAlpha = -0.0317;
const double tempBeta = 10.1133;
const double tempInputVal = 5.00 / voltageConversion; 

double Controller::getTemp(int pin) {
  //Gets temperature reading from specified sensor by using calibration curve
  //return tempSlope * analogRead(pin) + tempIntercept;
  int mval = analogRead(pin);
  return ((1) / (tempAlpha)) * (((mval) / (tempInputVal - mval)) - tempBeta);
}

//MAP Measurement
//scalingFactor = 1000
//oldMAPConversion = 18.8636364
//oldMAPOffset = 20
const double MAPConversion = 92.432; // = oldMAPConversion * voltageConversion * scalingFactor
const double MAPOffset = 20000; // = oldMAPOffset * scalingFactor

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
  //if (AFR <= 15 && AFR >= 14) {
  //    AFR = voltageConversion * analogRead(OIN2_Pin) * AO2slope + AO2minAFR;
  //}
  
  return AFR;
}
