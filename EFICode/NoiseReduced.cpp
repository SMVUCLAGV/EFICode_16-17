#include "NoiseReduced.h"
#include <stdlib.h>
// Constructors
NoiseReduced::NoiseReduced() {
  len = 10;
  filled = 0;
  data = (double *)malloc(sizeof(double)*10);
  avg = 0;
  head = 0;
}

NoiseReduced::NoiseReduced(int sampleSize) {
  if (sampleSize < 1) {
    len = 1;
  } else if (sampleSize > 512) {
    len = 512;
  } else {
    len = sampleSize;
  }
  filled = 0;
  data = (double *)malloc(sizeof(double)*sampleSize);
  for (int i = 0; i < sampleSize; i++)
  {
      data[i] = 0;
  }
  avg = 0;
  head = 0;
}

int NoiseReduced::addData(double input) {
  if (data == NULL) {
    return -1;
  }
  avg = avg + ((input - data[head])/len);
  data[head] = input;
  head++;
  if (head == len) {
    head = 0;
  }
  return 0;
}

double NoiseReduced::getData() {
  return avg;
}
