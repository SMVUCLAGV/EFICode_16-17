#ifndef PARAMETERS_H
#define PARAMETERS_H

// RPM counters
extern unsigned long lastRPMCalculationTime;
extern int RPM;

// Time counters

extern long lastRPMCalcTime;
extern long lastInterrupt;
extern int delayCount;

// Feedback loop values
extern double O2V;

// Pulse Width counters
extern long pulseTime;
extern int rpmIncrement;
extern int maxRPM;
extern int arraySize;
extern int arrayIndex;

// Pulse adjustment values
extern double lastTPS;
extern int endRPM;

extern double fuelRatioTable [32];//7000 - 7999

#endif
