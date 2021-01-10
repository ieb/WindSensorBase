#include "windconfig.h"
#include <cstdlib>


WindConfig::WindConfig(WindSensor * _windSensor, Stream * _io) {
    io = _io;
    windSensor = _windSensor;
    config = &(configBlob.config);
    memcpy(config, &defaultWindSensorConfig, sizeof(WindSensorConfig));
    load();
}



void WindConfig::help(const char * command) {
    if ( strncmp( command, "help", 4) == 0 ) {
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
        io->println("save                      - Saves the current configuration and makes it active.");
        io->print("$>");
    }
}
void WindConfig::dump(const char * command) { 
    if ( strncmp( command, "dump", 4) == 0 ) {
        char buffer[80];
        if ( strlen(command) == 4 ) {
            sprintf(buffer,"Angle Max %d", config->maxAngle);
            io->println(buffer);
            sprintf(buffer,"Angle Correction %d", config->angleCorrection);
            io->println(buffer);
            sprintf(buffer,"Angle Direction %s", config->signCorrection==1?"normal":"reversed");
            io->println(buffer);
        }
        if ( strlen(command) == 4 || strncmp( command, "dump angles", 11) == 0 ) {
            io->println("Angle Linerisation: angle, reading, offset");
            for (int i = 0; i < config->angleTableSize; i++) {
                sprintf(buffer," %f,%d,%d", 360.0*(double)i/(double)config->angleTableSize, 4096*i/config->angleTableSize, config->angleTable[i]);
                io->println(buffer);
            }
        }
        if ( strlen(command) == 4 || strncmp( command, "dump speed", 10) == 0 ) {
            io->println("Speed Table: Pulse Hz, m/s");
            for (int i = 0; i < config->speedTableSize; i++) {
                sprintf(buffer," %f,%f", config->speedTable[i], config->speed[i]);
                io->println(buffer);
            }
        }
        io->print("$>");
    }
}

void WindConfig::setAngleMax(const char * command) {
    const char * data = NULL;
    if ( strncmp(command, "angle max ", 10) == 0 ) {
        data = &command[10];
    } else if ( strncmp(command, "am ", 3) == 0 ) {
        data = &command[3];
    } 
    if ( data != NULL ) {
        config->maxAngle = atoi(data);
        io->print("Angle Sensor raw readings now 0-");
        io->println(config->maxAngle);
    }
}

void WindConfig::setAngleOffset(const char * command) {
    const char * data = NULL;
    if ( strncmp(command, "angle correction ", 17) == 0 ) {
        data = &command[17];
    } else if ( strncmp(command, "ac ", 3) == 0 ) {
        data = &command[3];
    } 
    if ( data != NULL ) {
        config->angleCorrection = atoi(data);
        io->print("Angle Sensor raw readings correction now ");
        io->println(config->angleCorrection);
    }
}

void WindConfig::setAngleDirection(const char * command) {
    const char * data = NULL;
    if ( strncmp(command, "angle dir ", 10) == 0 ) {
        data = &command[10];
    } else if ( strncmp(command, "ad ", 3) == 0 ) {
        data = &command[3];
    } 
    if ( data != NULL ) {
        config->signCorrection = atoi(data);
        if (config->signCorrection > 0) {
            config->signCorrection = 1;
            io->println("Sensor direction now normal");            
        } else {
            config->signCorrection = -1;
            io->println("Sensor direction now reversed");            
        }
    }
}

void WindConfig::setAngleSize(const char * command) {
    const char * data = NULL;
    if ( strncmp(command, "angle size ", 11) == 0 ) {
        data = &command[11];
    } else if ( strncmp(command, "as ", 3) == 0 ) {
        data = &command[3];
    } 
    if ( data != NULL ) {
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
    io->println("Done set angle table");

}

void WindConfig::setAngles(const char * command) {
    const char * data = NULL;
    if ( strncmp(command, "angles ", 6) == 0 ) {
        data = &command[6];
    } else if ( strncmp(command, "a ", 2) == 0 ) {
        data = &command[2];
    } 
    if ( data != NULL ) {
        int cell = -1, offset = 0;
        const char * pstart = data;
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
    
}

void WindConfig::setSpeedTable(const char * command) {
    const char * data = NULL;
    if ( strncmp(command, "speed table ", 12) == 0 ) {
        data = &command[12];
    } else if ( strncmp(command, "st ", 3) == 0 ) {
        data = &command[3];
    } 
    if ( data != NULL ) {
        config->speedTableSize = loadFloatTable(data, MAX_SPEEDTABLE, config->speedTable);
        io->println("Updated Speed Table");
    }
}

void WindConfig::setSpeedValues(const char * command) {
    const char * data = NULL;
    if ( strncmp(command, "speed values ", 13) == 0 ) {
        data = &command[13];
    } else if ( strncmp(command, "sv ", 3) == 0 ) {
        data = &command[3];
    } 
    if ( data != NULL ) {
        loadFloatTable(data, config->speedTableSize, config->speed);
        io->println("Updated Speed Values");
    }

}

void WindConfig::save(const char * command) {
    if ( strncmp(command, "save", 4) == 0 ) {
        io->println("Do save to flash");
        windSensor->calibrate(config);
    }
}

void WindConfig::load() {
    // load from flash
    io->println("Do load");
    windSensor->calibrate(config);
}


int WindConfig::loadFloatTable(const char * data, int size, float * table) {
    const char * pstart = data;
    char * pend = NULL;
    int n = 0;
    while(n < size && pstart != NULL && *pstart != '\0') {
        pend = NULL;
        float d = strtof(pstart, &(pend));
        if ( *pend == ',') {
            pend++;
        }
        if ( pend != NULL ) {
            table[n] = d;
            n++;
        }
        pstart = pend; 
    }
    return n;
}

void WindConfig::docmd(const char * command) {
        help(command);
        dump(command);
        save(command);
        setAngleSize(command);
        setAngles(command);
        setSpeedTable(command);
        setSpeedValues(command);
        setAngleMax(command);
        setAngleOffset(command);
        setAngleDirection(command);
}


void WindConfig::process() {
    char * command = readLine();
    if ( command != NULL ) {
        unitout("Processing"); unitout_ln(inputLine);
        docmd(command);
    }
}

char * WindConfig::readLine() {
  // Read lines from serial into buffer for processing.
  // 
  while(io->available() > 0) {
    char b = io->read();

    if ( b == '\n') {
      inputLine[bufferPos] = '\0';
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

