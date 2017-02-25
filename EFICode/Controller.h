#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "constants.h"

struct Controller {
  Controller();
  bool readSensors();
  void sendCurrentData();
  void printEndingData();
  bool getCommand();
  void countRevolution();
  void pulseOff();
  void calculatePulseTime();
  void lookupPulseTime();
  void calculateBasePulseTime(bool singleVal, int row, int col);
  bool startDataTransmission();
  bool stopDataTransmission();

  
  bool detectEngineOff();
  bool INJisDisabled;

  int getRPM (long int timePassed, int rev);
  double getTPS();
  double getTemp(int pin);
  double getMAP();
  double getAFR();

  void initializeParameters();
  bool setIdleVal(double val);
  bool setFuelRatio(double val);
  bool setDesiredRPM(int dRPM);
  bool setDesiredAFR(double dAFR);
  bool setFuelRatioTable(int index, double ratio); //Set the fuel ratio at the appropriate index in the fuel ratio table
  bool setStartupValue(double val);
  bool setResetRatio(double val);

  int RPM;
  double TPS;
  double ECT;
  double IAT;
  double MAP;
  double AFR;
  double AFRVolts;

  int desiredRPM;
  double desiredAFR;

  int revsPerCalc;
  int revolutions;
  unsigned long totalRevolutions;

  double tempAlpha;
  double tempBeta;
  double tempInputVal; 

  double fuelRatio; //unitless
  double idleVal;
  double resetVal;
  double startupVal;
  
  int mapIndex;
  int rpmIndex;
  
  unsigned long totalPulse[numTableCols];
  long totalPulseTime;

  long lastRPMCalcTime;
  long injectorPulseTime;
  long realPulseTime;
  int delayCount;

  long lastSerialOutputTime;
  long lastInterrupt;
  int openTime;

  bool currentlySendingData;
  long int minTimePerSampleReported;

  //WARNING: From here down are fuel ratio parameters. Don't change these unless you know what you are doing!

  double fuelRatioTable[numTableRows][numTableCols];

  long injectorBasePulseTimes[numTableRows][numTableCols];
  void AFRFeedback();

  long interpolate2D(int blrow, int blcol, double x, double y) {
    // Takes the coordinate of the bottom left corner of the square to perform 2D interpolation over.
    // x and y must be given in unit form. i.e., y = (yc-y1)-(y2-y1) and x = (xc-x1)-(x2-x1)
    // (0 <= y <= 1 and 0 <= x <= 1)
    return
    injectorBasePulseTimes[blrow][blcol]*(1-y)*(1-x)+
    injectorBasePulseTimes[blrow+1][blcol]*(y)*(1-x)+
    injectorBasePulseTimes[blrow][blcol+1]*(1-y)*(x)+
    injectorBasePulseTimes[blrow+1][blcol+1]*(y)*(x);
  }
};

#endif
