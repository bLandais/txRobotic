#include "Emotions.h"
#include "Arduino.h"

/*

Emotions::Emotions() {

}

Emotions::Emotions(int patternNumber) {
	this->patternNumber = patternNumber;

}

//procédure qui va afficher un pattern. 
//Paramètres :
//un tableau de 8 valeurs de LED
//le nombre de la plaque de LED a controller :
//1 : Yeux gauche
//2 : Bouche gauche
//3 : Bouche gauche
//4 : Yeux droit
void Emotions::printOnePattern(int value[8], int part) {
	int i;
	for (i = 0; i<8; i++) {
		maxOne(part, i + 1, value[i]);
	}
}

void Emotions::maxAll(int reg, int col) {    // initialize  all  MAX7219's in the system
	int c = 0;
	digitalWrite(load, LOW);  // begin     
	for (c = 1; c <= maxInUse; c++) {
		putByte(reg);  // specify register
		putByte(col);//((data & 0x01) * 256) + data >> 1); // put data
	}
	digitalWrite(load, LOW);
	digitalWrite(load, HIGH);
}

//MaxNr = Number of the MAX  | reg = Value in the line | col | wich column ?
//maxOne is for adressing different MAX7219's, 
//whilele having a couple of them cascaded
void Emotions::maxOne(int maxNr, int reg, int col) {    

	int c = 0;
	digitalWrite(load, LOW);  // begin     

	for (c = maxInUse; c > maxNr; c--) {
		putByte(0);    // means no operation
		putByte(0);    // means no operation
	}

	putByte(reg);  // specify register
	putByte(col);//((data & 0x01) * 256) + data >> 1); // put data 

	for (c = maxNr - 1; c >= 1; c--) {
		putByte(0);    // means no operation
		putByte(0);    // means no operation
	}

	digitalWrite(load, LOW); // and load da shit
	digitalWrite(load, HIGH);
}

void Emotions::putByte(int data) {
	int i = 8;
	int mask;
	while (i > 0) {
		mask = 0x01 << (i - 1);      // get bitmask
		digitalWrite(clock, LOW);   // tick
		if (data & mask) {            // choose bit
			digitalWrite(dataIn, HIGH);// send 1
		}
		else {
			digitalWrite(dataIn, LOW); // send 0
		}
		digitalWrite(clock, HIGH);   // tock
		--i;                         // move to lesser bit
	}
} 
*/

