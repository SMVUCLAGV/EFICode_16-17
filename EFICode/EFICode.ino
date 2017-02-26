#include "Controller.h"

#include "Constants.h"
#include "TimerThree.h"
//#include "EEPROM.h"

Controller *c;

void setup() {
  // Construct Controller object.
  c = new Controller();

  // Update all sensor values to current values.
  c->readSensors();

  // Attach rpm detector to revolution counter interrupt.
  attachInterrupt(digitalPinToInterrupt(HES_Pin), countRev, FALLING);

  // Initialize pulseOff timer, but do not attach the interrupt until it is necessary.
  Timer3.initialize();
}

void loop() {
  //Update RPM if needed.
  c->updateRPM();
 
  // Checks the status of the engine. e.g., detects whether the engine is on or off.
  c->checkEngineState();

  // Update Controller with most recent sensor values.
  c->readSensors();

  // Look up injection time on each loop cycle
  c->lookupPulseTime();

  // Adjust injectorBasePulseTime[][] Values by using feedback loop with O2 sensor.
  c->AFRFeedback();

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

