/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Motor.cpp
 * Author: Mathilde
 * 
 * Created on 17 octobre 2016, 16:53
 */

#include <Adafruit_NeoPixel.h>

//#include "Arduino.h"
#include "Motor.h"

//constructors

Motor::Motor(int newPinInput1, int newPinInput2, int newEn) {
    this->pinInput1 = newPinInput1;
    this->pinInput2 = newPinInput2;
    this->en = newEn;
    Motor::setSpeed(0);
    Motor::setDirection(true);
}

Motor::Motor(int newPinInput1, int newPinInput2, int newEn, float speed = 0, bool direction = true) {
    this->pinInput1 = newPinInput1;
    this->pinInput2 = newPinInput2;
    this->en = newEn;  
    Motor::setSpeed(speed);
    Motor::setDirection(direction);
}




//getters

float Motor::getSpeed() {
	return this->speed;
}

float Motor::getRealSpeed() {
  return realSpeed;
}

bool Motor::getDirection() {
	return this->direction;
}

int Motor::getEncoderPos() {
  return encoderPos;
}


//setters

void Motor::setSpeed(float newSpeed){
  //settings max and min limits
     if (newSpeed > 255){
        newSpeed = 255;  
      }
      else if (newSpeed < 0){
        newSpeed = -newSpeed;  
      }

  //register speed value
    this->speed = newSpeed;
    analogWrite( this->en, newSpeed );
}

void Motor::setRealSpeed(float newRealSpeed)
{
  realSpeed = newRealSpeed;
}

void Motor::setDirection(bool newDirection){
    this->direction = newDirection;
    //handle clockwise
    if(this->direction){
      // Clockwise
      digitalWrite( this->pinInput1, LOW );             // Clockwise
      digitalWrite( this->pinInput2, HIGH );
    }
    else{
      digitalWrite( this->pinInput1, HIGH );             // anti-Clockwise
      digitalWrite( this->pinInput2, LOW );
    }
      
}

void Motor::setEncoderPos(int newEncoderPos)
{
  encoderPos = newEncoderPos;
}

//utilitary

bool Motor::init() {
	// initialize serial communication at 9600 bits per second:
	//Serial.begin(9600);

	pinMode(this->en, OUTPUT);   // Sorties commande  moteur 1
	pinMode(this->pinInput1, OUTPUT);
	pinMode(this->pinInput2, OUTPUT);
}

void Motor::move(int direction, int speed){
	if (direction == 0) setDirection(true);
	else if (direction == 1) setDirection(false);
	setSpeed(floor(2.55*speed));
}

void Motor::stop(){
	setSpeed(0);
}

float Motor::percentPower()
{
  return -1 * a1 * power[1] + (-1) * a0 * power[2] + b1 * ek[1] + b0 * ek[2];
}

void Motor::addPowerValue(float newPower)
{
  power[2] = power[1];
  power[1] = power[0];
  power[0] = newPower;
}

void Motor::addEkValue(float newEk)
{
  ek[2] = ek[1];
  ek[1] = ek[0];
  ek[0] = newEk;
}
