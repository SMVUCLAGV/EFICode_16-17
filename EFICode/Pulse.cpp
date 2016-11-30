#include "Pulse.h"
#include "parameters.h"
#include "Sensors.h"
#include "constants.h"

void countRevolution () {
  revolutions++;
  totalRevolutions++;
  if (totalRevolutions % 2 == 1 || totalRevolutions < 25) {
    //&& RPM > desiredRPM)
    //Inject on every second revolution because this is a 4 stroke engine
    digitalWrite(INJ_Pin, HIGH);
    lastRPMCalcTime = millis();
    delayCount = 0;
    Timer3.setPeriod(pulseTime);
    Timer3.restart();
  }
}

void pulseOff() {
  //When it's time to turn the injector off, follow these steps and turn it off
  //ISR for Timer3 interrupt
  if (delayCount == 1) {
    //Determined experimentally.  I think the Timer interrupts in the very beginning.  To avoid this first interrupt, I switched to 1.  Can probably be revised and improved
    digitalWrite(INJ_Pin, LOW);
    pulseTimeReal= (micros() - lastRPMCalcTime) / 1000;
    arrayIndex = RPM / rpmIncrement;
    totalPulse[arrayIndex] += pulseTimeReal;
    //totalPulse[arrayIndex] += pulseTime / 1000;

  }
  if (delayCount > 100) {
    delayCount = 2;
  }
   delayCount++;
}

void inject() {
  //(displ * rpm)/ 2 = cm^3/s air per power stroke
  // ideal gas law PV = nRT
  // T = IAT
  // P = MAP
  // mult n by mm of air, divide by 14.7
  TPSx = 1 + TPS_MOD*(getTPS() - lastTPS);
  lastTPS = getTPS();
  if (totalRevolutions > 2 && lastTPS <= maxIdleTPS) {
    //RPM-based feedback loop
    if (RPM > desiredRPM) {
      idleVal = idleVal - .00001;
    }
    else if (RPM < desiredRPM) {
      idleVal = idleVal + .00001;
    }
  }
  double val = getMAP() * injectionConstant / (getTemp(IAT_Pin) * fuelRatio * injectorFuelRate);
  //Calculate pulse time
  if (lastTPS <= maxIdleTPS) {
    pulseTime = (val * 1000000 * idleVal * startupVal * TPSx) + openTime;
  }
  else {
    pulseTime = (val * 1000000 * startupVal * TPSx) + openTime;
  }
}
