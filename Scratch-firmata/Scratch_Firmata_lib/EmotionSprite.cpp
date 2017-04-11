// 
// 
// 

#include <StandardCplusplus.h>
#include <vector>

#include "EmotionSprite.h"
#include "LedMatrix.h"
#include "TabPattern.h"


EmotionSprite::EmotionSprite(String name, int pattern_number, int pattern_delay, std::vector<int> index, LedMatrix * ledMatrix, TabPattern * tabPattern) {
	this->name = name;
	this->pattern_number = pattern_number;
	this->pattern_delay = pattern_delay;
	this->emotion_array_eye = index;
	this->ledMatrix = ledMatrix;
	this->tabPattern = tabPattern;
}

//Proc�dure qui permettera d'initialier les tableaux contenant les indices des patterns
//Param�tres : 
//indexOfEmotion : tableau contenant les indices 
//sizeOfArray : taille du tableau 
//array : le tableau qui sera ensuite associ� au tableau d'emotion 
void EmotionSprite::SetEmotion_array_eye(int indexOfEmotion[], int sizeOfArray){
	for (int i = 0; i < 23; i++) {
		if (i < sizeOfArray) this->emotion_array_eye[i] = indexOfEmotion[i];
		else this->emotion_array_eye[i] = -1;
	}
}

void EmotionSprite::SetName(String name) {
	this->name = name;
}

void EmotionSprite::SetPattern_number(int pattern_number) {
	this->pattern_number = pattern_number;
}

void EmotionSprite::SetPattern_delay(int pattern_delay) {
	this->pattern_delay = pattern_delay;
}


//Fonction qui va afficher le pattern
//Param�tres : 
//emotionInAction : l'emotion sur lequel on est 
//patternCounter : pour savoir � quelle valeur le compteur de l'�motion on est 
//Si le patternCounter est �gale � la taille du tableau, alors nous sommes � la fin de l'�motion et la fonction renvera -1, sinon elle envoie emotionInAction.pattern_number-1  
int EmotionSprite::printPattern(int patternCounter, int wichMatrix) {

	std::vector<int> arrayEye;

	int testEnd;

	testEnd = this->pattern_number - 1;

	int nextIndexOfEmotion;

	for (int i = 0; i<8; i++) {
		arrayEye.push_back(tabPattern->Eyes[this->emotion_array_eye[patternCounter]][i]);
	}
	if (wichMatrix == 1) ledMatrix->printOnePattern(arrayEye, 1);
	else if (wichMatrix == 2) ledMatrix->printOnePattern(arrayEye, 2);
	else if (wichMatrix == 3) 
	{
		ledMatrix->printOnePattern(arrayEye, 1);
		ledMatrix->printOnePattern(arrayEye, 2);
	}

	delay(this->pattern_delay);

	if (patternCounter >= testEnd) {
		testEnd = -1;
	}

	return testEnd;
}

void EmotionSprite::printSprite(int wichMatrix) {
	int mem = wichMatrix;
	int counter = 0;
	int testEndOfSprite = 0;
	while (testEndOfSprite != -1) {
		testEndOfSprite = printPattern(counter,mem);
		counter = counter + 1;
	}
}

void EmotionSprite::clear() {
	int e = 0;
	for (e = 1; e <= 8; e++) {    // empty registers, turn all LEDs off 
		ledMatrix->maxAll(e, 0);
	}
	ledMatrix->maxAll(max7219_reg_intensity, 0x0f & 0x0f);
}
