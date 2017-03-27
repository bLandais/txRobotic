/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Motor.h
 * Author: Mathilde
 *
 * Created on 17 octobre 2016, 16:53
 */


#ifndef MOTOR_H
#define MOTOR_H

//#include <ctime>

#include "Arduino.h"
#include "Button.h"


class Motor {
    
private:

      int pinInput1; // Commande de sens moteur, Input 1
      int pinInput2; // Commande de sens moteur, Input 2  
      int en;  // Commande de vitesse moteur, Output Enabled1
      float speed;
      bool direction; //direction is forward? TRUE/FALSE
      

public:
    
    Motor(int, int, int);
    Motor(int, int, int, float, bool);

    //getter
    float getSpeed();
    bool getDirection();
    
    //setter
    void setSpeed(float);
    void setDirection(bool);

	//utilitary 
	bool init();
	void move(int direction, int speed);
	void stop();

};

#endif /* MOTOR_H */

