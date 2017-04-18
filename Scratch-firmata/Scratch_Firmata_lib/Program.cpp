/*
   To change this license header, choose License Headers in Project Properties.
   To change this template file, choose Tools | Templates
   and open the template in the editor.
*/

/*
   File:   Program.cpp
   Author: Mathilde

   Created on 22 novembre 2016, 14:32
*/


#include "Arduino.h"
#include "ControlPanel.h"
#include "Button.h"
#include "Program.h"
#include "Led.h"


static float circonference;			// Circonférence de la roue
 


//************************************************************************
//	Constructor
//************************************************************************

Program::Program() {
	circonference = 2 * 3.14159 * rayonMM;
	desiredSpeed = 6;
}

//************************************************************************
//	Constructor with parameters
//	Arguments : the number of motors; the number of sensors
//	Return : nothing
//************************************************************************

Program::Program(int motorNbr, int sensorNbr) {

	circonference = 2 * 3.14159 * rayonMM;
	this->motorList.reserve(motorNbr);
	this->sensorList.reserve(sensorNbr);
  
}

//************************************************************************
//	Getters
//************************************************************************

std::vector<Motor*> Program::getMotorList()
{
  return motorList;
}

std::vector<Sensor*> Program::getSensorList()
{
  return sensorList;
}

std::vector<Led*> Program::getLedList()
{
  return ledList;
}

ControlPanel* Program::getControls()
{
  return this->controls;
}

//************************************************************************
//	Setters
//************************************************************************

void Program::setControls(ControlPanel* newControlPanel)
{
  this->controls = newControlPanel;
}

void Program::setData(Data * newData)
{
	this->data = newData;
}

void Program::setBuzzer(Buzzer * newBuzzer)
{
	this->speaker = newBuzzer;
}

void Program::addMotor(Motor *const newMotor)
{
  this->motorList.push_back(newMotor);
}

void Program::addSensor(Sensor *const newSensor)
{
  this->sensorList.push_back(newSensor);
}

void Program::addLed(Led * const newLeds)
{
  this->ledList.push_back(newLeds);
}

//************************************************************************
//	Program obstacle doger
//	Arguments : the button panel object, the two leds strip objects
//	Return : nothing
//************************************************************************

void Program::dodger(ControlPanel *const buttonPanel, Led *const ledFront, Led *const ledBack)
{
  ledFront->setColorAll(1, 250, 49);//bleu foncé
  ledBack->setColorAll(1, 250, 49);
  delay(250);
  int speed = 110;

  do {

    this->updateSensor("distance");
    for (int i = 0; i < this->sensorList.size(); i++) {
      if (this->sensorList[i]->getType() == SensorType::infraR && this->sensorList[i]->getPosition() == 0 && this->sensorList[i]->getValue() == true) {
        Serial.println("Reculer");
        motorList[0]->setDirection(true);
        motorList[1]->setDirection(false);
        motorList[0]->setSpeed(0);
		    motorList[1]->setSpeed(255);
		    delay(20);
        motorList[1]->setSpeed(speed);
        //i = 20; <<<<<<<<<<<<<<<<< Pourquoi utiliser un for pour en sortir à la fin du premier tour?
      }
      else if (this->sensorList[i]->getType() == SensorType::infraR && this->sensorList[i]->getPosition() == 1 && this->sensorList[i]->getValue() == true) {
        Serial.println("tourner droite");
        motorList[0]->setDirection(true);
        motorList[1]->setDirection(true);
        motorList[1]->setSpeed(0);
        motorList[0]->setSpeed(150);
        delay(20);
        motorList[0]->setSpeed(speed);
        //i = 20;
      }
      else if (this->sensorList[i]->getType() == SensorType::infraR && this->sensorList[i]->getPosition() == 2 && this->sensorList[i]->getValue() == true) {
        Serial.println("tourner fort droite");
        motorList[0]->setDirection(true);
        motorList[1]->setDirection(false);
        motorList[0]->setSpeed(255);
        motorList[1]->setSpeed(250);
        delay(20);
        motorList[0]->setSpeed(speed);
        motorList[1]->setSpeed(speed);
        //i = 20;
        }
        
      else if (this->sensorList[i]->getType() == SensorType::infraR && this->sensorList[i]->getPosition() == -1 && this->sensorList[i]->getValue() == true) {
        Serial.println("tourner gauche");
        motorList[0]->setDirection(true);
        motorList[1]->setDirection(true);
        motorList[0]->setSpeed(0);
        motorList[1]->setSpeed(255);
        delay(20);
        motorList[1]->setSpeed(speed);
        i = 20;;
      }
      
        else if (this->sensorList[i]->getType() == SensorType::infraR && this->sensorList[i]->getPosition() == -2 && this->sensorList[i]->getValue() == true) {
        Serial.println("tourner fort gauche");
        motorList[0]->setDirection(false);
        motorList[1]->setDirection(true);
        motorList[0]->setSpeed(255);
        motorList[1]->setSpeed(255);
        delay(20);
        motorList[0]->setSpeed(speed);
        motorList[1]->setSpeed(speed);
        i = 20;
      }
      else if (this->sensorList[i]->getType() == SensorType::infraR && (this->sensorList[i]->getPosition() == 10 || this->sensorList[i]->getPosition() == -10)  && this->sensorList[i]->getValue() == true) {
        Serial.println("aller avant");
        motorList[0]->setDirection(true);
        motorList[1]->setDirection(true);
        motorList[0]->setSpeed(255);
        motorList[1]->setSpeed(255);
        delay(20);
        i = 20;
      }

      else if (this->sensorList[0]->getValue() == false && this->sensorList[1]->getValue() == false && this->sensorList[2]->getValue() == false ){
        Serial.println("Tout droit");
        motorList[0]->setDirection(true);
        motorList[1]->setDirection(true);
        motorList[0]->setSpeed(150);
        motorList[1]->setSpeed(150);
    		delay(20);
    		motorList[0]->setSpeed(speed);
        motorList[1]->setSpeed(speed);
      }
      delay(70);
    }
  }
  while (buttonPanel->analyze() != 5);
  delay(200);
  this->motorList[0]->setSpeed(0);
  this->motorList[1]->setSpeed(0);
}

