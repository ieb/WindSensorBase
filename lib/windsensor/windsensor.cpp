
#include "windsensor.h"


WindSensorConfig defaultWindSensorConfig = {
    .maxAngle = 4096, 
    .angleCorrection = 0, 
    .signCorrection = 1, 
    .angleTableSize = 36, 
    .speedTableSize = 2, 
    .angleTable  = 0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,
    .speedTable = 0.0,5.5,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
    .speed = 0.0,0.51444444444,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0
};



WindSensor::WindSensor(Stream * _debug) {
    errors = 0;
    debugStream = _debug;
    calibrate(&defaultWindSensorConfig);
}

int WindSensor::csvParse(char * inputLine, uint16_t len, char * elements[]) {
  elements[0] = inputLine;
  int n = 1;
  for ( int i = 0; i < len-1 && n < 11; i++ ) {
    if ( inputLine[i] == ',') {
      inputLine[i] ='\0';
      elements[n] = &(inputLine[i+1]);
      n++;
    }
  }
  return n;
}

bool WindSensor::processData(char *inputLine) {
  if ( inputLine == NULL ) {
      return false;
  }
  uint16_t len = strlen(inputLine);
  if ( len == 0 ) {
    // ignore 
    return false;
  }
  debugStream->println(len);
  inputLine[len] = '\0';
  debugStream->println(inputLine);
  unitout_ln(inputLine);
  // slice into seperate strings by setting the ' to a \0 and storing the start of 
  // each string in a array
  char * elements[11];
  int n = csvParse(inputLine,len, elements);
  if ( n == 10 ) {
      // debug format
      //int detected = atoi(elements[0].c_str());
      int magnet = atoi(elements[1]);
      int agc = atoi(elements[2]);
      int magnitude = atoi(elements[3]);
      int angle = atoi(elements[4]);
      long period = atol(elements[5]);
      float windSpeed = atof(elements[6]);
      int delayms = atoi(elements[7]);
      int edgescount = atoi(elements[8]);
      int calltime = atoi(elements[9]);
      char buffer[128];
      sprintf(buffer, "%ld,%d,%d,%d,%d,%ld,%f,%d,%d,%d,%d", 
              millis(),magnet,agc,magnitude,angle,
              period,
              windSpeed,
              delayms,edgescount,calltime, errors);
      unitout_ln(buffer);
      debugStream->println(buffer);
      updateWindAngle(angle);
      updateWindSpeed(windSpeed);
      return true;
    } else if ( n == 3 ) {
      // compressed format
      int magnet = atoi(elements[0]);
      int angle = atoi(elements[1]);
      float windSpeed = atof(elements[2]);
      unitout("Magnet "); unitout(magnet);unitout(" Angle ");unitout(angle);unitout(" Windspeed ");unitout_ln(windSpeed);
      char buffer[128];
      sprintf(buffer, "%ld,%d,%d,%f,%d\r\n",
              millis(),magnet,angle,
              windSpeed,
              errors);
      unitout_ln(buffer);
      debugStream->println(buffer);
      updateWindAngle(angle);
      updateWindSpeed(windSpeed);
      return true;
    } else {
      errors++;
      char buffer[128];
      sprintf(buffer, "ERROR Rejected (fields) %d %s \r\n", n, inputLine);
      unitout(buffer);
      debugStream->println(buffer);
      return false;
    }
  }

void WindSensor::calibrate(WindSensorConfig * _config) {
    config = _config;
}


float WindSensor::updateWindAngle(int angle) {
    // the raw angle goes from 0 to 4096
    // there will be histeresis and offset. 
    // perform linear interpolation with a lookup table to adjust the angle for histersis
    // then apply an offset from a fixed lookup of offsets before converting to radians.
    angle = angle % config->maxAngle;
    int16_t offset = 0;
    if ( config->angleTableSize > 0) {
        // which bucket ot use
        int16_t ioffset = (angle*config->angleTableSize)/config->maxAngle;
        // measurement of the bucket
        int16_t langle = ioffset*config->maxAngle/config->angleTableSize;
        // measurement of the bucket above
        int16_t uangle = (ioffset+1)*config->maxAngle/config->angleTableSize;
        // use linear interpolation on the bucket to find the linerisation offset for this raw reading.
        offset = config->angleTable[ioffset] + (config->angleTable[ioffset+1]-config->angleTable[ioffset])*((angle-langle)/(uangle-langle));
    }
    // calculate the offset angle in raw units of rotation in the range 0 to maxAngle (eg 4096)
    int16_t offset_angle = (angle+offset) % config->maxAngle;
    // apply rotational correction, we do this last as this may change with user input, but the linearisation is 
    // likely to be a feature of the sensor misalignment relative to the magnet.
    // this will be in the range 0-4096
    int16_t final_angle = (offset_angle + config->angleCorrection) * config->signCorrection;
    // this should now be in the range -2048 to + 2048 port to starboard with dead ahead 0.
    // covert to raidians, the 2 is there since final_angle/maxangle is effectively 2 pi, one rotation.
    windAngleRad = 2.0*3.14159*final_angle/config->maxAngle;
    return windAngleRad;
}


float WindSensor::updateWindSpeed(float windSpeed) {
    // wind speed is in Pulses per second from the wind sensor.
    // since the speed table is a variable table with user defined boundaries we have to iterate over
    // the windSpeedTable to find the interpolation boundaries, and then use linear interpolation to find
    // the speed. If the speed is greater than the last entry then linear interpolation should use the line
    // between the last 2 cells. Straight line can be achieved with 2 entries, the first being 0 and the second being
    // the speed and a known frequency of pulses.
    // typically 5.5Hz == 1kn.
    for(int i = 1; i < config->speedTableSize; i++) {
        if  (windSpeed < config->speedTable[i] ) {

            float factor = (windSpeed - config->speedTable[i-1])/ (config->speedTable[i]-config->speedTable[i-1]);
            windSpeedMS = config->speed[i-1]+factor*(config->speed[i]-config->speed[i-1]);
            return windSpeedMS;
        }
    }
    float factor = (windSpeed - config->speedTable[config->speedTableSize-2])/ 
                  (config->speedTable[config->speedTableSize-1]-config->speedTable[config->speedTableSize-2]);
    windSpeedMS = config->speed[config->speedTableSize-2]+
         factor*(config->speed[config->speedTableSize-1]-config->speed[config->speedTableSize-2]);
    return windSpeedMS;
}
  
float WindSensor::getWindSpeed() {
    return windSpeedMS;
}
float WindSensor::getWindAngle() {
    return windAngleRad;
}
int WindSensor::getErrors() {
    return errors;
}

