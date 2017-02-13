#ifndef CONST_H
#define CONST_H

const int HES_Pin = 2;
const int TPS_Pin = 3;
const int ECT_Pin = 5;
const int MAP_Pin = 11;
const int OIN_Pin = 13;
const int IAT_Pin = 14;
const int INJ_Pin = 32;

const double TPS_MOD = 1;
const double injectionConstant = .000172064;  //volume
const double injectorFuelRate = 2.1333333333; //grams per second

const double maxIdleTPS = 0.15;
const double desiredO2 = 0.5;
const int endRPM = 8000;

const int RPMIncrement = 250;

const int maxRPM = 7999;

#endif