//************************************************************************
//	Program line follower
//	Arguments : the buton panel object, the two leds strip objects
//	Return : nothing
//************************************************************************

void Program::lineFollower(ControlPanel *const buttonPanel, Led *const ledFront, Led *const ledBack) {

  int premierCapt = 0;
  int etat0 = 0, etat1 = 0, etat = 0;
  int i = 0;

  ledFront->setColorAll(254, 0, 27);//orange foncé
  ledFront->setColorAll(254, 0, 27);

  for (int i = 0; i < this->sensorList.size(); i++) {
    if (this->sensorList[i]->getType() != SensorType::line) {

      premierCapt++;
    }
  }
  do {
    updateSensor("line");

    if (this->sensorList[premierCapt]->getValue() == true && this->sensorList[premierCapt + 1]->getValue() == false && etat == 0) {

      etat = 1;

    }

    else if (this->sensorList[premierCapt + 1]->getValue() == true && this->sensorList[premierCapt]->getValue() == true && etat == 1) {

      etat = 1;
    }

    else if (this->sensorList[premierCapt + 1]->getValue() == true && this->sensorList[premierCapt]->getValue() == false && etat == 0) {

      etat = 2;
    }

    else if (this->sensorList[premierCapt + 1]->getValue() == true && this->sensorList[premierCapt]->getValue() == true && etat == 2) {

      etat = 2;
    }
    else if (this->sensorList[premierCapt + 1]->getValue() == false && this->sensorList[premierCapt]->getValue() == false && (etat == 1 || etat == 2)) {

      etat = etat;
    }

	else {
		
		etat = 0;
	}


    switch (etat)
    {
      case 0 :
        if (etat1 != 0) {
          //Serial.println("Tourner tout droit");
          motorList[0]->setDirection(true);
          motorList[1]->setDirection(true);
		  motorList[1]->setSpeed(150);
		  motorList[0]->setSpeed(150);
		  delay(20);
          motorList[0]->setSpeed(80);
          motorList[1]->setSpeed(80);
        }
        delay(60);
        break;
      case 1 :
        if (etat1 != 1) {
          //Serial.println("Tourner gauche");
          motorList[0]->setDirection(true);
          motorList[1]->setDirection(true);
		  motorList[0]->setSpeed(0);
        }
        delay(80);
        break;
      case 2 :
        if (etat1 != 2) {
          //Serial.println("Tourner droite");
          motorList[0]->setDirection(true);
          motorList[1]->setDirection(true);
		  motorList[1]->setSpeed(0);
        }
        delay(80);
      default:

        break;
    }
    etat1 = etat;
  }
  while (buttonPanel->analyze() != 5);
  delay(200);
  this->motorList[0]->setSpeed(0);
  this->motorList[1]->setSpeed(0);
}


