#ifndef CONTROLLER_H
#define CONTROLLER_H

//#include <queue>

//////////////////////////////////////////////////////////////////////////////
// CONTROLLER CLASS                                                         //
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
// an array of doubles corresponding to our fuel adjustment table           //
//////////////////////////////////////////////////////////////////////////////

/*Syntax:
  idle ____
  fuel ____
  dRPM ____
  deO2 ____
  ar19 ____
*/

const int MAX_QUEUE_SIZE = 100;

// CommandType can be changed if necessary
typedef String CommandType;

class Parameters;

class Controller {
public:
    Controller() {}

    void setParameters(Parameters* p) {params = p;}

    bool getCommand();
    // Read input from the serial port. If input is obtained, return true. Otherwise, return false.

    bool executeCommand();
    // Tell the controller to execute the command at the front of the command queue.
    // We will take index 0 as the front of the command queue (Assuming an array structure).
    // *If it is successfully executed, return true and eliminate that command from the queue.
    // *Otherwise, return false and put the failed command at the back of the queue.
    // **REMEMBER**: our system needs to be robust. Even if a command fails the system needs to
    // keep chugging on.

// Feel free to change up the private members as needed in order to maximize efficiency.
// Some possible ones are listed here in order to make the desired functionality clear.
private:
    //.....................
    bool set_idleVal();
    bool set_fuelRatio();
    //.....................etc...

    //Queue<CommandType> queue;

    Parameters* params;
};

#endif // CONTROLLER_H*/
