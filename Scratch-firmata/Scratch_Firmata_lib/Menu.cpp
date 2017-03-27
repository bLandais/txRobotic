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
        
        Serial.print("choice = ");
            Serial.println(this->choice);
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
                
            case 3 : // wi fi joystick
				robot->useWifi(buttonPanel, ledFront, ledBack);
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

      
     switch(buttonPanel->analyze()){
        
            case 1 : // rightBtn
                this->tempChoice++;
                buzzer->playMelody(MENUNEXT);
                //delay(250);
                break;
                
            case 2: // leftBtn
                this->tempChoice--;
                buzzer->playMelody(MENUNEXT);
               // delay(250);
                break;
                
            case 3 : // downBtn
                buzzer->playMelody(MENUNEXT);
                //delay(250);
                break;
                
            case 4 : // upBtn
                buzzer->playMelody(MENUNEXT);
                //delay(250);
                break;
                
            case 5 : // validateBtn
                buzzer->playMelody(VALIDATE);
                runChoice(robot,buttonPanel,ledFront,ledBack);
                break;
                
            default:
                break;
        }

        choice = abs(this->tempChoice)%locations;
        
        if (choice == 0){
			ledFront->setColorAll(0, 128, 255);//bleu:evitement d'obstacle
			ledBack->setColorAll(0, 128, 255);
        }
        else if(choice == 1){
          ledFront->setColorAll(0, 25, 255);//vert:joystick
		  ledBack->setColorAll(0, 25, 255);
        }
		else if (choice == 2) {
			ledFront->setColorAll(255, 0, 50);//orange:suiveur de ligne
			ledBack->setColorAll(255, 0, 50);
		}
		else if (choice == 3){//rose wi fi
			ledFront->setColorAll(253, 108, 158);
			ledBack->setColorAll(253, 108, 158);
		}
    
}