//************************************************************************
//	Joystick program
//	Arguments : the button panel object, the two leds strip objects
//	Return : nothing
//************************************************************************

void Program::joystick(ControlPanel *const buttonPanel, Led *const ledFront, Led *const ledBack) {

  ledFront->setColorAll(136, 29, 66);
  ledBack->setColorAll(136, 29, 66);
  bool loop = true;
  int state = 0;
  this->motorList[0]->setDirection(true);
  this->motorList[1]->setDirection(true);

  while (loop) {

    delay(250);

    switch (buttonPanel->analyze()) {

      case 1 : // rightBtn
        ledFront->setColorAll(88, 0, 41);//bleu foncé
        ledBack->setColorAll(88, 0, 41);
        if (state != 1) {
          Serial.print("Case 1 first time");
          this->motorList[0]->setDirection(true);
          this->motorList[1]->setDirection(true);
          delay(5);
          this->motorList[0]->setSpeed(125);
          this->motorList[1]->setSpeed(0);
        }
        break;

      case 2: // leftBtn
        ledFront->setColorAll(44, 255, 117);//bleu ciel
        ledBack->setColorAll(44, 255, 117);
        if (state != 2) {
          Serial.print("Case 2 first time");
          this->motorList[0]->setDirection(true);
          this->motorList[1]->setDirection(true);
          delay(5);
          this->motorList[0]->setSpeed(0);
          this->motorList[1]->setSpeed(125);
        }
        break;

      case 3 : // downBtn
        ledFront->setColorAll(121, 249, 28);//violet
        ledBack->setColorAll(121, 249, 28);
        if (state != 3) {
          Serial.print("Case 3 first time");
          this->motorList[0]->setDirection(false);
          this->motorList[1]->setDirection(false);
          delay(5);
          this->motorList[0]->setSpeed(125);
          this->motorList[1]->setSpeed(125);
        }
        break;

      case 4 : // upBtn
        ledFront->setColorAll(129, 83, 20);//framboise
        ledBack->setColorAll(129, 83, 20);
        if (state != 4) {
          Serial.print("Case 4 first time");
          this->motorList[0]->setDirection(true);
          this->motorList[1]->setDirection(true);
          delay(5);
          this->motorList[0]->setSpeed(125);
          this->motorList[1]->setSpeed(125);
        }
        break;

      case 5 : // validateBtn
        loop = false;
        break;

      default:
        this->motorList[0]->setSpeed(0);
        this->motorList[1]->setSpeed(0);
        break;
    }
    state = buttonPanel->analyze();
  }
  this->motorList[0]->setSpeed(0);
  this->motorList[1]->setSpeed(0);
  delay(750);
}

//************************************************************************
//	User program
//	Arguments : none
//	Return : nothing
//************************************************************************

