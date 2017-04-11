/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Led.cpp
 * Author: Mathilde
 * 
 * Created on 25 octobre 2016, 16:29
*/

#include "Led.h"


Led::Led(int newPin, int newNumber) {
    this->pin = newPin;
    this->number = newNumber;
    this->state = false;
    this->blink = false;
    this->color = {0,0,0};
}

bool Led::init(){

    this->strip = Adafruit_NeoPixel(this->number, this->pin, NEO_GRB + NEO_KHZ800);
    this->strip.begin();// Initialise toute les led à 'off'
    this->strip.show(); //display   
}

std::vector<int> Led::getColor(){
   return this->color;
}

void Led::show(int ledNbr, int newR, int newV, int newB) {
	this->color = { newR, newV, newB };
	this->strip.setPixelColor(ledNbr,newR, newV, newB);
	this->strip.show();
}

void Led::setColorAll(int newR, int newV, int newB){
    this->color = {newR, newV, newB};

    int cpt;
    for(cpt = 0; cpt < this->number; cpt++){
      this->strip.setPixelColor(cpt,newR, newV, newB);
    }
    if(this->pin == 44) {
      this->strip.setPixelColor(0,0,0,0);
    }
    this->strip.show();
    delay(100);
}

void Led::setColorUnit(int ledNbr, int newR, int newV, int newB) {
	ledNbr = abs(ledNbr);
	if (ledNbr > this->number) show(this->number,0,0,0);
	else show(ledNbr-1, newR, newV, newB);
}

/* Code de Quentin */
void Led::setColor(int couleur) {
  /*this->r = this->tabColor[couleur][0];
  this->v = this->tabColor[couleur][1];
  this->b = this->tabColor[couleur][2];*/
  switch (couleur) {
    case 0:setColorAll(10, 10, 10); //blanc
      break;
    case 1:setColorAll(100,0,0); //rouge
      break;
    case 2:setColorAll(0,100,0); //bleu
      break;
    case 3:setColorAll(0,0,100); //vert   
      break;
    case 4:setColorAll(100,255,255); //turquoise
      break;
    case 5:setColorAll(255,0,50); //orange
      break;
    case 6:setColorAll(160,160,160); //gris
      break;
    case 7:setColorAll(153,0,153); //jaune
      break;
    case 8:setColorAll(255,255,51); //magenta
      break;
    case 9:setColorAll(120,120,0); //violet
      break;
    default:
      break;
  }
}


void Led::ledOnOff(bool state) {
	if (state == 1) {
		int cpt;
		for (cpt = 0; cpt < this->number; cpt++) {
			this->strip.setPixelColor(cpt, 255, 255, 255);
		}
		this->strip.show();
	}
	else if (state == 0){
		int cpt;
		for (cpt = 0; cpt < this->number; cpt++) {
			this->strip.setPixelColor(cpt,0,0,0);
		}
		this->strip.show();
	}
}
