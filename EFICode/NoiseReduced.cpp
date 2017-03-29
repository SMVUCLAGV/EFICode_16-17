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
  avg = 0;
  head = 0;
}

int NoiseReduced::addData(double input) {
  if (data == NULL) {
    return -1;
  }
  if (filled == len) {
    avg = avg + ((input - data[head])/len);
    data[head] = input;
    head++;
  } else {
    data[filled] = input;
    filled++;
    int sum = 0;
    for (int i = 0; i<filled; i++) {
      sum += data[i];
    }
    avg = sum/filled;
    head++;
  }
  if (head == filled) {
    head = 0;
  }
  return 0;
}

double NoiseReduced::getData() {
  return avg;
}
