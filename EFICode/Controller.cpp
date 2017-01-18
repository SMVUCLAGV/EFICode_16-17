#include "Runner.h"
#include "Parameters.h"

bool Controller::getCommand() {
  //Check if there is a command in serial, if so, grab it and figure out what function to call
  if (Serial.available() >= 2) {
    String message = Serial.readString();
    String iden = message.substring(0,2);
    int id = iden.toInt();
    String val = message.substring(2);
    switch(id) {
      case 0:
        //TODO: Arduino RESET
        break;
      case 1:
        //TODO: Start Sending Data
        break;
      case 2:
        //TODO: Stop Sending Data
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
        break;
      case 7:
        //TODO: Set Current Fuel Ratio
        break;
      case 8:
        //TODO: Set Reset Fuel Ratio
        break;
      case 9:
        //TODO: Set desired RPM
        break;
      case 10:
        //TODO: Set desired O2
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

bool Controller::setDesiredO2(int dO2) {
  desiredO2 = dO2;
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
