#include "windconfig.h"
#include <configstorage.h>
#include <cstdlib>

#define STORAGE_NAMESPACE "WindSensor"
#define STORAGE_KEY "cf10" // config version 10, keynames need to be short

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
#define CMD_ENABLE_OUTPUT 19
#define CMD_DISABLE_OUTPUT 20
#define NCOMMANDS 21

static const char * windconfig_commands[] = {
    "help",
    "am ",
    "angle max ",
    "ac ",
    "angle correction ",
    "ad ",
    "angle dir ",
    "as ",
    "angle size ",
    "a ",
    "angles ",
    "st ",
    "speed table ",
    "sv ",
    "speed values ",
    "dump",
    "save",
    "load",
    "activate",
    "on",
    "off"
};


WindConfig::WindConfig(WindSensor * _windSensor, Stream * _io) {
    io = _io;
    windSensor = _windSensor;
    config = &(configBlob.config);
}


void WindConfig::begin() {
    memcpy(config, &defaultWindSensorConfig, sizeof(WindSensorConfig));
    load();
}


void WindConfig::help() {
    io->println("Lists of numbers should be comma seperated with no spaces.");
    io->println("Commands:");
    io->println("help|? - this help");
    io->println("am|angle max <n>          - set maximum angle value, default in 4096");
    io->println("ac|angle correction <n>   - set maximum angle sensor raw offset offset, default 0");
    io->println("ad|angle dir <n>          - set maximum angle sensor rotation direction, 1 = normal, 0 = reversed");
    io->println("as|angle size <n>         - set the number of angle caibrations.");
    io->println("a|angles <n>,<v>,<n1>,<v1> - set the angle calibrations, where n and v are ints. n = slot (0-slot), v = offset (+-)");
    io->println("st|speed table <n>,<n>    - Set the speed table up, where n is the puse frequency in Hz ");
    io->println("sv|speed values <n>,<n>   - Set the speed values up matching the speed table where n is the speed in m/s");
    io->println("dump                      - Dumps the current configuration");
    io->println("save                      - Saves the current configuration to non volatile storage.");
    io->println("load                      - Loads configuration from no volatile storage");
    io->println("actviate                  - Activates Current configuration");
    io->println("on                        - Enables diagnostic output");
    io->println("off                       - Disables diagnostic output");
}

void WindConfig::docmd(const char * command) {
    const char *data;
    int cid = match(command, windconfig_commands, NCOMMANDS, &data );
    switch(cid) {
        case CMD_ENABLE_OUTPUT: 
            enableOutput(true); 
            break;
        case CMD_DISABLE_OUTPUT: 
            enableOutput(false); 
            break;
        case CMD_ACTIVATE: 
            activate(); 
            break;
        case CMD_LOAD: 
            load(); 
            break;
        case CMD_SAVE: 
            save(); 
            break;
        case CMD_DUMP: 
            dump(); 
            break;
        case CMD_HELP: 
            help(); 
            break;
        case CMD_ANGLE_MAX_S:
        case CMD_ANGLE_MAX:  
            setAngleMax(data);
            break;
        case CMD_ANGLE_CORRECTION:
        case CMD_ANGLE_CORRECTION_S:  
            setAngleOffset(data);
            break;
        case CMD_ANGLE_DIR:
        case CMD_ANGLE_DIR_S:  
            setAngleDirection(data);
            break;
        case CMD_ANGLE_SIZE:
        case CMD_ANGLE_SIZE_S:  
            setAngleSize(data);
            break;
        case CMD_ANGLES:
        case CMD_ANGLES_S:  
            setAngles(data);
            break;
        case CMD_SPEED_TABLE:
        case CMD_SPEED_TABLE_S:  
            setSpeedTable(data);
            break;
        case CMD_SPEED_VALUES:
        case CMD_SPEED_VALUES_S:  
            setSpeedValues(data);
            break;
        default:
            io->print(F("Command Not recognised:"));
            io->println(command);
            break;

    }
}

void WindConfig::process() {
    char * command = readLine();
    if ( command != NULL ) {
        unitout("Processing"); unitout_ln(inputLine);
        docmd(command);
        io->print("$>");
    }
}


void WindConfig::dump() { 
    char buffer[80];
    sprintf(buffer,"Angle Max %d", config->maxAngle);
    io->println(buffer);
    sprintf(buffer,"Angle Correction %d", config->angleCorrection);
    io->println(buffer);
    sprintf(buffer,"Angle Direction %s", config->signCorrection==1?"normal":"reversed");
    io->println(buffer);
    io->println("Angle Linerisation Table:");
    io->println("   angle, reading, offset");
    for (int i = 0; i < config->angleTableSize; i++) {
        sprintf(buffer," %7.3f,%8d,%7d", 360.0*(double)i/(double)config->angleTableSize, 4096*i/config->angleTableSize, config->angleTable[i]);
        io->println(buffer);
    }
    io->println("Speed Table:");
    io->println("      Hz,    m/s,     kn");
    for (int i = 0; i < config->speedTableSize; i++) {
        sprintf(buffer," %7.3f,%7.3f,%7.3f", config->speedTable[i], config->speed[i], config->speed[i]*1.94384);
        io->println(buffer);
    }
}

