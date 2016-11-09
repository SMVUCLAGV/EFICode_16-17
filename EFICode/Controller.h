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

bool getCommand(); //Check if there is a command in serial, if so, grab it and figure out what function to calculated

//These functions modify the appropriate parameters
bool setIdleVal(double val);
bool setFuelRatio(double val);
bool setDesiredRPM(int dRPM);
bool setDesiredO2(int dO2);
bool setFuelRatioTable(int index, double ratio); //Set the fuel ratio at the appropriate index in the fuel ratio table
bool setStartupValue(double val);

#endif // CONTROLLER_H*/
