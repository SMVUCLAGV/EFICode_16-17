#ifndef PARAMETERS_H
#define PARAMETERS_H

extern int openTime;

// Revolution counters
extern int revolutions;
extern unsigned long totalRevolutions;

// RPM counters
extern unsigned long lastRPMCalculationTime;
extern int RPM;

// Time counters
extern long lastSerialOutputTime;
extern long lastRPMCalcTime;
extern long pulseTimeReal;
extern long lastInterrupt;
extern int delayCount;

// Feedback loop values
extern int desiredRPM;
const double maxIdleTPS = 0.15;
const double desiredO2 = 0.5;
extern double O2V;

// Pulse Width counters
extern long pulseTime;
extern int rpmIncrement;
extern int maxRPM;
extern int arraySize;
extern unsigned long totalPulse[32];
extern unsigned long sumPulse;
extern int arrayIndex;

// Pulse adjustment values
extern double fuelRatio; // grams per grams
extern double idleVal;
extern double resetVal;
extern double startupVal;
extern double lastTPS;
extern double TPSx;
extern int endRPM;

extern double fuelRatioTable [32];//7000 - 7999

// FEEDBACK LOOP GLOBAL VARIABLES //
//String lastOperation = "+";
extern int LastRPMGradient;
extern double adjustAFR;
extern long int nAdditions;
extern long int nSubtractions;
extern const int minDRPM;
extern const int maxDRPM;
////////////////////////////////////

#endif