void Program::useWifi(ControlPanel *const buttonPanel, Led *const ledFront, Led *const ledBack) {
  ledFront->setColorAll(255, 10, 255);
  ledBack->setColorAll(255, 10, 255);

  while (buttonPanel->analyze() != 5) {
    //*data = data->receiveData();
    int a = data->getA();
    int p = data->getP();
    switch (data->analyze()) {
    case 0: // sleep
      delay(5);
      this->motorList[0]->setDirection(true);
      this->motorList[1]->setDirection(true);
      this->motorList[1]->setSpeed(0);
      this->motorList[0]->setSpeed(0);

      // Print pour test 
      //Serial.println("ARRET");
      break;
    case 1: // up
      delay(5);
      this->motorList[0]->setDirection(true);
      this->motorList[1]->setDirection(true);
      this->motorList[1]->setSpeed(240);
      this->motorList[0]->setSpeed(240);

      // Print pour test 
      //Serial.println("up");
      break;
    case 2:
      delay(5);
      this->motorList[0]->setDirection(true);
      this->motorList[1]->setDirection(true);
      this->motorList[0]->setSpeed(240);
      this->motorList[1]->setSpeed(200);
      // Print pour test 
      //Serial.println("up right");
      break;
    case 3:
      delay(5);
      this->motorList[0]->setDirection(true);
      this->motorList[1]->setDirection(true);
      this->motorList[0]->setSpeed(200);
      this->motorList[1]->setSpeed(100);
      //Serial.println("droite"); 
      break;
    case 4:
      delay(5);
      this->motorList[0]->setDirection(false);
      this->motorList[1]->setDirection(false);
      this->motorList[0]->setSpeed(240);
      this->motorList[1]->setSpeed(200);
      //Serial.println(" down right");
      break;
    case 5:
      delay(5);
      this->motorList[0]->setDirection(false);
      this->motorList[1]->setDirection(false);
      this->motorList[0]->setSpeed(240);
      this->motorList[1]->setSpeed(240);
      //Serial.println("down");
      break;
    case 6:
      delay(5);
      this->motorList[0]->setDirection(false);
      this->motorList[1]->setDirection(false);
      this->motorList[0]->setSpeed(200);
      this->motorList[1]->setSpeed(240);
      //Serial.println("down left");
      break;
    case 7:
      delay(5);
      this->motorList[0]->setDirection(true);
      this->motorList[1]->setDirection(true);
      this->motorList[0]->setSpeed(100);
      this->motorList[1]->setSpeed(200);
      //Serial.println("left");
      break;
    case 8:
      delay(5);
      this->motorList[0]->setDirection(true);
      this->motorList[1]->setDirection(true);
      this->motorList[0]->setSpeed(200);
      this->motorList[1]->setSpeed(240);
      //Serial.println("upleft");
      break;
    default:
      this->motorList[0]->setDirection(true);
      this->motorList[1]->setDirection(true);
      this->motorList[1]->setSpeed(0);
      this->motorList[0]->setSpeed(0);
      break;
    }
  }
  motorList[0]->setSpeed(0);
  motorList[1]->setSpeed(0);
}

//************************************************************************
//	User program
//	Arguments : none
//	Return : nothing
//************************************************************************

void Program::arduino() {

}

//************************************************************************
//	Sensor data updating function
//	Arguments : the type of sensor to update ("distance" or "line")
//	Return : nothing
//************************************************************************

void Program::updateSensor(String sensorToUpdate) {
  if (sensorToUpdate == "distance") {
    for (int i = 0; i < this->sensorList.size(); i++)
    {
      if (this->sensorList[i]->getType() == SensorType::infraR) {
        this->sensorList[i]->read();
      }
    }
  }
  if (sensorToUpdate == "line") {
    for (int i = 0; i < this->sensorList.size(); i++)
    {
      if (this->sensorList[i]->getType() == SensorType::line) {
        this->sensorList[i]->read();
      }
    }
  }
}

//bool Program::checkRight() {
//  bool result = false;
//  for (int i = 0; i < this->sensorList.size(); i++)
//  {
//    if (this->sensorList[i]->getValue() && this->sensorList[i]->getPosition() < 0)
//    {
//      result = true;
//    }
//  }
//
//  return result;
//}

//bool Program::checkLeft() {
//  bool result = false;
//  for (int i = 0; i < this->sensorList.size(); i++)
//  {
//    if (this->sensorList[i]->getValue() && this->sensorList[i]->getPosition() > 0)
//    {
//      result = true;
//    }
//  }
//
//  return result;
//}

//bool Program::checkCenter() {
//  bool result = false;
//  for (int i = 0; i < this->sensorList.size(); i++)
//  {
//    if (this->sensorList[i]->getValue() && this->sensorList[i]->getPosition() == 0)
//    {
//      result = true;
//    }
//  }
//
//  return result;
//}

