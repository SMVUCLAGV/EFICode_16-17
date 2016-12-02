#include "Controller.h"
#include "Parameters.h"

bool getCommand() { //Check if there is a command in serial, if so, grab it and figure out what function to calculated
// send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.readString();
    // figure out the command:
    char firstLetter = incomingByte[0];
    switch(firstLetter) {
      case 'i':
        setIdleVal(incomingByte.substring(5).toFloat());
        return true;
      case 'f':
        setFuelRatio();
        return true;
      case 'd':
        if(incomingByte[1] == 'R') {
          setDesiredRPM(incomingByte.substring(5).toFloat());
          return true;
        }
        else {
          setDesiredO2(incomingByte.substring(5).toFloat());
          return true;
        }
      case 'a':
        setFuelRatioTable(incomingByte.substring(incomingByte[2],incomingByte[4]).toInt(), incomingByte.substring(5).toFloat());
        return true;
      case 's':
        setStartupValue(incomingByte.substring(5).toFloat());
        return true;
      default:
        return false;
    }
    return false;
}

bool setIdleVal(double val) {
  idleVal = val;
  return true;
}

bool setFuelRatio(double val) {
  fuelRatio = val;
  return true;
}

bool setDesiredRPM(int dRPM) {
  desiredRPM = dRPM;
}

bool setDesiredO2(int dO2) {
  desiredO2 = dO2;
  return true;
}

bool setFuelRatioTable(int index, double ratio) {
  //TODO: Finish when Fuel Ratio Table is implemented
  return true;
}

bool setStartupValue(double val) {
  startupVal = val;
  return true;
}
