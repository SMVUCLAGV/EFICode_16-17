#include "Controller.h"

#include "Constants.h"
#include "TimerThree.h"
//#include "EEPROM.h"

Controller *c;
void countRev();
void pulseOff();
void enableINJ();

void setup() {
  // Construct Controller object.
  c = new Controller();

  // Update all sensor values to current values.
  c->readSensors();

  // Attach rpm detector to revolution counter interrupt.
  attachInterrupt(HES_Interrupt, countRev, FALLING);

  // Initialize pulseOff timer, but do not attach the interrupt until it is necessary.
  Timer3.initialize();
}

void loop() {
  //TODO: Abstract this away

  // Considers the engine off if it falls below a certain RPM. When this happens,
  // the injector must be disabled.
  if (c->detectEngineOff()) {
    c->revolutions = 0;
    c->RPM = 0;
    c->lastRPMCalcTime = micros();
    if (!c->INJisDisabled) {
      disableINJ();
    }
  }

  //Update RPM
  if (c->revolutions >= c->revsPerCalc) {
    //Calculate RPM each cycle
    c->RPM = c->getRPM(micros() - c->lastRPMCalcTime, c->revolutions);
    c->revolutions = 0;
    c->lastRPMCalcTime = micros();
    // Should also dynamically change revsPerCalc. At lower RPM
    // the revsPerCalc should be lower but at higher RPM it should be higher.
  }

  // Update Controller with most recent sensor values.
  c->readSensors();

  // Look up injection time on each loop cycle
  c->lookupPulseTime();

  // Adjust basePulseTime Values by using feedback loop with O2 sensor.
  c->AFRFeedback();
  
  // Only send data if told to do so.
  if (c->currentlySendingData) {
    if (micros() - c->lastSerialOutputTime >= c->minTimePerSampleReported) {
      c->sendCurrentData();
      c->lastSerialOutputTime = micros();
    }
  }

  // Check for commands from Serial port.
  c->getCommand();
}

void countRev() {
  if (c->INJisDisabled) {
    enableINJ();
  }
  c->countRevolution();
}

void pulseOff() {
  // When it's time to turn the injector off, follow these steps and turn it off
  // ISR for Timer3 interrupt
  digitalWrite(INJ_Pin, LOW);

  // Save the amount of time the injector pin spent HIGH.
  c->realPulseTime = (micros() - c->lastRPMCalcTime) / 1000000;
  c->totalPulseTime += c->realPulseTime;
}

void enableINJ() {
  Timer3.attachInterrupt(pulseOff, c->injectorPulseTime);
  c->INJisDisabled = false;
}

void disableINJ() {
  Timer3.detachInterrupt();
  pulseOff();
  c->INJisDisabled = true;
}

