#include "Controller.h"

#include "Arduino.h"

bool Controller::getCommand() {
  //Check if there is a command in serial, if so, grab it and figure out what function to call
  if (Serial.available() >= 2) {
    String message = Serial.readString();
    String iden = message.substring(0,2);
    int id = iden.toInt();
    String val = message.substring(2);
    double vald = 0;
    int vali = 0;
    switch(id) {
      case 0:
        //TODO: Arduino RESET
        break;
      case 1: //Start Sending Data
        currentlySendingData = true;
        break;
      case 2: //Stop Sending Data
        currentlySendingData = false;
        break;
      case 3:
        //TODO: Synchronize Parameter Order
        break;
      case 4:
        //TODO: Update Arduino AFR Table
        break;
      case 5:
        //TODO: Update DAQ AFR Table
        break;
      case 6:
        //TODO: Set Idle Fuel Ratio
        vald = val.toFloat();
        setIdleVal(vald);
        break;
      case 7:
        //TODO: Set Current Fuel Ratio
        vald = val.toFloat();
        setFuelRatio(vald);
        break;
      case 8:
        //TODO: Set Reset Fuel Ratio
        vald = val.toFloat();
        setResetRatio(vald);
        break;
      case 9:
        //TODO: Set desired RPM
        vali = val.toInt();
        setDesiredRPM(vali);
        break;
      case 10:
        //TODO: Set desired O2
        vald = val.toFloat();
        setDesiredO2(vald);
        break;
      default:
        break;
    }
  }
}

bool Controller::setIdleVal(double val) {
  idleVal = val;
  return true;
}

bool Controller::setFuelRatio(double val) {
  fuelRatio = val;
  return true;
}

bool Controller::setDesiredRPM(int dRPM) {
  desiredRPM = dRPM;
}

bool Controller::setDesiredO2(double dO2) {
  desiredOIN = dO2;
  return true;
}

bool Controller::setResetRatio(double val) {
  resetVal = val;
  return true;
}

bool Controller::setFuelRatioTable(int index, double ratio) {
  //TODO: Finish when Fuel Ratio Table is implemented
  return true;
}

bool Controller::setStartupValue(double val) {
  startupVal = val;
  return true;
}
