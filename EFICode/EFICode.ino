#include "Controller.h"

#include "Constants.h"
#include "TimerThree.h"
//#include "EEPROM.h"

Controller *c;

void setup() {
  // Construct Controller object.
  c = new Controller();

  // Update all sensor values to current values.
  c->updateRPM();
  c->readSensors();

  // Attach rpm detector to revolution counter interrupt.
  attachInterrupt(digitalPinToInterrupt(HES_Pin), dummy, FALLING);
  detachInterrupt(digitalPinToInterrupt(HES_Pin));
  attachInterrupt(digitalPinToInterrupt(HES_Pin), countRev, FALLING);

  // Initialize pulseOff timer.
  Timer3.initialize(1000000);

  // Attach the interrupt for INJ pulse modulation.
  Timer3.attachInterrupt(dummy);
  Timer3.detachInterrupt();
  Timer3.attachInterrupt(handle_pulseTimerTimeout);
  
  // Immediately stop the timer.
  Timer3.stop();
}

void loop() {
  //Update RPM if needed.
  c->updateRPM();

  // Update Controller with most recent sensor values.
  c->readSensors();

  // Look up injection time on each loop cycle
  c->lookupPulseTime();

  // Adjust injectorBasePulseTime[][] Values by using feedback loop with O2 sensor.
  c->AFRFeedback();
 
  // Checks the status of the engine. e.g., detects whether the engine is on or off.
  c->checkEngineState();

  // Attempt to send sensor data to the DAQ system. Will only occur if the
  // currentlySendingData flag is set to true. This flag is set by sending 
  // the ID 1 signal to the controller.
  c->trySendingData();

  // Check for commands from Serial port.
  c->getCommand();
}

void countRev() {
  c->countRevolution();
}

void handle_pulseTimerTimeout() {
  c->pulseOff();
}

void dummy() {}

