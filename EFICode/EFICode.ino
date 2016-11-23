//Pin 9 Not Working 3/12/16
#include "TimerThree.h"
#include "constants.h"
#include "parameters.h"
#include "Sensors.h"
//#include "serial.h"

// FEEDBACK LOOP GLOBAL VARIABLES //
String lastOperation = "+";
int LastRPMGradient = 500;
double adjustAFR= 0.0001;
long int nAdditions = 0;
long int nSubtractions = 0;
const int minDRPM = 2000;
const int maxDRPM = 2500;
////////////////////////////////////

void serialOutput() {
  //Prints data on each of our sensors to Serial output
  Serial.print(millis());
  Serial.print(", ");
  Serial.print(getTemp(ECT_Pin));
  Serial.print(", ");
  Serial.print(getTemp(IAT_Pin));
  Serial.print(", ");
  Serial.print(getMAP());
  Serial.print(", ");
  Serial.print(getTPS());
  Serial.print(", ");
  Serial.print(getOIN());
  Serial.print(", ");
  Serial.print(RPM);
  Serial.print(", ");
  Serial.print(pulseTime);
  Serial.print(", ");
  Serial.print(pulseTimeReal);
  Serial.print(", ");
  Serial.print(totalRevolutions);
  Serial.print(", ");
  Serial.print(startupVal);
  Serial.print(", ");
  Serial.println(fuelRatioTable[RPM/rpmIncrement]);
}

void countRevolution () {
  //  Serial.println("Revolution counted");
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
    pulseTimeReal= (millis() - lastRPMCalcTime);
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

void setup() {
  pinMode(INJ_Pin, OUTPUT); //Sets injector pin to output mode
  Serial.begin(9600); //Initializes Serial output
  Serial.println("Time, ECT,    IAT,    MAP,     TPS,  OIN,  RPM,  DesPW,PWRe,Revs,Start,AFR"); //Prints headers for our data
  lastTPS = getTPS();
  attachInterrupt(2, countRevolution, FALLING);
  inject();
  Timer3.initialize(100000);
  Timer3.attachInterrupt(pulseOff, pulseTime); //Attaches timer-based interrupt to stop injector from injecting
  delayCount = 4;
  pulseTimeReal = 0;
  lastRPMCalcTime = millis();
  lastInterrupt = millis();
  for (int i = 0; i <= 31; i++) {
    totalPulse[i] = 0; //Initial fuel usage in each RPM range - the ranges are shown in parameters.h
  }
}

int determineDesiredRPM(const int& minRPM,const int& maxRPM) {
  double TPS = getTPS();
  int desiredRPM = minRPM + (maxRPM - minRPM) * TPS;
  return desiredRPM;
}

void loop() {
  if (Serial.available()) {
    //If serial output is not being used by anything else
    sumPulse = 0;
    int pulseUnder3000 = 0;
    for (int i = 0; i <= 31; i++) {
      sumPulse += totalPulse[i]; //figure out how much time we've spent with the injector on -> figure out how much fuel we've used
      if (i < 16) {
        pulseUnder3000 += totalPulse[i];
      }
      Serial.print(String(i*rpmIncrement) + "-" + String((i+1)*rpmIncrement));
      Serial.print(": ");

      Serial.println(totalPulse[i]);
    }
    Serial.println("Total     : " + String(sumPulse));
    Serial.println("Under 3000: " + String(pulseUnder3000));
    Serial.end();
  }
  if (RPM > endRPM) {
    //If the engine goes above our max RPM, shut it off and print out the following analytics
    fuelRatio = 0;
    openTime = 0;
    sumPulse = 0;
    int pulseUnder3000 = 0;
    for (int i = 0; i <= 31; i++) {
      sumPulse += totalPulse[i];
      if (i < 16) {
        pulseUnder3000 += totalPulse[i];
      }
      Serial.print(String(i*rpmIncrement) + "-" + String((i+1)*rpmIncrement));
      Serial.print(": ");

      Serial.println(totalPulse[i]);
    }
    Serial.println("Total     : " + String(sumPulse));
    Serial.println("Under 3000: " + String(pulseUnder3000));
    Serial.end();
  }
  if (revolutions >= 25) {
    //Calculate RPM each cycle
    RPM = getRPM(micros() - lastRPMCalculationTime, revolutions);
    startupVal = 1;
    revolutions = 0;
    lastRPMCalculationTime = micros();
    //AFRFeedbackLoop( determineDesiredRPM(minDRPM, maxDRPM), RPM );  //Update fuel ratio when new RPM is calculated
  }
  inject(); //Calculate injection time on each loop cycle
  if (millis()-lastRPMCalculationTime >= 2000) {
    //If RPM falls below 600, sets RPM to 0 meaning shut the engine off
    RPM = 0;
    startupVal = resetVal;
    totalRevolutions = 0;
  }
  if (millis()-lastSerialOutputTime>=500) { //If it's been more than 500ms since we last printed data, print data
    serialOutput();
    lastSerialOutputTime = millis();
  }
}
