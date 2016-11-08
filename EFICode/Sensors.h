#ifndef SENSORS_H
#define SENSORS_H

int getRPM (int timePassed, int rev);
double getTPS();
double getTemp(int pin);
double getMAP();
double getOIN ();
bool isLean (int pin);

#endif
