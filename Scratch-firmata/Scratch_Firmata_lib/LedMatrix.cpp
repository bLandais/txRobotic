// 
// 
// 

#include "LedMatrix.h"
#include "EmotionSprite.h"
#include "Arduino.h"

LedMatrix::LedMatrix() {

}

LedMatrix::LedMatrix(int dataIn, int load, int clock, int maxInUse) {
	this->dataIn = dataIn;
	this->load = load;
	this->clock = clock;
	this->maxInUse = maxInUse;
	pinMode(this->dataIn, OUTPUT);
	pinMode(this->load, OUTPUT);
	pinMode(this->clock, OUTPUT);

/*
	int e = 0;
	Serial.begin(115200);
	digitalWrite(13, HIGH);

	//initiation of the max 7219
	maxAll(max7219_reg_scanLimit, 0x07);
	maxAll(max7219_reg_decodeMode, 0x00);  // using a led matrix (not digits)
	maxAll(max7219_reg_shutdown, 0x01);    // not in shutdown mode
	maxAll(max7219_reg_displayTest, 0x00); // no display test
	for (e = 1; e <= 8; e++) {    // empty registers, turn all LEDs off 
		maxAll(e, 0);
	}
	maxAll(max7219_reg_intensity, 0x0f & 0x0f);    // the first 0x0f is the value you can set
												   // range: 0x00 to 0x0f 
*/
}

//procédure qui va afficher un pattern. 
//Paramètres :
//un tableau de 8 valeurs de LED
//le nombre de la plaque de LED a controller :
//1 : Yeux gauche
//2 : Bouche gauche
//3 : Bouche gauche
//4 : Yeux droit
void LedMatrix::printOnePattern(std::vector<int> value, int part) {
	int i;
	for (i = 0; i<8; i++) {
		maxOne(part, i + 1, value[i]);
	}
}

void LedMatrix::maxAll(int reg, int col) {    // initialize  all  MAX7219's in the system
	int c = 0;
	digitalWrite(this->load, LOW);  // begin     
	for (c = 1; c <= this->maxInUse; c++) {
		putByte(reg);  // specify register
		putByte(col);//((data & 0x01) * 256) + data >> 1); // put data
	}
	digitalWrite(this->load, LOW);
	digitalWrite(this->load, HIGH);
}

//MaxNr = Number of the MAX  | reg = Value in the line | col | wich column ?
//maxOne is for adressing different MAX7219's, 
//whilele having a couple of them cascaded
void LedMatrix::maxOne(int maxNr, int reg, int col) {

	int c = 0;
	digitalWrite(this->load, LOW);  // begin     

	for (c = this->maxInUse; c > maxNr; c--) {
		putByte(0);    // means no operation
		putByte(0);    // means no operation
	}

	putByte(reg);  // specify register
	putByte(col);//((data & 0x01) * 256) + data >> 1); // put data 

	for (c = maxNr - 1; c >= 1; c--) {
		putByte(0);    // means no operation
		putByte(0);    // means no operation
	}

	digitalWrite(this->load, LOW); // and load da shit
	digitalWrite(this->load, HIGH);
}

void LedMatrix::putByte(int data) {
	int i = 8;
	int mask;
	while (i > 0) {
		mask = 0x01 << (i - 1);      // get bitmask
		digitalWrite(this->clock, LOW);   // tick
		if (data & mask) {            // choose bit
			digitalWrite(this->dataIn, HIGH);// send 1
		}
		else {
			digitalWrite(this->dataIn, LOW); // send 0
		}
		digitalWrite(this->clock, HIGH);   // tock
		--i;                         // move to lesser bit
	}
}