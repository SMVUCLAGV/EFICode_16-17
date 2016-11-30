#include "Data.h"
#include "Sensors.h"
#include "parameters.h"

void printCurrentData() {
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

void printEndingData() {
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
