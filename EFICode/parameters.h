#include "Arduino.h"

#ifndef PARAMETERS_H
#define PARAMETERS_H

int openTime = 350;

// Revolution counters
int revolutions = 0;
unsigned long totalRevolutions = 0;

// RPM counters
unsigned long lastRPMCalculationTime = 0;
int RPM = 0;

// Time counters
long lastSerialOutputTime = 0;
long lastRPMCalcTime;
long pulseTimeReal = 0;
long lastInterrupt;
int delayCount = 0;

// Feedback loop values
const int desiredRPM = 2500;
const double maxIdleTPS = 0.15;
double desiredO2 = 0.5;
double O2V = 0;

// Pulse Width counters
long pulseTime;
int rpmIncrement = 250;
int maxRPM = 7999;
int arraySize = maxRPM / rpmIncrement + 1;
unsigned long totalPulse[32];
unsigned long sumPulse = 0;
int arrayIndex = 0;

// Pulse adjustment values
double fuelRatio = 14.7; // grams per grams
double idleVal  = 1.5;
double resetVal = 0.5;
double startupVal = 0.5;
double lastTPS;
double TPSx;
int endRPM = 80000;

double fuelRatioTable [32] =
   //                  0000-0249 0250-0499 0500-0749 0750-0999
                    {    14.9  ,   14.9  ,   14.9  ,   14.9,  //0000 - 0999
                         14.9  ,   14.9  ,   14.9  ,   14.9,  //1000 - 1999
                         14.9  ,   14.9  ,   14.9  ,   14.9,  //2000 - 2999
                         15.4  ,   15.4  ,   15.4  ,   15.4,  //3000 - 3999
                         15.4  ,   15.4  ,   15.4  ,   15.4,  //4000 - 4999
                         14.7  ,   14.7  ,   14.7  ,   14.7,  //5000 - 5999
                         14.7  ,   14.7  ,   14.7  ,   14.7,  //6000 - 6999
                         14.7  ,   14.7  ,   14.7  ,   14.7 };//7000 - 7999

// FEEDBACK LOOP GLOBAL VARIABLES //
String lastOperation = "+";
int LastRPMGradient = 500;
double adjustAFR= 0.0001;
long int nAdditions = 0;
long int nSubtractions = 0;
const int minDRPM = 2000;
const int maxDRPM = 2500;
////////////////////////////////////

#endif
