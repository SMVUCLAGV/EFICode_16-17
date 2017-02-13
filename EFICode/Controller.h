#ifndef CONTROLLER_H
#define CONTROLLER_H

struct Controller {
    Controller();
    bool readSensors();
    void sendCurrentData();
    void printEndingData();
    bool getCommand();
    void countRevolution();
    void pulseOff();
    void calculatePulseTime();
    void lookupPulseTime();
    void calculatePulseTime(bool singleVal, int row, int col);

    int getRPM (int timePassed, int rev);
    double getTPS();
    double getTemp(int pin);
    double getMAP();
    double getOIN();

    void initializeParameters();
    bool isLean (int pin);
    bool setIdleVal(double val);
    bool setFuelRatio(double val);
    bool setDesiredRPM(int dRPM);
    bool setDesiredO2(double dO2);
    bool setFuelRatioTable(int index, double ratio); //Set the fuel ratio at the appropriate index in the fuel ratio table
    bool setStartupValue(double val);
    bool setResetRatio(double val);

    int RPM;
    double TPS;
    double ECT;
    double IAT;
    double MAP;
    double OIN;

    int desiredRPM;
    double desiredOIN;

    int revolutions;
    unsigned long totalRevolutions;

    double fuelRatio; //unitless
    double idleVal;
    double resetVal;
    double startupVal;
    unsigned long totalPulse[32];
    long totalPulseTime;

    long lastRPMCalcTime;
    long injectorPulseTime;
    long realPulseTime;
    int delayCount;

    long lastSerialOutputTime;
    long lastInterrupt;
    int openTime;

    bool currentlySendingData;

    //WARNING: From here down are fuel ratio parameters. Don't change these unless you know what you are doing!

    double fuelRatioTable [10][32] =
    {{14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7},
     {14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7},
     {14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7},
     {14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7},
     {14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7},
     {14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7},
     {14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7},
     {14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7},
     {14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7},
     {14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 14.9, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 15.4, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7, 14.7}};

     long injectorPulseTimes[10][32];
};

#endif
