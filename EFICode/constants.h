#ifndef CONST_H
#define CONST_H

const int HES_Pin = 19;
const int HES_Interrupt = 4;
const int TPS_Pin = 2;
const int ECT_Pin = 1;
const int MAP_Pin = 5;
const int OIN_Pin = 3;
const int IAT_Pin = 0;
const int INJ_Pin = 45;
const int FP_Pin = 47;
const int FRS_Pin = 6;
const int SP1 = 7;
const int SP2 = 8;
const int SP3 = 39;
const int SP4 = 37;

const double TPS_MOD = 1;
const double injectionConstant = .000172064;  //volume
const double injectorFuelRate = 2.1333333333; //grams per second

const double maxIdleTPS = 0.15;
const double desiredO2 = 0.5;

const int revsPerCalc = 25;

const int maxRPM = 8000;
const int minRPM = 250;
const int maxMAP = 120;
const int minMAP = 20;

const int numTableRows = 11;
const int numTableCols = 16;

const int BAUD_RATE = 9600;//860800;

// Engine is considered off if it has RPM less than 500.
const int SHUTOFF_RPM = 500;

// Amount of time corresponding to SHUTOFF_RPM.
// Given in microseconds.
const unsigned long int SHUTOFF_DELAY = 6E7 / SHUTOFF_RPM ; 

#endif
