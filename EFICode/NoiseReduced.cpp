#include "NoiseReduced.h"

// Constructors
NoiseReduced::NoiseReduced() {
  len = 10;
  filled = 0;
  data = (double *)malloc(sizeof(double)*10);
  avg = 0;
  head = 0;
}

NoiseReduced::NoiseReduced(double sampleSize) {
  len = sampleSize;
  filled = 0;
  data = (double *)malloc(sizeof(double)*sampleSize);
  avg = 0;
  head = 0;
}

void NoiseReduced::addData(double input) {
  if (filled == len) {
    avg = avg + ((input - data[head])/len);
    data[head] = input;
    head++;
    if (head == filled) {
      head = 0;
    }
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
}

int NoiseReduced::getData() {
  return avg;
}
