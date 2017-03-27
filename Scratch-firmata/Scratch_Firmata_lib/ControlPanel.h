/* 
 * File:   ControlPanel.h
 * Author: Mathilde
 *
 * Created on 18 octobre 2016, 11:37
 */


#include "StandardCplusplus.h"
#include "vector"
#include "Button.h"

#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

class ControlPanel {
 
public: 
    //int btnNumber;
    int btnNumberMax;				//used to store	 the maximum number of button to resere memory for
    std::vector<Button> btnList;	//used to store the button list
    
public:
    
	//Constructors
    ControlPanel();
    ControlPanel(int);
    
    //getter
    //std::vector::size_type getBtnNumber();
    int getBtnNumberMax();					
    std::vector<Button> getBtnList();
    
    //setters
    void setBtnNumberMax(int);
      
    //l'utilitaire du boutton
    void addButton(Button);		//Add a button in the Button list
    void reserve(int);			//Reserve memory for the list of buttons	
    void controlRead();			//Read and store the status of the buttons of the control panel
    //int toBinary();
    int analyze();				//Analyse the data stored with the previous read
};

#endif /* CONTROLPANEL_H */

