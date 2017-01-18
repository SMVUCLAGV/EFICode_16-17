#include "Arduino.h"

#ifndef PARAMETERS_WRAPPER_H
#define PARAMETERS_WRAPPER_H

// RPM counters
unsigned long lastRPMCalculationTime = 0;

// Time counters
long lastSerialOutputTime = 0;
long lastRPMCalcTime;
long lastInterrupt;
int delayCount = 0;

// Pulse Width counters
long pulseTime;
int maxRPM = 7999;
int arraySize = maxRPM / rpmIncrement + 1;
int arrayIndex = 0;

#endif