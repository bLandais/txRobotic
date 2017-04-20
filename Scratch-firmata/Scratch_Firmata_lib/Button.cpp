/*
   File:   Button.cpp
   Author: Mathilde

   Created on 17 octobre 2016, 17:45
*/

#include <StandardCplusplus.h>
#include <vector>

#include "Arduino.h"
#include "Button.h"


Button::Button() {
}

Button::Button(int newPin) {
  this->pin = newPin;
  pinMode(this->pin, INPUT);
}

int Button::getValue() {
  return this->value;
}

void Button::readValue() {
  this->value = digitalRead(this->pin);
}

