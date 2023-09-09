
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

#define CMD_HELP 0
#define CMD_ANGLE_MAX_S 1
#define CMD_ANGLE_MAX 2
#define CMD_ANGLE_CORRECTION_S 3
#define CMD_ANGLE_CORRECTION 4
#define CMD_ANGLE_DIR_S 5
#define CMD_ANGLE_DIR 6
#define CMD_ANGLE_SIZE_S 7
#define CMD_ANGLE_SIZE 8
#define CMD_ANGLES_S 9
#define CMD_ANGLES 10
#define CMD_SPEED_TABLE_S 11
#define CMD_SPEED_TABLE 12
#define CMD_SPEED_VALUES_S 13
#define CMD_SPEED_VALUES 14
#define CMD_DUMP 15
#define CMD_SAVE 16
#define CMD_LOAD 17
#define CMD_ACTIVATE 18
#define CMD_ENABLE_MON 19
#define CMD_DISABLE_MON 20
#define CMD_STATUS 21
#define CMD_BT_OFF 22
#define NCOMMANDS 23


union ConfigBlob {
    char blob[sizeof(WindSensorConfig)];    
    WindSensorConfig config;
};


class WindConfig {
    public:
        WindConfig(WindSensor * windSensor, Stream * _io = &Serial);
        void begin(void);
        void process(void cb(int8_t));
        bool isMonitoringEnabled();
        ConfigBlob configBlob;
        WindSensorConfig * config;
    private:
        char * readLine();
        int8_t docmd(const char * command);
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
        void enableMonitoring(bool enable);


        Stream* io;
        WindSensor* windSensor;
        bool monitoringOn = false;
        int bufferPos;
        char inputLine[READBUFFER_LEN];

};



#endif