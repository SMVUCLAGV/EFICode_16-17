#include "Runner.h"

#include "Arduino.h"
#include "Constants.h"

void Controller::printCurrentData() {
  //Prints data on each of our sensors to Serial output
  float dataValues [7];
  for (int x = 0; x < 7; x++) {
    float currentVal;
    switch (printOrder[x]) {
      case 0:
        //TODO: Take time in microseconds then convert it to seconds with 4 decimal points of precision
        currentVal = (float) (millis());
        break;
      case 1:
        currentVal = ECT;
        break;
      case 2:
        currentVal = IAT;
        break;
      case 3:
        currentVal = MAP;
        break;
      case 4:
        currentVal = TPS;
        break;
      case 5:
        currentVal = OIN;
        break;
      case 6:
        currentVal = (float) (RPM);
        break;
      default:
        break;
    }
    dataValues[x] = currentVal;
  }
  Serial.write(dataValues, 7);
}

void Controller::printEndingData() {
  unsigned long sumPulse = 0;
  int pulseUnder3000 = 0;
  for (int i = 0; i <= 31; i++) {
    sumPulse += totalPulse[i];
    if (i < 16) {
      pulseUnder3000 += totalPulse[i];
    }

    //TODO: Do this with string interpolation
    Serial.print(String(i*rpmIncrement) + "-" + String((i+1)*rpmIncrement));
    Serial.print(": ");
    Serial.println(totalPulse[i]);
  }
  Serial.println("Total     : " + String(sumPulse));
  Serial.println("Under 3000: " + String(pulseUnder3000));
  Serial.end();
}
