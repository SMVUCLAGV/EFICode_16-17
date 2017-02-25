#include "Controller.h"

#include "Arduino.h"

bool Controller::getCommand() {
  //Check if there is a command in serial, if so, grab it and figure out what function to call
  if (Serial.available() >= 1) {
    char id = Serial.read();
    switch(id) {
      byte acknowledgement [5];
      acknowledgement[0] = id;
      ((unsigned int*)(&acknowledgement[1]))[0] = 0x80000002;
      case 0: //Arduino Reset
      {
        Serial.write(acknowledgement, 5);
        //digitalWrite(RST_PIN, HIGH);
      }
      case 1: //Start Sending Data
      {
        Serial.write(acknowledgement, 5);
        startDataTransmission();
        break;
      }
      case 2: //Stop Sending Data
      {
        stopDataTransmission();
        Serial.write(acknowledgement, 5);
        break;
      }
      case 3: //Synchronize Parameter Order
        //TODO: This
        break;
      case 4: //Update Arduino AFR Table
        {
          int numCols = Serial.read();
          int numRows = Serial.read();
          for (int x = 0; x < numTableRows; x++) {
            for (int y = 0; y < numTableCols; y++) {
              double* currentRatio;
              Serial.readBytes((char*) currentRatio, 4);
              fuelRatioTable[x][y] = *currentRatio;
            }
          }
          calculateBasePulseTime(false, 0, 0);
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
      {
        double idleVal;
        if (Serial.readBytes((byte*)&idleVal, 4) < 4) {
          return false;
        }
        setIdleVal(idleVal);
        Serial.write(acknowledgement,5);
        break;
      }
      case 7: //Current Fuel Ratio
      {
        double currentRatio;
        if (Serial.readBytes((byte*)&currentRatio, 4) < 4) {
          return false;
        }
        setFuelRatio(currentRatio);
        Serial.write(acknowledgement,5);
        break;
      }
      case 8: //Reset Fuel Ratio
      {
        double resetVal;
        if (Serial.readBytes((byte*)&resetVal, 4) < 4) {
          return false;
        }
        setResetRatio(resetVal);
        Serial.write(acknowledgement,5);
        break;
      }
      case 9: //Desired RPM
      {
        int dRPM;
        if (Serial.readBytes((byte*)&dRPM, 4) < 4) {
          return false;
        }
        setDesiredRPM(dRPM);
        Serial.write(acknowledgement,5);
        break;
      }
      case 10: //Desired O2
      {
        double dAFR;
        if (Serial.readBytes((byte*)&dAFR, 4) < 4) {
          return false;
        }
        setDesiredAFR(dAFR);
        Serial.write(acknowledgement,5);
        break;
      }
      default:
        break;
    }
  }
}

bool Controller::startDataTransmission() {
  currentlySendingData = true;
  return true;
}

bool Controller::stopDataTransmission() {
  currentlySendingData = false;
  return true;
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

bool Controller::setDesiredAFR(double dAFR) {
  desiredAFR = dAFR;
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
