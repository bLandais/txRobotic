/*
* To change this license header, choose License Headers in Project Properties.
* To change this template file, choose Tools | Templates
* and open the template in the editor.
*/

/*
* File:   Buzzer.h
*/

#ifndef BUZZER_H
#define BUZZER_H

#include <StandardCplusplus.h>
#include <vector>

#define WELCOMSONG 1    //Son de bienvenu
#define MENUNEXT 2      //A Chaque Changement De Menu
#define VALIDATE 3      //quand on appuis sur le bouton au milieu.

#define DO 523.25
#define DOd 554.37
#define RE 587.33
#define REd 622.25
#define MI 659.26
#define FA 698.46
#define FAd 739.99
#define SOL 783.99
#define SOLd 830.61
#define LA 880
#define LAd 932.33
#define SI 987.77

class Buzzer {

private:
	int pin;

	int delayRythme = 300;
	int delayAttente = 100;

	std::vector<int> welcomeSong;
	std::vector<int> menuNext;
	std::vector<int> validate;

public:
	//Son ne pas oublier la fonction qui défini le pin
	//Les fonctions qui jour les Buzzer.
	Buzzer();
	Buzzer(int); //definition du pin speaker

	void setDelayRythme(int time);
	void setDelayAttente(int time);

	void playSon(int frequency);
	void playSonDelay(int frequency, int time);

	void playNote(int octave,int note);

	void playMelody(int melody);
	void playMusic(std::vector<int>);

	void buzzerOnOff(bool state);
	void buzzerOnOffDelay(bool state, int time);

	void linkKey(char key); //TODO
	void playKey(char key);	//TODO
	void playMode();		//TODO
	void createMelody();	//TODO

};

#endif /* Buzzer_H */
