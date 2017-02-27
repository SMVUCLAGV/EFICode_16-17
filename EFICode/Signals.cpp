#include "Controller.h"

#include "Arduino.h"

#define ACKNOWLEDGEMENT_END_VAL 0x80000002

bool Controller::getCommand() {
  //Check if there is a command in serial, if so, grab it and figure out what function to call
  if (Serial.available() >= 1) {
    // Receive the signal ID from the first byte in the incoming message.
    char id = Serial.read();
    switch(id) {
      // Allocate space for the general acknowledgement message.
      byte acknowledgement [5];
      // Set the first byte of the acknowledgement message to the value of the signal ID.
      acknowledgement[0] = id;
      // Set the last four bytes of the acknowledgement message to the value of ACKNOWLEDGEMENT_END_VAL.
      ((unsigned int*)(&acknowledgement[1]))[0] = ACKNOWLEDGEMENT_END_VAL;
      case 0: //Arduino Reset
      {
        // Confirm reception of the message.
        Serial.write(acknowledgement, 5);
        // Write HIGH to the reset pin to perform a hardware reset of the controller.
        //digitalWrite(RST_PIN, HIGH);
      }
      break;
      case 1: //Start Sending Data
      {
        // Send an acknowledgement message back to the DAQ system.
        Serial.write(acknowledgement, 5);
        // Begin transmitting data to the DAQ system.
        startDataTransmission();
        break;
      }
      case 2: //Stop Sending Data
      {
        // Send an acknowledgement message back to the DAQ system.
        Serial.write(acknowledgement, 5);
        // Stop sending data to the DAQ system.
        stopDataTransmission();
        break;
      }
      case 3: //Synchronize Parameter Order
        //TODO: This
        break;
      case 4: //Update Arduino AFR Table
        {
          // The next byte in the serial buffer is expected to be a row index (starts at (0,0))
          byte rowNum = Serial.read();
          // The next byte in the serial buffer is expected to be a column index (starts at (0,0))
          byte colNum = Serial.read();
          // Allocate space for 4 bytes and interpret them as a double.
          double val;
          // The next 4 bytes are expected to be a 4 byte float or double.
          if (Serial.readBytes((char*) &val, 4) != 4)
          {
            // Should send error code back in this case.
            return false;
          }
          // Check to make sure the row and columns are within the table.
          if (rowNum < numTableRows && colNum < numTableCols && rowNum > 0 && colNum > 0)
          {
            // Clip out of bounds AFR values to the valid range.
            val = constrain(val, MIN_AFR, MAX_AFR);
            // Set the AFR table value at the specified row and column to the recieved value.
            fuelRatioTable[rowNum][colNum] = val;
            // Calculate a base pulse time for the AFR table value that was changed.
            calculateBasePulseTime(true, rowNum, colNum);
            // Allocate space for the special acknowledgement that will be sent back to the DAQ system.
            byte rowColAcknowledgement[7];
            // The first byte is the id
            rowColAcknowledgement[0] = id;
            // The next byte is the row number
            rowColAcknowledgement[1] = rowNum;
            // The next byte is the column number
            rowColAcknowledgement[2] = colNum;
            // The next 4 bytes are the ACKNOWLEDGEMENT_END_VAL unsigned int.
            ((unsigned int*)(&rowColAcknowledgement[3]))[0] = ACKNOWLEDGEMENT_END_VAL;
            // Send the acknowledgement out.
            Serial.write(rowColAcknowledgement,7);
            return true;
          }
          return false;
        }
        break;
      case 5: //Update DAQ AFR Table
        {
          // The next byte is the row number being asked for by the DAQ system.
          byte rowNum = Serial.read();
          // The next byte is the column number being asked for by the DAQ system.
          byte colNum = Serial.read();
          // Check to see if the requested row and column numbers are in the bounds of the table.
          if (rowNum < numTableRows && colNum < numTableCols && rowNum > 0 && colNum > 0)
          {
            //Serial.print("Received valid row and column numbers.\n");
            // Save the requested value.
            double requestedVal = fuelRatioTable[rowNum][colNum];
            // Send the requested value to the DAQ system.
            byte returnMsg[11];
            returnMsg[0] = 5;
            returnMsg[1] = rowNum;
            returnMsg[2] = colNum;
            ((double*)(&returnMsg[3]))[0] = requestedVal;
            ((unsigned int*)(&returnMsg[7]))[0] = ACKNOWLEDGEMENT_END_VAL;
            //Serial.println(String("5")+","+
            //String(rowNum)+","+
            //String(colNum)+","+
            //String(requestedVal)+","+
            //String(ACKNOWLEDGEMENT_END_VAL));
            Serial.write((char*) &returnMsg, 11);
            return true;
          }
          return false;
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
    return true;
  }
  return false;
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
