/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Sensor.cpp
 * Author: Mathilde
 * 
 * Created on 11 octobre 2016, 14:05
 */

#include "Arduino.h"
#include "Sensor.h"
#include "SensorType.h"

Sensor::Sensor(){
    setType(SensorType::undefined);
}

Sensor::Sensor(int newDigitalPin, int newAnalogPin, int newCap, SensorType type, int pos){
    this->digitalPin = newDigitalPin;
    this->analogPin = newAnalogPin;
    this->cap = newCap;
    this->position = pos;
    setType(type);
}

void Sensor::read(){
  int analog;
    // print out the value you read:
  digitalWrite(this->digitalPin, HIGH);
  analog = analogRead(analogPin);
  Serial.print("capteur ");
//    Serial.print(i);
    Serial.print(" = ");
    Serial.println(analog);
  digitalWrite(this->digitalPin, LOW);

  if(analog>=this->cap){
    this->value = true ;
  }else{
    this->value = false ;
  }
  
}

void Sensor::setType(SensorType newType){
      type = newType;
    }

void Sensor::setCap(int newCap){
    this->cap = newCap;
}

/*  /!\should not exist/!\ --> you don't want to set a value for your sensor
 * 
 * void Sensor::setValue(float newValue){
  value = newValue;
}
 */


SensorType Sensor::getType(){
  return this->type;
}

int Sensor::getCap(){
  return this->cap;
}

bool Sensor::getValue(){
  return this->value;
}

void Sensor::setPosition(int pos)
{
  this->position = pos;
}

int Sensor::getPosition()
{
  return this->position;
}






