#include "Controller.h"

#include "Arduino.h"

bool Controller::getCommand() {
  //Check if there is a command in serial, if so, grab it and figure out what function to call
  if (Serial.available() >= 1) {
    char id = Serial.read();
    double val_d = 0;
    long val_i = 0;
    switch(id) {
      case 0: //Arduino Reset
        //TODO: This
        //digitalWrite(RST_PIN, HIGH);
        break;
      case 1: //Start Sending Data
        currentlySendingData = true;
        break;
      case 2: //Stop Sending Data
        currentlySendingData = false;
        break;
      case 3: //Synchronize Parameter Order
        //TODO: This
        break;
      case 4: //Update Arduino AFR Table
        {
          int numCols = Serial.read();
          int numRows = Serial.read();
          for (int x = 0; x < 10; x++) {
            for (int y = 0; y < 32; y++) {
              double* currentRatio;
              Serial.readBytes((char*) currentRatio, 4);
              fuelRatioTable[x][y] = *currentRatio;
            }
          }
          calculatePulseTime(false, 0, 0);
        }
        break;
      case 5: //Update DAQ AFR Table
        Serial.write(10);
        Serial.write(36);
        for (int x = 0; x < 10; x++) {
          Serial.write((byte*)fuelRatioTable[x], 128);
        }
        break;
      case 6: //Idle Fuel Ratio
        if (Serial.readBytes((byte*)&val_d, 4) < 4) {
          return false;
        }
        setIdleVal(val_d);
        break;
      case 7: //Current Fuel Ratio
        if (Serial.readBytes((byte*)&val_d, 4) < 4) {
          return false;
        }
        setFuelRatio(val_d);
        break;
      case 8: //Reset Fuel Ratio
        if (Serial.readBytes((byte*)&val_d, 4) < 4) {
          return false;
        }
        setResetRatio(val_d);
        break;
      case 9: //Desired RPM
        if (Serial.readBytes((byte*)&val_i, 4) < 4) {
          return false;
        }
        setDesiredRPM(val_i);
        break;
      case 10: //Desired O2
        if (Serial.readBytes((byte*)&val_d, 4) < 4) {
          return false;
        }
        setDesiredO2(val_d);
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
  return true;
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
