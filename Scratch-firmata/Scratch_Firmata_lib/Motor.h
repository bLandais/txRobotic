/*
   File:   Motor.h
   Author: Mathilde

   Created on 17 octobre 2016, 16:53
*/

#ifndef MOTOR_H
#define MOTOR_H

//#include <ctime>

//#include "Arduino.h"
#include "Button.h"


class Motor {

  private:

    int pinInput1; // Commande moteur vers l'avant
    int pinInput2; // Commande moteur vers l'arrière
    float speed;
    float realSpeed;
    bool direction; //direction is forward? TRUE/FALSE
    int encoderPos;

    // Constants linked to motor
    float b0 = 0.076;
    float b1 = 0.157;
    float a0 = 0.5353;
    float a1 = -1.535;

    float ek[3];
    float power[3]; //power over the present state + the 2 last states

  public:
    Motor(int, int);
    Motor(int, int, float, bool);

    // Getters
    float getSpeed();
    float getRealSpeed();
    bool getDirection();
    int getEncoderPos();
    int getPinInput2();
    int getPinInput1();

    //setter
    void setSpeed(float);
    void setRealSpeed(float);
    void setDirection(bool);
    void setEncoderPos(int);

    //utilitary
    bool init();
    void move(int direction, int speed);
    void stop();
    float percentPower();
    void addPowerValue(float);
    void addEkValue(float);

};

#endif /* MOTOR_H */