//void checkLineSensor() {
//  for (int i = 0; i < 2; i++) {
//
//    if (i == 0) {
//      Serial.println("Capteur gauche");
//    }
//    else {
//      Serial.println("Capteur droit");
//    }
//    digitalWrite(sensCommand[i], HIGH);
//    delay(5);
//    rawtcrt[i] = analogRead(sensData[i]);
//    Serial.println(rawtcrt[i]);
//    if (rawtcrt[i] < colorThreshold) {
//      bwtcrt[i] = 0;
//    }
//    else {
//      bwtcrt[i] = 1;
//    }
//    Serial.println(bwtcrt[0]);
//    Serial.println(bwtcrt[1]);
//    delay(5);
//    digitalWrite(sensCommand[i], LOW);
//  }

void Program::testAsserv(int target_mm =1000) {
	
	//avancer(target_mm);

	/*for (int i = 0; i < 2; i++) {
		Program::avancer(target_mm);
		delay(500);

		Rotation(95, 1);
		delay(500);

		target_mm = 500;
		Avancer(target_mm);
		delay(500);

		Rotation(95, 1);
		delay(500);
	}*/
}



double Program::calculateTicks(int target_mm) {
	return (target_mm * gain * nbOfTicksPerRotation / circonference );
}  

void Program::avancer(ControlPanel *const buttonPanel, Led *const ledFront, Led *const ledBack) {
  //test();
  //foo.init();
  int target_mm=1000;
  ledFront->setColorAll(200, 0, 0);
  ledBack->setColorAll(200, 0, 0);
  int target_ticks=calculateTicks(target_mm);
	Serial.print("target_ticks : ");
	Serial.println(target_ticks);

	//motorList[0]->setEncoderPos(0);
	//motorList[1]->setEncoderPos(0);
	Serial.println("dedans avant while");
  while (buttonPanel->analyze() != 5) {
    Serial.println(encoder1Pos);
    if (encoder1Pos > target_ticks){
      break;
    }
  asservissement_vitesse_Motors(200, false);
  //motorList[0]->setSpeed(200);
 // motorList[1]->setSpeed(200);   
 

	//	Serial.print(motorList[0]->getEncoderPos());
		//Serial.print(" , ");
	//	Serial.println(motorList[1]->getEncoderPos());
	}
  delay(200);
  motorList[0]->setSpeed(0);
  motorList[1]->setSpeed(0);
  Serial.println("fin");

	//  Serial.println("dedans apres while");

}

void Program::reculer(int target_mm) {

	double target_ticks = calculateTicks(target_mm);
	//  Serial.print("target_ticks : ");
	//  Serial.println(target_ticks);

	motorList[0]->setEncoderPos(0);
	motorList[1]->setEncoderPos(0);
	//  Serial.println("dedans avant while");

	while (motorList[0]->getEncoderPos() < target_ticks) {
		asservissement_vitesse_Motors(desiredSpeed, true);
		//    Serial.print(encoder1Pos);
		//    Serial.print(" , ");
		//    Serial.println(encoder2Pos);
	}

	motorList[0]->setSpeed(0);
	motorList[1]->setSpeed(0);

	//  Serial.println("dedans apres while");

}

void Program::rotation(int angle, int sens) {      // Sens des aiguilles et sens inverse des aiguilles
	float distance_desiree = (rayon_base * 3.14159 * 2 * angle) / 360.0;      // 2*Pi*R*angle / 360
	double target_ticks = calculateTicks(distance_desiree);
	//  Serial.println(target_ticks);

	motorList[0]->setEncoderPos(0);
	motorList[1]->setEncoderPos(0);

	//  Serial.println("dans rotation");

	if (sens == 0) {                            // Sens horaire
		while (motorList[0]->getEncoderPos() < target_ticks) {
			motorList[0]->setSpeed(180);
			motorList[1]->setSpeed(0);
			Serial.println(motorList[0]->getEncoderPos());

		}
		motorList[0]->setSpeed(0);
		motorList[1]->setSpeed(0);
		//    Serial.println("dans le if de rotation");
	}
	else if (sens == 1) {                       // sens anti-horaire
		while (motorList[1]->getEncoderPos() < target_ticks) {
			motorList[0]->setSpeed(0);
			motorList[1]->setSpeed(180);
			Serial.println(motorList[1]->getEncoderPos());
		}
		motorList[0]->setSpeed(0);
		motorList[1]->setSpeed(0);
	}
}

