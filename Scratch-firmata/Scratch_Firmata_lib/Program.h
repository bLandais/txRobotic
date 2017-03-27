/*
   To change this license header, choose License Headers in Project Properties.
   To change this template file, choose Tools | Templates
   and open the template in the editor.
*/

/*
   File:   Program.h
   Author: Mathilde

   Created on 22 novembre 2016, 14:32
*/

#ifndef PROGRAM_H
#define PROGRAM_H

#include "vector"
#include "Motor.h"
#include "Sensor.h"
#include "Button.h"
#include "Led.h"
#include "Buzzer.h"
#include "Data.h"
#include <Arduino.h>

class Program {

private:
    std::vector<Motor*> motorList;		//used to store the motor list
    std::vector<Sensor*> sensorList;	//used to store the sensor list
    std::vector<Led*> ledList;			//used to store the led strips list
    ControlPanel* controls;				//used to store the control panel
	Data* data;							//used to store the data
	Buzzer* speaker;					//used to store the buzzer

	static const int nbOfTicksPerRotation = 20;
	static const int samplingPeriodMillis = 50;
	static const int gain = 4;
	static const int rayonMM = 24;      // Rayon de la Roue
	
	static const int rayon_base = 80;
	static const int maxSpeed = 15;
	static const int maxPower = 100;
	float desiredSpeed; // en rotation par sec

  public:

	//Constructors
    Program();
    Program(int, int);

    //getters
    std::vector<Motor*> getMotorList();
    std::vector<Sensor*> getSensorList();
    std::vector<Led*> getLedList();
    ControlPanel* getControls();

    //setters
    void setControls(ControlPanel* newControlPanel);
	void setData(Data* newData);
	void setBuzzer(Buzzer* newBuzzer);
    //utilitaire
    void addMotor(Motor *const);		//add a motor object in the motor vector
    void addSensor(Sensor *const);		//add a sensor object in the sensor vector
    void addLed(Led *const);			//add a led strip object in the sensor vector

    //for demo
    void dodger(ControlPanel *const, Led *const, Led *const);			//obstacle doger program
    void lineFollower(ControlPanel *const, Led *const, Led  *const);	//lie follower program
    void joystick(ControlPanel *const , Led *const, Led *const);		//joystick program
	void useWifi(ControlPanel *const buttonPanel, Led *const ledFront, Led *const ledBack);												//joystick with wifi

    //user programs
    void arduino();			//slot for the user program

    //others
    bool checkLeft();
    bool checkRight();
    bool checkCenter();
    void updateSensor(String sensorToUpdate);
//    void checkLineSensor();

	//speed control / position control
	void initAsserv();
	void testAsserv(int);
	double calculateTicks(int target_mm);
	void avancer(int target_mm);
	void reculer(int target_mm);
	void rotation(int angle, int sens);
	float asservissement_vitesse_Motors(double desired_speed_RotPerSec, boolean sens);
	int pourcentToDigital(int pourcentage);
};

#endif /* PROGRAM_H */


