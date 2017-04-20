/*
   To change this license header, choose License Headers in Project Properties.
   To change this template file, choose Tools | Templates
   and open the template in the editor.
*/

/*
   File:   Button.h
   Author: Mathilde

   Created on 17 octobre 2016, 17:45
*/



#ifndef BUTTON_H
#define BUTTON_H

class Button {

  public:
    int pin;
    int value;

  public:
    Button();
    Button(int);
    int getValue();
    void readValue();

};

#endif /* BUTTON_H */

