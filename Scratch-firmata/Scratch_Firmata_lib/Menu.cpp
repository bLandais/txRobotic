/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Menu.cpp
 * Author: Mathilde
 * 
 * Created on 18 octobre 2016, 15:11
 */
#include "Menu.h"
#include "Led.h"
#include "Button.h"
#include "ControlPanel.h"
#include "Buzzer.h"

//************************************************************************
//	Constructor with parameters
//************************************************************************

Menu::Menu() {

    this->locations = 0;
    this->choice = 0;
    this->tempChoice = 0;
}

//************************************************************************
//	Constructor with parameters
//	Arguments : the number of choices of programs in the menu
//	Return : nothing
//************************************************************************

Menu::Menu(int locationsNbr){
    this->locations = locationsNbr;
    this->choice = 0;
    this->tempChoice = 0;
}



//************************************************************************
//	Getters
//************************************************************************

int Menu::getLocations(){
    return this->locations;
}

//************************************************************************
//	Setters
//************************************************************************

void Menu::setLocations(int locationsNbr){
    this->locations = locationsNbr;
}

//*************************************************************************************************************************
//	Run choice
//	Arguments : the robot object with all the functionalities, the control panel object, the leds strips for front and back
//	Return : nothing
//*************************************************************************************************************************

void Menu::runChoice(Program *const robot, ControlPanel *const buttonPanel,Led *const ledFront , Led *const ledBack){
  switch(this->choice){
    case 0 : // bleu:evitement d'obstacle
      robot->dodger(buttonPanel, ledFront, ledBack);
      break;
    case 1 : // vert:joystick
      robot->joystick(buttonPanel, ledFront, ledBack);
      break;          
    case 2: //	orange:suivi de ligne
  	  robot->lineFollower(buttonPanel, ledFront, ledBack);
      break;          
    case 3 : // triangle
  		robot->triangle(buttonPanel, ledFront, ledBack);
      break;
    case 4 :
      robot->avancer(buttonPanel, ledFront, ledBack);
    break;          
    default:
      break;
  }
  this->choice = 0;
  this->tempChoice = 0;
}

//**************************************************************************************************************************
//	Function to run the program selection
//	Arguments : the robot object with all the functionalities, the control panel object, the leds strips for front and back
//	Return : nothing
//**************************************************************************************************************************

void Menu::runMenu(Program *const robot, ControlPanel *const buttonPanel, Led *const ledFront, Led *const ledBack, Buzzer *const buzzer){
     int boutonChoisi = buttonPanel->analyze();
     //boolean isChanged = false;
     if(boutonChoisi != 0) {
       
    switch(boutonChoisi){
        
            case 1 : // rightBtn
                this->tempChoice++;
                Serial.write(255);
                Serial.write(85);
                Serial.write(128);
                Serial.write(1);
                Serial.write(1);
                Serial.write(1);
                Serial.write(13);

             // buzzer->playMelody(MENUNEXT);
                delay(250);
                break;
                
            case 2: // leftBtn
                this->tempChoice--;
                Serial.write(255);
                Serial.write(85);
                Serial.write(128);
                Serial.write(1);
                Serial.write(1);
                Serial.write(3);
                Serial.write(13);
             //   buzzer->playMelody(MENUNEXT);
                delay(250);
                break;
                
            case 3 : // downBtn
                Serial.write(255);
                Serial.write(85);
                Serial.write(128);
                Serial.write(1);
                Serial.write(1);
                Serial.write(4);
                Serial.write(13);
                
                // buzzer->playMelody(MENUNEXT);
                delay(250);
                break;
                
            case 4 : // upBtn
                // buzzer->playMelody(MENUNEXT);
                Serial.write(255);
                Serial.write(85);
                Serial.write(128);
                Serial.write(1);
                Serial.write(1);
                Serial.write(2);
                Serial.write(13);
                delay(250);
                break;
                
            case 5 : // validateBtn
                Serial.write(255);
                Serial.write(85);
                Serial.write(128);
                Serial.write(1);
                Serial.write(1);
                Serial.write(0);
                Serial.write(13);
                delay(250);
                // buzzer->playMelody(VALIDATE);
                runChoice(robot, buttonPanel, ledFront, ledBack);
                break;
                
            default:
                break;
        }
            choice = abs(this->tempChoice)%locations;        
            /* Serial.println(boutonChoisi);
            delay(100);
            Serial.println(choice); */
            switch(choice) {
              case 0:
                ledFront->setColorAll(0, 128, 255);//bleu:evitement d'obstacle
                ledBack->setColorAll(0, 128, 255);
                break;
              case 1:
                ledFront->setColorAll(0, 0, 250);//vert:joystick
                ledBack->setColorAll(0, 0, 250);
                break;
              case 2:
                ledFront->setColorAll(255, 50, 160);//orange:suiveur de ligne
                ledBack->setColorAll(255, 50, 160);
                break;
              case 3:
                ledFront->setColorAll(255, 60, 60); // rose wifi
                ledBack->setColorAll(255, 60, 60);
                break;
              case 4 :
                ledFront->setColorAll(200, 0, 0); // rouge ligne droite
                ledBack->setColorAll(200, 0, 0);
                break;
            }
/*            if (choice == 0) {
    		  	ledFront->setColorAll(0, 128, 255);//bleu:evitement d'obstacle
    		    ledBack->setColorAll(0, 128, 255);
            }
            else if(choice == 1) {
                ledFront->setColorAll(0, 25, 255);//vert:joystick
                ledBack->setColorAll(0, 25, 255); 
            } /*
            else if (choice == 2) {
                ledFront->setColorAll(255, 0, 50);//orange:suiveur de ligne
    			  ledBack->setColorAll(255, 0, 50);
    		}
    		else if (choice == 3) {//rose wi fi
    			ledFront->setColorAll(253, 108, 158);
    			ledBack->setColorAll(253, 108, 158);
      } */
   }
   return;
}
