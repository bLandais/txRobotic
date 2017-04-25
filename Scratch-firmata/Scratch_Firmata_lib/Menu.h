/*
   File:   Menu.h
   Author: Mathilde

   Created on 18 octobre 2016, 15:11
*/

#include "Led.h"
#include "ControlPanel.h"
#include "Program.h"
#include "Buzzer.h"
//#include "Wifi.h"
//#include "Data.h"

#ifndef MENU_H
#define MENU_H

class Menu {
    int choice;
    int tempChoice;
    int locations;

  public:
    Menu();				//Constructors
    Menu(int);

    int getLocations(); //getters

    void runMenu(Program *const, ControlPanel *const, Led *const, Led *const, Buzzer *const);	//Function to run the select mode menu
    void runChoice(Program *const, ControlPanel *const, Led *const, Led *const);				//Function to run the selected mode in the menu

  private:

    void setLocations(int);	//setters
};

#endif /* MENU_H */