void WindConfig::setAngleMax(const char * data) {
    config->maxAngle = atoi(data);
    io->print("Angle Sensor raw readings now 0-");
    io->println(config->maxAngle);
}

void WindConfig::setAngleOffset(const char * data) {
    config->angleCorrection = atoi(data);
    io->print("Angle Sensor raw readings correction now ");
    io->println(config->angleCorrection);
}

void WindConfig::setAngleDirection(const char * data) {
    config->signCorrection = atoi(data);
    if (config->signCorrection > 0) {
        config->signCorrection = 1;
        io->println("Sensor direction now normal");            
    } else {
        config->signCorrection = -1;
        io->println("Sensor direction now reversed");            
    }
}

void WindConfig::setAngleSize(const char * data) {
    config->angleTableSize = atoi(data);
    if (config->angleTableSize >  MAX_ANGLETABLE) {
        config->angleTableSize = MAX_ANGLETABLE;
    }
    for(int i = 0; i < config->angleTableSize; i++) {
        config->angleTable[i] = 0;
    }
    io->print("Angle Table cleared to   ");
    io->println(config->angleTableSize);
}

void WindConfig::setAngles(const char * data) {
    int cell = -1;
    char * pstart = (char *)data;
    char * pend = NULL;
    while(pstart != NULL && *pstart != '\0') {
        pend = NULL;
        int i = strtol(pstart, &(pend), 10);
        if ( *pend == ',' ) {
            pend++;
        }
        if ( pend != NULL ) {
            if ( cell == -1 ) {
                cell = i;
            } else if ( cell >= 0 && cell < config->angleTableSize) {
                config->angleTable[cell] = i;
                cell = -1;
            } else {
                cell = -1;
            }
        }
        pstart = pend; 
    }
    io->println("Updated Angles  ");    
}

void WindConfig::setSpeedTable(const char * data) {
    config->speedTableSize = loadFloatTable(data, MAX_SPEEDTABLE, config->speedTable);
    io->println("Updated Speed Table");
}

void WindConfig::setSpeedValues(const char * data) {
    loadFloatTable(data, config->speedTableSize, config->speed);
    io->println("Updated Speed Values");
}




void WindConfig::save() {
    io->println("Do save to flash");
    int32_t err = config::writeStorage(STORAGE_NAMESPACE, STORAGE_KEY, configBlob.blob, sizeof(WindSensorConfig));
    if ( err != config::ok ) {
        io->print("Save Failed, err:");
        io->println(err);
    }
}

void WindConfig::activate() {
    windSensor->calibrate(config);
}


void WindConfig::load() {
    // load from flash
    io->println("Do load");
    int32_t err = config::readStorage(STORAGE_NAMESPACE, STORAGE_KEY, configBlob.blob, sizeof(WindSensorConfig));
    if ( err != config::ok ) {
        io->print("Load Failed, err:");
        io->println(err);
    } else {
        io->println("Loaded");
    }
}

void WindConfig::enableOutput(bool enable) {
    outputOn = enable;
}

bool WindConfig::isOutputEnabled() {
    return outputOn;
}



int WindConfig::loadFloatTable(const char * data, int size, float * table) {
    char * pstart = (char *)data;
    char * pend = NULL;
    int n = 0;

    while(n < size && pstart != NULL && *(pstart) != '\0' ) {
        pend = NULL;
        float d = strtof(pstart, &(pend));
        if ( *pend == ',') {
            pend++;
        }
        if ( pend != NULL ) {
           io->printf("%d, %f\n",n,d);
           table[n] = d;
            n++;
        }
        pstart = pend; 
    }
    io->printf("Loaded %d\n",n);
    return n;
}

int WindConfig::match(const char * command, const char ** commands, int ncommands, const char ** startData) {
    
    for( int i = 0; i < ncommands; i++) {
        int len = strlen(commands[i]);
        if ( strncmp(command, commands[i], len) == 0 ) {
            *startData = &command[len];
           return i;
        }
    }
    return -1;
}





char * WindConfig::readLine() {
  // Read lines from serial into buffer for processing.
  // 
  while(io->available() > 0) {
    char b = io->read();

    if ( b == '\n') {
      inputLine[bufferPos] = '\0';
      if (bufferPos > 0 && inputLine[bufferPos-1] == '\r') {
          inputLine[bufferPos-1] = '\0';
      }
      bufferPos = 0;
      unitout("Got line"); unitout_ln(inputLine);
      return inputLine;
    } else if ( bufferPos < READBUFFER_LEN-1 ) {
      inputLine[bufferPos] = b;
      bufferPos++;
    } else {
      io->println("Error, input too long");
      bufferPos = 0;
      return NULL;
    }
  }
  unitout_ln("No more chars, end line not found");
  return NULL;
}

