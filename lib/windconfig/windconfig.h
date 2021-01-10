
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

union ConfigBlob {
    char blob[sizeof(WindSensorConfig)];    
    WindSensorConfig config;
};


class WindConfig {
    public:
        WindConfig(WindSensor * windSensor, Stream * _io = &Serial);
        void process();
        ConfigBlob configBlob;
        WindSensorConfig * config;
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
        int loadFloatTable(const char * data, int size, float * table);
        void setAngleMax(const char * command);
        void setAngleOffset(const char * command);
        void setAngleDirection(const char * command);

        Stream* io;
        WindSensor* windSensor;
        int bufferPos;
        char inputLine[READBUFFER_LEN];

};



#endif