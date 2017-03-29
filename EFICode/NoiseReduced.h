// Averages the last x values fed to NoiseReduced, default x=10, to
// reduce momentary noise in data streams. Optional length parameter.

// void addData(double input);
// Takes parameter input and calculates new internal average of past
// len input values.

// double getData();
// Takes no parameters and returns average of past len input values.

#ifndef NOISEREDUCE_H
#define NOISEREDUCE_H

struct NoiseReduced {
  public:
    NoiseReduce();
    NoiseReduce(int sampleSize);
    void addData(double input);
    double getData();
    
  private:
    int len;
    int filled;
    int head;
    double *data;
    double avg;
}

#endif
