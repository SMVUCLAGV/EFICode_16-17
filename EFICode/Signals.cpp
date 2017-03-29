#include "Controller.h"

#include "Arduino.h"

#define ACKNOWLEDGEMENT_END_VAL 0x80000002
#define timeoutDuration 100

const uint16_t checkSumInitial= 0xFFFF;

bool Controller::getCommand() {
  //Check if there is a command in serial, if so, grab it and figure out what function to call
  if (Serial.available() >= 1) {
    // Receive the signal ID from the first byte in the incoming message.
    byte id = Serial.read();
    //id = id - '0';
    // Allocate space for the general acknowledgement message.
    byte acknowledgement [8];
    // Set the first byte of the acknowledgement message to the value of the signal ID.
    acknowledgement[0] = id;
    // Set the last four bytes of the acknowledgement message to the value of ACKNOWLEDGEMENT_END_VAL.
    ((unsigned long*)(&acknowledgement[1]))[0] = ACKNOWLEDGEMENT_END_VAL;
    switch(id) {
      case 0: //Arduino Reset
      {
         long loopStartTime = millis();
          while(Serial.available() < 2)
          {
            if (millis() - loopStartTime > timeoutDuration)
            {
              return false;
            }
          }
          uint16_t checkSum;
          // The next 2 bytes are expected to be a 2 byte checkSum.
          if (Serial.readBytes((char*) &checkSum, 2) != 2)
          {
            // Should send error code back in this case.
            return false;
          }
          if(checkSum!=checkSumInitial-id)
          {
            //if checkSum doesn't match expected checkSum, not a valid command
            return false;
          }
        // Confirm reception of the message.
        Serial.write(acknowledgement, 5);
        // Write HIGH to the reset pin to perform a hardware reset of the controller.
        //digitalWrite(RST_PIN, HIGH);
      }
      break;
      case 1: //Start Sending Data
      {
          long loopStartTime = millis();
          while(Serial.available() < 2)
          {
            if (millis() - loopStartTime > timeoutDuration)
            {
              return false;
            }
          }
          uint16_t checkSum;
          // The next 2 bytes are expected to be a 2 byte checkSum.
          if (Serial.readBytes((char*) &checkSum, 2) != 2)
          {
            // Should send error code back in this case.
            return false;
          }
          if(checkSum!=checkSumInitial-id)
          {
            //if checkSum doesn't match expected checkSum, not a valid command
            return false;
          }
        // Send an acknowledgement message back to the DAQ system.
        Serial.write(acknowledgement, 5);
        // Begin transmitting data to the DAQ system.
        startDataTransmission();
        break;
      }
      case 2: //Stop Sending Data
      {
          long loopStartTime = millis();
          while(Serial.available() < 2)
          {
            if (millis() - loopStartTime > timeoutDuration)
            {
              return false;
            }
          }
          uint16_t checkSum;
          // The next 2 bytes are expected to be a 2 byte checkSum.
          if (Serial.readBytes((char*) &checkSum, 2) != 2)
          {
            // Should send error code back in this case.
            return false;
          }
          if(checkSum!=checkSumInitial-id)
          {
            //if checkSum doesn't match expected checkSum, not a valid command
            return false;
          }
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
          long loopStartTime = millis();
          while(Serial.available() < 9)
          {
            if (millis() - loopStartTime > timeoutDuration)
            {
              return false;
            }
          }
          // The next byte in the serial buffer is expected to be a row index (starts at (0,0))
          byte rowNum = Serial.read();
          // The next byte in the serial buffer is expected to be a column index (starts at (0,0))
          byte colNum = Serial.read();
          // Throw out alignment byte
          Serial.read();
          // Allocate space for 4 bytes and interpret them as a double.
          double val;
          // The next 4 bytes are expected to be a 4 byte float or double.
          if (Serial.readBytes((char*) &val, 4) != 4)
          {
            // Should send error code back in this case.
            return false;
          }
          uint16_t checkSum;
          if (Serial.readBytes((char*) &checkSum, 2) != 2)
          {
            // Should send error code back in this case.
            return false;
          }
           unsigned char * valuePointer = reinterpret_cast<unsigned char *>(&val);
          if(checkSum!= checkSumInitial - id -(rowNum<<1) - (colNum<<2) - ((uint16_t)(valuePointer[0]) << 3) - ((uint16_t)(valuePointer[1]) << 4) - ((uint16_t)(valuePointer[2]) << 5) - ((uint16_t)(valuePointer[3]) << 6))
          {
             //if checkSum doesn't match expected checkSum, not a valid command
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
            byte rowColAcknowledgement[8];
            // The first byte is the id
            rowColAcknowledgement[0] = id;
            // The next byte is the row number
            rowColAcknowledgement[1] = rowNum;
            // The next byte is the column number
            rowColAcknowledgement[2] = colNum;
            // The next 4 bytes are the ACKNOWLEDGEMENT_END_VAL unsigned int.
            ((unsigned long*)(&rowColAcknowledgement[4]))[0] = ACKNOWLEDGEMENT_END_VAL;
            // Send the acknowledgement out.
            Serial.write(rowColAcknowledgement,8);
            return true;
          }
          return false;
        }
        break;
      case 5: //Update DAQ AFR Table
        {
          long loopStartTime = millis();
          while(Serial.available() < 4)
          {
            if (millis() - loopStartTime > timeoutDuration)
            {
              return false;
            }
          }
          // The next byte is the row number being asked for by the DAQ system.
          byte rowNum = Serial.read();
          // The next byte is the column number being asked for by the DAQ system.
          byte colNum = Serial.read();
          //Throws away alignmentPadding
          Serial.read();
          // Check to see if the requested row and column numbers are in the bounds of the table.
           uint16_t checkSum;
          if (Serial.readBytes((char*) &checkSum, 2) != 2)
          {
            // Should send error code back in this case.
            return false;
          }
          if(checkSum!= checkSumInitial - id -(rowNum<<1) - (colNum<<2))
          {
             //if checkSum doesn't match expected checkSum, not a valid command
            return false;
          }
          if (rowNum < numTableRows && colNum < numTableCols && rowNum > 0 && colNum > 0)
          {
            //Serial.print("Received valid row and column numbers.\n");
            // Save the requested value.
            double requestedVal = fuelRatioTable[rowNum][colNum];
            // Send the requested value to the DAQ system.
            unsigned char returnMsg[12];
            returnMsg[0] = 5;
            returnMsg[1] = rowNum;
            returnMsg[2] = colNum;
            ((double*)(&returnMsg[4]))[0] = requestedVal;
            ((unsigned long*)(&returnMsg[8]))[0] = ACKNOWLEDGEMENT_END_VAL;
            //Serial.println(String("5")+","+
            //String(rowNum)+","+
            //String(colNum)+","+
            //String(requestedVal)+","+
            //String(ACKNOWLEDGEMENT_END_VAL));
            Serial.write(returnMsg, 12);
            return true;
          }
          return false;
        }
        break;
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
