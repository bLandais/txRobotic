
/*
   File:   Sensor.h
   Author: Mathilde

   Created on 11 octobre 2016, 14:05
*/

#ifndef SENSOR_H
#define SENSOR_H


#include "SensorType.h"

class Sensor {

  private:
    SensorType type;
    int digitalPin;
    int analogPin;
    int cap;
    int position;
    bool value;


  public:
    Sensor();
    Sensor(int, int, int, SensorType, int);

    //getter
    bool getValue();
    SensorType getType();
    int getPosition();
    int getCap();

    //setter
    void setType(SensorType type);
    void setPosition(int pos );
    void setCap(int);

    //utilitaire
    void read();

};

#endif /* SENSOR_H */

