
#ifndef WINDSENSOR_H
#define WINDSENSOR_H

#include <Arduino.h>

#ifdef UNIT_TEST
#include <iostream> 
#define unitout_ln(x) std::cout << x << std::endl
#define unitout(x) std::cout << x 
#else
#define unitout_ln(x)
#define unitout(x) 
#endif

#define MAX_ANGLETABLE 72
#define MAX_SPEEDTABLE 10

struct WindSensorConfig { // 231 bytes, must fit into flash
    uint16_t maxAngle;  // 2
    int16_t angleCorrection; // 2
    int8_t signCorrection; // 1
    int8_t angleTableSize; // 1
    int8_t speedTableSize; // 1
    int16_t angleTable[MAX_ANGLETABLE]; // 72*2 
    float speedTable[MAX_SPEEDTABLE]; // 10*4
    float speed[MAX_SPEEDTABLE]; // 10*4
};

extern WindSensorConfig defaultWindSensorConfig;


class WindSensor {
    public:
      WindSensor(Stream * _debug = &Serial);
      void calibrate(WindSensorConfig * config);
      bool processData(char *inputLine, bool debug);
      float getWindSpeed();
      float getWindAngle();
      unsigned long getPacketsDropped();
      unsigned long getPacketsRead();
      unsigned long getSumReadPeriod();
      unsigned long getErrors();

      float updateWindAngle(int angle);
      float updateWindSpeed(float windSpeed);

    private:
        int csvParse(char * inputLine, uint16_t len, char * elements[]);
        void calcPackets();
        unsigned long lastReadTime = 0;
        unsigned long packetsDropped = 0;
        unsigned long packetsRead = 0;
        unsigned long sumReadPeriod = 20000; // configuration is 200ms between packets, see WindSensor loop().
        float windSpeedMS;
        float windAngleRad;
        unsigned long errors = 0;
        bool debugOutput = false;
        WindSensorConfig *config;
        Stream * debugStream;

};



#endif