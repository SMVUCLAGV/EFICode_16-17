#ifndef RUNNER_H
#define RUNNER_H

struct Controller {
    Controller();
    bool readSensors();
    void printCurrentData();
    void printEndingData();
    bool getCommand();
    void countRevolution();
    void pulseOff();
    void calculatePulseTime();

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
    bool setDesiredO2(int dO2);
    bool setFuelRatioTable(int index, double ratio); //Set the fuel ratio at the appropriate index in the fuel ratio table
    bool setStartupValue(double val);

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

    double fuelRatioTable [32] =
   //                  0000-0249 0250-0499 0500-0749 0750-0999
                    {    14.9  ,   14.9  ,   14.9  ,   14.9,  //0000 - 0999
                         14.9  ,   14.9  ,   14.9  ,   14.9,  //1000 - 1999
                         14.9  ,   14.9  ,   14.9  ,   14.9,  //2000 - 2999
                         15.4  ,   15.4  ,   15.4  ,   15.4,  //3000 - 3999
                         15.4  ,   15.4  ,   15.4  ,   15.4,  //4000 - 4999
                         14.7  ,   14.7  ,   14.7  ,   14.7,  //5000 - 5999
                         14.7  ,   14.7  ,   14.7  ,   14.7,  //6000 - 6999
                         14.7  ,   14.7  ,   14.7  ,   14.7 };//7000 - 7999

    long lastRPMCalcTime;
    long injectorPulseTime;
    long realPulseTime;
    int delayCount;

    extern long lastSerialOutputTime;

    int printOrder [7];
};

#endif
