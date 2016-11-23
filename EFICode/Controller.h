#ifndef CONTROLLER_H
#define CONTROLLER_H

//#include <queue>

//////////////////////////////////////////////////////////////////////////////
// CONTROLLER                                                         //
// ======================================================================== //
// Facilitates real time communication between a laptop and the Arduino.    //
// Should be able to take input from the serial port and efficiently        //
// dispatch that input as commands for adjusting parameters in the Arduino. //
// ======================================================================== //
// PARAMETERS TO ADJUST:                                                    //
// ======================================================================== //
// idleVal                                                                  //
// fuelRatio                                                                //
// desiredRPM                                                               //
// desired02                                                                //
// startupVal                                                               //
// an array of doubles corresponding to our fuel adjustment table           //
//////////////////////////////////////////////////////////////////////////////

/*Syntax:
  4 characters for identifier, 1 character for space, 4 characters for value
  idle ____
  fuel ____
  dRPM ____
  deO2 ____
  ar19 ____
  strt ____
*/

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

//These functions modify the appropriate parameters
bool setIdleVal(double val);
bool setFuelRatio(double val);
bool setDesiredRPM(int dRPM);
bool setDesiredO2(int dO2);
bool setFuelRatioTable(int index, double ratio); //Set the fuel ratio at the appropriate index in the fuel ratio table
bool setStartupValue(double val);

bool setIdleVal(double val) {
  parameters.idleVal = val;
  return true;
}

bool setFuelRatio(double val) {
  parameters.fuelRatio = val;
  return true;
}

bool setDesiredRPM(int dRPM) {
  parameters.desiredRPM = dRPM;
}

bool setDesiredO2(int dO2) {
  parameters.desiredO2 = dO2;
  return true;
}

// cannot finish setFuelRatioTable function until table is created

bool setStartupValue(double val) {
  parameters.startupVal = val;
  return true;
}

#endif // CONTROLLER_H*/
