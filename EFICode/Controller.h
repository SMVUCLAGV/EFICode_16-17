#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Constants.h"

class Controller {
public:
  // Constructor
  Controller();

  // Updates sensor values.
  bool readSensors();

  // Sends the current sensor values and requested parameter information.
  void sendCurrentData();

  // If the currentlySendingData flag is set and the minTimePerSampleReported
  // has passed since that last transmission, sends out a data sample.
  void trySendingData();

  // Possibly obsolete.
  void printEndingData();

  // Checks the serial read buffer for incoming bytes. If bytes are found, acts
  // according to the documentation specified in EFI Documentation.
  bool getCommand();

  // Increments the counted number of revolutions since the last RPM update.
  // Every 2nd revolution, switches the INJ_Pin to HIGH and sets a timer for
  // the amount of time equal to the most recent calculated pulse time.
  void countRevolution();

  // Updates the RPM reading by dividing the total number of revolutions since
  // the last update by the number of minutes that have passed.
  void updateRPM();

  // Performs 2D interpolation to lookup a pulse time for the given engine conditions.
  void lookupPulseTime();

  // If singleVal is true, determines the pulse time for the specified cell [row][col]
  // of the AFR Table in units of microseconds times kelvin.
  // If singleVal is false, determines the pulse time for every single AFR Table value
  // in the AFR Table. In its current state, this will overwrite any adjustments that
  // the O2 sensor feedback loop has made to the base pulse times.
  void calculateBasePulseTime(bool singleVal, int row, int col);

  // Sets the currentlySendingData flag to true, allowing the controller to report
  // sampled data back to the DAQ system.
  bool startDataTransmission();

  // Sets the currentlySendingData flag to false, preventing the controller from
  // reporting sampled data back to the DAQ system.
  bool stopDataTransmission();

  // Returns true if the engine drops below the minimum RPM to consider the engine running.
  bool detectEngineOff();

  // Checks to see if the engine is on or off. If the engine switches state since the last
  // check, changes parameters accordingly.
  void checkEngineState();

  // Turns the injector on if it is not disabled.
  void pulseOn();

  // Turns the injector off.
  void pulseOff();

  // Attaches the interrupt timer for shutting the injector off and 
  // sets the INJisDisabled flag to false.
  void enableINJ();

  // Detaches the interrupt timer for shutting the injector off and
  // sets the INJisDisabled flag to true.
  void disableINJ();

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

  // OBSOLETE!!
  int desiredRPM;
  double desiredAFR;
  double fuelRatio; //unitless
  double idleVal;
  double resetVal;
  double startupVal;
  // END OF OBSOLETE!

  void AFRFeedback();
  long interpolate2D(int blrow, int blcol, double x, double y);
  
private:
  // Has a value of true if the timer3 interrupt is detached from the "pulseOff" function.
  // Also prevents the injector from pulsing on if true.
  bool INJisDisabled;

  int revsPerCalc;
  int revolutions;
  unsigned long totalRevolutions;
  
  long totalPulseTime;
  long lastRPMCalcTime;
  long injectorPulseTime;
  int delayCount;
  bool currentlySendingData;
  long int minTimePerSampleReported;
  long lastSerialOutputTime;
  int mapIndex;
  int rpmIndex;
  
  int RPM;
  double TPS;
  double ECT;
  double IAT;
  double MAP;
  double AFR;
  double AFRVolts;

  double fuelRatioTable[numTableRows][numTableCols];
  long injectorBasePulseTimes[numTableRows][numTableCols];
};

#endif
