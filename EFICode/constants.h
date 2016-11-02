#ifndef CONST_H
#define CONST_H

const int TPS_Pin = 3;
const int IAT_Pin = 14;
const int ECT_Pin = 5;
const int MAP_Pin = 11;
const int OIN_Pin = 13;

const int INJ_Pin = 32;

const double TPS_MOD = 1;
const double slope = -19.451;
const double intercept = 89.135;
const double voltageConversion = .0049;
const double throttlePositionConversion = 0.401738191;
const double injectionConstant = .000172064;  //volume
const double injectorFuelRate = 2.1333333333; //grams per second

#endif
