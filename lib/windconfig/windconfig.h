
#ifndef WIND_CONFIG_H
#define WIND_CONFIG_H

#include <Arduino.h>
#include <windsensor.h>

#ifdef UNIT_TEST
#include <iostream> 
#define unitout_ln(x) std::cout << x << std::endl
#define unitout(x) std::cout << x 
#else
#define unitout_ln(x)
#define unitout(x) 
#endif

#define READBUFFER_LEN 1024
#define MAX_ANGLETABLE 72
#define MAX_SPEEDTABLE 10




class WindConfig {
    public:
        WindConfig(WindSensor * windSensor, Stream * _io = &Serial);
        void process();
        uint16_t maxAngle;
        int16_t angleCorrection;
        int8_t signCorrection;
        int angleTableSize;
        int speedTableSize;
        int16_t angleTable[MAX_ANGLETABLE];
        double speedTable[MAX_SPEEDTABLE];
        double speed[MAX_SPEEDTABLE];
    private:
        char * readLine();
        void docmd(const char * command);
        void dump(const char * command);
        void save(const char * command);
        void load();
        void help(const char * command);
        void setAngles(const char * command);
        void setAngleSize(const char * command);
        void setSpeedTable(const char * command);
        void setSpeedValues(const char * command);
        int loadDoubleTable(const char * data, int size, double * table);
        void setAngleMax(const char * command);
        void setAngleOffset(const char * command);
        void setAngleDirection(const char * command);

        Stream* io;
        WindSensor* windSensor;
        int bufferPos;
        char inputLine[READBUFFER_LEN];

};



#endif