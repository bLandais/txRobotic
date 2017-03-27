
/* 
 * File:   ControlPanel.cpp
 * Author: Mathilde
 * 
 * Created on 18 octobre 2016, 11:37
 */

#include <map>
#include "vector"
#include <math.h>
#include <iostream>
#include "Arduino.h"
#include "ControlPanel.h"
#include "Button.h"


// constructors
ControlPanel::ControlPanel() {

}

ControlPanel::ControlPanel(int newBtnNumberMax) {
    this->btnList.reserve(newBtnNumberMax);
}

//getter
/*
std::vector::size_type ControlPanel::getBtnNumber(){
    return this->btnList.size;
}*/

int ControlPanel::getBtnNumberMax(){
    return this->btnList.capacity();
}

std::vector<Button> ControlPanel::getBtnList() {
    return this->btnList;
}

//setters

void ControlPanel::setBtnNumberMax(int newBtnNumberMax){
    this->btnList.reserve(newBtnNumberMax);
}

//************************************************************************
//	Add button in the control panel button list
//	Arguments : the button you want to add in the list
//	Return : nothing
//************************************************************************

void ControlPanel::addButton(Button newBtn){ 
        this->btnList.push_back(newBtn);  
}

//************************************************************************
//	Memory reservation for buttons
//	Arguments : the number of buttons you want to rreserve memory for
//	Return : nothing
//************************************************************************

void ControlPanel::reserve(int newBtnNumberMax){
    this->btnList.reserve(newBtnNumberMax);
}

//************************************************************************
//	Control panel read: read all buttons stocked in the button vector
//	Arguments : none
//	Return : none
//************************************************************************

void ControlPanel::controlRead(){
    for (auto &btn: this->btnList){
        btn.readValue();
    }
}

//int ControlPanel::toBinary(){
//    int count=0;
//    int binary=0;
//
//    controlRead();
//    for (auto &btn: this->btnList){
//        if(btn.getValue() == LOW){
//            binary = binary + pow(10,count);
//        }
//        count++;
//    }
//    return binary;
//}

//************************************************************************
//	Control panel analyse: analyse the values stored in memory from read
//	Arguments : none
//	Return : 1 right; 2 left; 3 down; 4 up; 5 validate
//************************************************************************

int ControlPanel::analyze(){
    controlRead();
    
    if(this->btnList[0].value == LOW){
      return 1;
    }
    else if(this->btnList[1].value == LOW){
      return 2;
    }
    else if(this->btnList[2].value == LOW){
      return 3;
    }
    else if(this->btnList[3].value == LOW){
      return 4;
    }
    else if(this->btnList[4].value == LOW){
      return 5;
    }else{
      return 0;
    }  
}
