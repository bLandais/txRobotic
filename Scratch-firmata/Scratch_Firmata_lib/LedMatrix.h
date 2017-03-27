// LedMatrix.h

#ifndef _LEDMATRIX_h
#define _LEDMATRIX_h

// define max7219 registers
#define max7219_reg_noop  0x00
#define max7219_reg_digit0  0x01
#define max7219_reg_digit1  0x02
#define max7219_reg_digit2  0x03
#define max7219_reg_digit3  0x04
#define max7219_reg_digit4  0x05
#define max7219_reg_digit5  0x06
#define max7219_reg_digit6  0x07
#define max7219_reg_digit7  0x08
#define max7219_reg_decodeMode  0x09
#define max7219_reg_intensity  0x0a
#define max7219_reg_scanLimit  0x0b
#define max7219_reg_shutdown  0x0c
#define max7219_reg_displayTest  0x0f

// Defining constants
#define dt 20                       // time difference in milli seconds
#define rad2degree 57.3              // Radian to degree conversion
#define Filter_gain 0.95             // e.g.  angle = angle_gyro*Filter_gain + angle_accel*(1-Filter_gain)
#define BUFFER_SIZE 32

#include <StandardCplusplus.h>
#include <vector> 

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
class LedMatrix {

public:

	int dataIn,		//DIN 
		load,		//CS  
		clock,		//CLK 
		maxInUse;	//change this variable to set how many MAX7219's you'll use

	LedMatrix();
	LedMatrix(int dataIn = 30, int load = 31, int clock = 32, int maxInUse = 1);

	void printOnePattern(std::vector<int> arrayEye, int part);
	void maxAll(int reg, int col);
	void maxOne(int maxNr, int reg, int col);
	void putByte(int data);

};
#endif

