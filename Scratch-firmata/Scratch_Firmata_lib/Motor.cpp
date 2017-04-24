/*
   To change this license header, choose License Headers in Project Properties.
   To change this template file, choose Tools | Templates
   and open the template in the editor.
*/

/*
   File:   Motor.cpp
   Author: Mathilde

   Created on 17 octobre 2016, 16:53
*/

#include <Adafruit_NeoPixel.h>

//#include "Arduino.h"
#include "Motor.h"

//constructors
Motor::Motor(int newPinInput1, int newPinInput2) {
  this->pinInput1 = newPinInput1;
  this->pinInput2 = newPinInput2;
  Motor::setSpeed(0);
  Motor::setDirection(true);
}

Motor::Motor(int newPinInput1, int newPinInput2, float speed = 0, bool direction = true) {
  this->pinInput1 = newPinInput1;
  this->pinInput2 = newPinInput2;
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

int Motor::getPinInput1() {
  return this->pinInput1;
}

int Motor::getPinInput2() {
  return this->pinInput2;
}

//setters

void Motor::setSpeed(float newSpeed) {
  //settings max and min limits
  if (newSpeed > 255) {
    newSpeed = 255;
  }
  else if (newSpeed < 0) {
    newSpeed = -newSpeed;
  }

  //register speed value
  this->speed = newSpeed;
  if(this->direction) {
    analogWrite(this->pinInput1, newSpeed);
  }
  else {
    analogWrite(this->pinInput2, newSpeed);
  }
}

void Motor::setRealSpeed(float newRealSpeed)
{
  realSpeed = newRealSpeed;
}

void Motor::setDirection(bool newDirection) {
  this->direction = newDirection;
}

void Motor::setEncoderPos(int newEncoderPos)
{
  encoderPos = newEncoderPos;
}

//utilitary

bool Motor::init() {
  // initialize serial communication at 9600 bits per second:
  //Serial.begin(9600);
  pinMode(this->pinInput1, OUTPUT);
  pinMode(this->pinInput2, OUTPUT);
}

void Motor::move(int direction, int speed) {
  if (direction == 0) setDirection(true);
  else if (direction == 1) setDirection(false);
  setSpeed(floor(2.55 * speed));
}

void Motor::stop() {
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
