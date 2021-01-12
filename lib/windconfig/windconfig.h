
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
        bool isOutputEnabled();
        ConfigBlob configBlob;
        WindSensorConfig * config;
    private:
        char * readLine();
        void docmd(const char * command);
        void dump();
        void save();
        void load();
        void activate();
        void help();
        void setAngles(const char * data);
        void setAngleSize(const char * data);
        void setSpeedTable(const char * data);
        void setSpeedValues(const char * data);
        int loadFloatTable(const char * data, int size, float * table);
        void setAngleMax(const char * data);
        void setAngleOffset(const char * data);
        void setAngleDirection(const char * data);
        int match(const char * command, const char ** commands, int ncommands, const char ** startData);
        void enableOutput(bool enable);


        Stream* io;
        WindSensor* windSensor;
        bool outputOn = false;
        int bufferPos;
        char inputLine[READBUFFER_LEN];

};



#endif