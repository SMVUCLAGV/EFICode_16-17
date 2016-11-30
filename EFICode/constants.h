#ifndef CONST_H
#define CONST_H

const int TPS_Pin = 3;
const int IAT_Pin = 14;
const int ECT_Pin = 5;
const int MAP_Pin = 11;
const int OIN_Pin = 13;
const int INJ_Pin = 32;

const double TPS_MOD = 1;

const double voltageConversion = .0049;
const double injectionConstant = .000172064;  //volume
const double injectorFuelRate = 2.1333333333; //grams per second

//Temperature Measurement
//oldTempSlope = -19.451
//oldTempIntercept = 89.135;
const double tempSlope = -.0953 // = oldTempSlope * voltageConversion
const double tempIntercept = 362.135 // = oldTempIntercept + 273

//TPS Measurement
//oldTPSConversion = 0.401738191;
//oldTPSOffset = -.84
const double TPSConversion = .0019685 // = oldTPSConversion * voltageConversion
const double TPSOffset = -.33746 // = oldTPSOffset * oldtpsConversion

//MAP Measurement
//scalingFactor = 1000
//oldMAPConversion = 18.8636364
//oldMAPOffset = 20
const double MAPConversion = 92.432 // = oldMAPConversion * voltageConversion * scalingFactor
const double MAPOffset = 20000 // = oldMAPOffset * scalingFactor

#endif
