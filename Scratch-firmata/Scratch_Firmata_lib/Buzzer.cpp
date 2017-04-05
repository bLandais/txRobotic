/*
* To change this license header, choose License Headers in Project Properties.
* To change this template file, choose Tools | Templates
* and open the template in the editor.
*/

/*
* File:   Buzzer.cpp
* Author: BenoitP
*
* Created on 7 december 2016, 11:22
*/

#include <StandardCplusplus.h>
#include <vector> 

#include <Adafruit_NeoPixel.h>

#include "Buzzer.h"
//#include "Arduino.h"


Buzzer::Buzzer() {          //Par defaut on fait rien.
}

Buzzer::Buzzer(int newPin) {
	this->pin = newPin;
	pinMode(this->pin, INPUT); //definition du pin Buzzer

	welcomeSong.push_back(DO);		//d�finition des melodies pr�enregistr�es
	welcomeSong.push_back(0);
	welcomeSong.push_back(DO);
	welcomeSong.push_back(0);
	welcomeSong.push_back(SI);
	welcomeSong.push_back(0);
	welcomeSong.push_back(SI);

	menuNext.push_back(DO);

	validate.push_back(DO);
	validate.push_back(0);
	validate.push_back(MI);
	validate.push_back(0);
	validate.push_back(SOL);
	validate.push_back(0);
	validate.push_back(DO * 2);
}

void Buzzer::setDelayRythme(int time) {
	delayRythme = time;
}

void Buzzer::setDelayAttente(int time) {
	delayAttente = time;
}

void Buzzer::playSon(int frequency) {          //Joue un seul Son

	if (frequency>10 && frequency<20000) {
		tone(this->pin, frequency, 2000); //A MODIFIER VITE PAR CE QU'IL FAUT TESTER MA
	}
	else
	{
		noTone(this->pin);
	}
}

void Buzzer::playSonDelay(int frequency, int time) {          //Joue un seul Son

	if (frequency>10 && frequency<20000) {
		tone(this->pin, frequency, time * 1000); //A MODIFIER VITE PAR CE QU'IL FAUT TESTER MA
	}
	else
	{
		noTone(this->pin);
	}
}

void Buzzer::linkKey(char key) {	//TODO: associer une note � une touche clavier

}

void Buzzer::playKey(char key) {	//TODO: jouer une note � l'appuis d'une touche associ�e

}


//	Fonction playNote	//
/*
La gamme par d�faut pour le buzzer est la 4 (celle avec le la/A 880Hz)(cf le wiki des octaves)
==> L'octave 4 en musique = � l'octave 0 dans le code.

Pour simplifier la th�orie musical toussa toussa on passe � l'octave sup�rieure en
multipliant par 2 et � l'ocatve inf�rieure en divisant par 2.
*/
void Buzzer::playNote(int octave,int note) {   //surcharge pour les demi-tons

	if (octave < 1) {		// on limite le nombre d'octave par s�curit�
		octave = 1;
	}
	else if (octave > 7) {
		octave = 7;
	}

	octave = floor(octave) - 4;			//on emp�che les puissances � virgules et on ram�ne l'octave 4 � 0 (cf le gros commentaire au dessus)
	int delta = pow(2, octave);

	switch (note) {
	case 1: playSon(DO*delta);
		break;
	case 2: playSon(DOd*delta);
		break;
	case 3: playSon(RE*delta);
		break;
	case 4: playSon(REd*delta);
		break;
	case 5: playSon(MI*delta);
		break;
	case 6: playSon(FA*delta);
		break;
	case 7: playSon(FAd*delta);
		break;
	case 8: playSon(SOL*delta);
		break;
	case 9: playSon(SOLd*delta);
		break;
	case 10: playSon(LA*delta);
		break;
	case 11: playSon(LAd*delta);
		break;
	case 12: playSon(SI*delta);
		break;
	default:
		break;
	}

	delay(delayRythme);
	noTone(this->pin);

}

void Buzzer::playMode() {	//TODO: mode piano grace � la fct linkKey

}

void Buzzer::createMelody() {	//TODO:

}


void Buzzer::playMusic(std::vector<int> melody) {

	for (auto note : melody)
	{
		playSon(note);
	}

	//int song_welcom[]={doo,2,doo,2,sii,1,sii,1}; //Faire une fonction pour optimiser le son.

}

void Buzzer::playMelody(int melody) {
	switch (melody)
	{
	case 0: playMusic(welcomeSong);
		break;
	case 1: playMusic(menuNext);
		break;
	case 2: playMusic(validate);
	default:
		break;
	}
}

void Buzzer::buzzerOnOff(bool state) {
	if (state == 1) {
		digitalWrite(this->pin, HIGH);
	}
	else if (state == 0) {
		digitalWrite(this->pin, LOW);
	}
}

void Buzzer::buzzerOnOffDelay(bool state,int time) {
	buzzerOnOff(state);
	delay(time*1000);
	buzzerOnOff(!state);
}

