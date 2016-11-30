#include "TimerThree.h"
#include "constants.h"
#include "parameters.h"
#include "Sensors.h"
#include "Data.h"
#include "Pulse.h"

void setup() {
  pinMode(INJ_Pin, OUTPUT); //Sets injector pin to output mode
  Serial.begin(9600); //Initializes Serial output
  Serial.println("Time, ECT,    IAT,    MAP,     TPS,  OIN,  RPM,  DesPW,PWRe,Revs,Start,AFR"); //Prints headers for our data
  lastTPS = getTPS();
  attachInterrupt(2, countRevolution, FALLING);
  inject();
  Timer3.initialize(100000);
  Timer3.attachInterrupt(pulseOff, pulseTime); //Attaches timer-based interrupt to stop injector from injecting
  delayCount = 4;
  pulseTimeReal = 0;
  lastRPMCalcTime = millis();
  lastInterrupt = millis();
  for (int i = 0; i <= 31; i++) {
    totalPulse[i] = 0; //Initial fuel usage in each RPM range - the ranges are shown in parameters.h
  }
}

void loop() {
  if (RPM > endRPM) {
    //If the engine goes above our max RPM, shut it off and print out the following analytics
    fuelRatio = 0;
    openTime = 0;
    printEndingData();
  }
  if (revolutions >= 25) {
    //Calculate RPM each cycle
    RPM = getRPM(micros() - lastRPMCalculationTime, revolutions);
    startupVal = 1;
    revolutions = 0;
    lastRPMCalculationTime = micros();
  }
  inject(); //Calculate injection time on each loop cycle
  if (micros()-lastRPMCalculationTime >= 2000000) {
    //If RPM falls below 600, sets RPM to 0 meaning shut the engine off
    RPM = 0;
    startupVal = resetVal;
    totalRevolutions = 0;
  }
  if (micros()-lastSerialOutputTime>=500000) {
    //If it's been more than 500ms since we last printed data, print data
    printCurrentData();
    lastSerialOutputTime = micros();
  }
}