float Program::asservissement_vitesse_Motors(double desired_speed_RotPerSec, boolean sens)       // Asservissement du Moteur Gauche (1)
{
	//  Serial.print("Vitesse desiree : ");
	//  Serial.println(desired_speed_RotPerSec);

	if (desired_speed_RotPerSec > maxSpeed) {
		desired_speed_RotPerSec = maxSpeed;
	}

	// Calcul de la commande :
	float percentPower_1 = motorList[0]->percentPower();

	// Calcul de la commande :
	float percentPower_2 = motorList[1]->percentPower();


	if (percentPower_1 > maxPower) {
		percentPower_1 = maxPower;
	}
	if (percentPower_1 < -maxPower) {
		percentPower_1 = -maxPower;
	}

	if (percentPower_2 > maxPower) {
		percentPower_2 = maxPower;
	}
	if (percentPower_2 < -maxPower) {
		percentPower_2 = -maxPower;
	}

	float erreur_cmd = percentPower_1 - percentPower_2;
	Serial.println(erreur_cmd);

	if (erreur_cmd < 0) {
		percentPower_2 = percentPower_2 + erreur_cmd;
		percentPower_1 = percentPower_1 - erreur_cmd;
	}
	else if (erreur_cmd > 0) {
		percentPower_1 = percentPower_1 - erreur_cmd;
	}

	  Serial.print("commande en pourcentage pour Moteur gauche : ");
	  Serial.print(percentPower_1);
	  Serial.print(",  commande en pourcentage pour Moteur droit : ");
   Serial.println(percentPower_2);

	motorList[0]->setDirection(sens);
	motorList[0]->setSpeed(pourcentToDigital(percentPower_1)); 
	motorList[1]->setDirection(sens);
	motorList[1]->setSpeed(pourcentToDigital(percentPower_2));


	// Record ticks over time
	long start = millis();
	int startTicks_1 = motorList[0]->getEncoderPos();
	int startTicks_2 = motorList[1]->getEncoderPos();

	while (millis() - start < (samplingPeriodMillis)) {
	}

	  Serial.print("start : ");
	  Serial.println(start);

	int ticks_1 = motorList[0]->getEncoderPos() - startTicks_1;
	int ticks_2 = motorList[1]->getEncoderPos() - startTicks_2;

	Serial.println(ticks_1);

	motorList[0]->setRealSpeed((ticks_1 / nbOfTicksPerRotation) / (samplingPeriodMillis / 1000.0)); // Vitesse réelle en rot/sec
	motorList[1]->setRealSpeed((ticks_2 / nbOfTicksPerRotation) / (samplingPeriodMillis / 1000.0)); // Vitesse réelle en rot/sec

																					   //  Serial.print("Vitesse reelle Moteur gauche : ");
																					   //  Serial.print(speed_1_Real);
																					   //  Serial.print(",  Vitesse reelle Moteur droit : ");
																					   //  Serial.println(speed_2_Real);
																					   //
																					   //  Serial.println();

	motorList[0]->addEkValue(desired_speed_RotPerSec - motorList[0]->getRealSpeed());
	motorList[1]->addEkValue(desired_speed_RotPerSec - motorList[1]->getRealSpeed());
	motorList[0]->addPowerValue(percentPower_1);
	motorList[1]->addPowerValue(percentPower_2);

//	Serial.println(ek_1);

//	return (percentPower_1);

}

int Program::pourcentToDigital(int pourcentage)  //Convertit un pourcentage positif en 0-255
{
	if (pourcentage >= 0)
	{ //then
		return (pourcentage * 255 / 100); //convertion
	}
	else
	{ //Si le pourcentage est négatif, on renvois zero.
		return (0);
	}
}

//-------------------------------------------------Compteurs-------------------------------------------------------------------
// Interruption appelée à tous les changements d'état

 // static Program* instance = NULL;
  
void Program::init() {
    // Interruption de l'encodeur A en sortie 5 (pin 18)
    attachInterrupt(5, interruptCount1, RISING);   // increase counter of Motor 1 when speed sensor pin goes High
    instance = this;
   // Interruption de l'encodeur B en sortie 4 (pin 19)
  //  attachInterrupt(4, docount_2, RISING);   // increase counter of Motor 2 when speed sensor pin goes High
}

void Program::setEncoder1PosDecrease() {
  encoder1Pos--;
}

void Program::setEncoder1PosIncrease() {
  encoder1Pos++;
}
