
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




class WindSensor {
    public:
      WindSensor(Stream * _debug = &Serial);
      void windAngleCalibration(int16_t maxAngle, 
         int16_t angleCorrecton, 
         int16_t signCorrection, 
         int16_t nlinearisations, 
         const int16_t *linearisation);
      void windSpeedCalibration(int16_t _windSpeedTableSize, 
          double * _windSpeedTable,
          double * _transformedSpeed);
      bool processData(char *inputLine);
      double getWindSpeed();
      double getWindAngle();
      double updateWindAngle(int angle);
      double updateWindSpeed(double windSpeed);

      int getErrors();
    private:
        int csvParse(char * inputLine, uint16_t len, char * elements[]);
        double windSpeedMS;
        double windAngleRad;
        int errors;
        Stream * debugStream;
        /**
         * maximum angle reading, assuming that the minimum is 0.
         */ 
        int16_t maxAngle;
        /**
         * Number of linearisations.
         */ 
        int16_t nlinearisations;
        /**
         * histeresis curve containing offsets to linearise the angular reading, 
         * must be nlinearisations + 1 long and each reading distributed evenly round 360 degrees.
         * raw angle units
         */ 
        const int16_t *linearisation; 
        /**
         * rotational angle correction added to the linerarised reading to rotate it to the boat, raw angle units.
         */ 
        int16_t angleCorrection;
        /**
         *  correct any rotational direction, should be -1 or 1.
         */ 
        int16_t signCorrection;  
        /**
         * Number of entries in the wind speed calibration table.
         */
        int16_t windSpeedTableSize;
        /**
         * Wind speed caliration table, in Hz ordered ascending and indicating the Hz at which the same index in the other
         * correction arrays represents.
         */
        double * windSpeedTable;

        /**
         * The corresponding transformed speeds in m/s
         * as a minimum this requies 2 elements 0, and the first measuring point.
         */ 
        double * transformedSpeed;


};



#endif