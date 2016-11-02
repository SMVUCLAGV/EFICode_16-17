//Pin 9 Not Working 3/12/16
#include "TimerThree.h"
#include "temperature.h"
#include "TPS.h"
#include "RPM.h"
#include "OIN.h"
#include "MAP.h"
//#include "serial.h"
#include "constants.h"
#include "parameters.h"

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
  Serial.print(revolutions2);
  Serial.print(", ");
  Serial.print(startupVal);
  Serial.print(", ");
  Serial.println(fuelRatioTable[RPM/rpmIncrement]); 
}

void countRevolution () {
  //  Serial.println("Revolution counted");

    revolutions++;
    revolutions2++;
    if (revolutions2 % 2 == 1 || revolutions2 < 25)//&& RPM > desiredRPM)
    {
      digitalWrite(INJ_Pin, HIGH);
      lastTime2 = millis();
      delayCount = 0;
      Timer3.setPeriod(pulseTime);
      Timer3.restart();
    }
    
    

}

void pulseOff()//ISR for Timer3 interrupt
{
  if (delayCount == 1)//Determined experimentally.  I think the Timer interrupts in the very beginning.  To avoid this first interrupt, I switched to 1.  Can probably be revised and improved
  {
    digitalWrite(INJ_Pin, LOW);
    pulseTimeReal= (millis() - lastTime2);
    arrayIndex = RPM / rpmIncrement;
    totalPulse[arrayIndex] += pulseTimeReal;
//    totalPulse[arrayIndex] += pulseTime / 1000;

  }
  if (delayCount > 100)
    delayCount = 2;
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
  if (revolutions2 > 2 && lastTPS <= maxIdleTPS)
  {
    if (RPM > desiredRPM)
      {
        idleVal = idleVal - .00001;
      }
    else if (RPM < desiredRPM)
      {
        idleVal = idleVal + .00001;
      }
  
//  
//      O2V = getOIN();
  //    if (O2V > desiredO2)
  //    {
  //      idleVal = idleVal - .000002;
  //    }
  //    else if (O2V < desiredO2)
  //    {
  //      idleVal = idleVal + .000002;
  //    }
  }
  //double val = getMAP()*injectionConstant/(getTemp(IAT_Pin)*fuelRatioTable[RPM/rpmIncrement]*injectorFuelRate);
  double val = getMAP()*injectionConstant/(getTemp(IAT_Pin)*fuelRatio*injectorFuelRate);
  if (lastTPS <= maxIdleTPS)
  {
    pulseTime =(val * 1000000 * idleVal * startupVal * TPSx) + openTime;
  }else
  {
    pulseTime =(val * 1000000 * startupVal * TPSx) + openTime;
  }
}

void setup() {
  pinMode(INJ_Pin, OUTPUT);
  Serial.begin(9600);
  Serial.print("Time, ECT,    IAT,    MAP,     TPS,  OIN,  RPM,  DesPW,PWRe,Revs,Start,AFR");
  Serial.println();
  lastTPS = getTPS();
  attachInterrupt(2, countRevolution, FALLING);
  inject();
  Timer3.initialize(100000);
  Timer3.attachInterrupt(pulseOff, pulseTime);
  delayCount = 4;
  pulseTimeReal = 0;
  lastTime2 = millis();
  lastInterrupt = millis();
  for (int i = 0; i <= 31; i++)
    totalPulse[i] = 0;
}

int determineDesiredRPM(const int& minRPM,const int& maxRPM)
{
  double TPS = getTPS();
  int desiredRPM = minRPM + (maxRPM - minRPM) * TPS;
  return desiredRPM;
}
//Feedback loop for getting the controller to figure out how to increase or decrease RPM on its own.
void AFRFeedbackLoop(const int& DRPM, const int& CRPM)
{
  int RPMGradient = abs(DRPM - CRPM); //desired rpm - current rpm -> distance from desired rpm
  if (lastOperation == "+")
  {
    if (RPMGradient < LastRPMGradient)  //if adding to the fuel ratio decreses our distance from the desired rpm, then keep adding to our fuel ratio.
    {
      fuelRatio += adjustAFR * RPMGradient; //the rate at which the fuel ratio (AFR) adjusts is proportional to the distance from the desired rpm.
      nAdditions++; //count the number of times the controller decides to increase the air-fuel ratio. For providing insight on how the fuel mixture varies with speed.
    }
    else if (RPMGradient > LastRPMGradient) //if adding more fuel increases our distance from the desired rpm, then try subtracting from our fuel ratio to get closer to the desired fuel ratio.
    {
      fuelRatio -= adjustAFR * RPMGradient;
      nSubtractions++;  //Count the number of subtractions from the fuel ratio. Provides insight on how the fuel mixture varies with speed.
      lastOperation == "-"; //The last operation used in adjusting the fuel ratio is now subtraction, so update the parameter to reflect that.
    }
  }
  else if (lastOperation == "-")  //The reverse of the lastOperation == "+" logic if the last operation on the fuel ratio was subtraction
  {
    if (RPMGradient < LastRPMGradient)
    {
      fuelRatio -= adjustAFR * RPMGradient;
      nSubtractions++;
    }
    else if (RPMGradient > LastRPMGradient)
    {
      fuelRatio += adjustAFR * RPMGradient;
      nAdditions++;
      lastOperation = "+";
    }
  }
  LastRPMGradient = RPMGradient;  //update the previous distance from desired rpm
}

void loop()
{
  if (Serial.available())
  {
    sumPulse = 0;
    int pulseUnder3000 = 0;
    for (int i = 0; i <= 31; i++)
    {
      sumPulse += totalPulse[i];
      if (i < 16)
      {
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
  if (RPM >endRPM)
  {
    fuelRatio = 0;
    openTime = 0;
    sumPulse = 0;
    int pulseUnder3000 = 0;
    for (int i = 0; i <= 31; i++)
    {
      sumPulse += totalPulse[i];
      if (i < 16)
      {
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
    RPM = getRPM(micros() - lastTimeRPM, revolutions);
    startupVal = 1; 
    revolutions = 0;
    lastTimeRPM = micros();
    //AFRFeedbackLoop( determineDesiredRPM(minDRPM, maxDRPM), RPM );  //Update fuel ratio when new RPM is calculated
  }
  inject();
  //Timer3.setPeriod(pulseTime);
  if (millis()-lastTimeRPM >= 2000) //If RPM falls below 600, sets RPM to 0.
  {
    RPM = 0;
    startupVal = resetVal; 
    revolutions2 = 0;
  }
  
  if (millis()-lastTime>=500) {
    serialOutput();
    lastTime = millis();
  }
}
