/*
  Firmata is a generic protocol for communicating with microcontrollers
  from software on a host computer. It is intended to work with
  any host computer software package.

  To download a host software package, please clink on the following link
  to open the list of Firmata client libraries your default browser.

  https://github.com/firmata/arduino#firmata-client-libraries

  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2009-2015 Jeff Hoefs.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Last updated by Jeff Hoefs: August 9th, 2015
*/

#include "TabPattern.h"
#include "LedMatrix.h"
#include <Wire.h>
#include <Firmata.h>
#include <Boards.h>

#include <Adafruit_NeoPixel.h>

#include <utility.h>
#include <unwind-cxx.h>
#include <system_configuration.h>

#include <StandardCplusplus.h>
#include <vector>

//#include "Arduino.h"
#include "SensorType.h"
#include "Sensor.h"
#include "Motor.h"
#include "Button.h"
#include "Menu.h"
#include "Led.h"
#include "Buzzer.h"
#include "Emotions.h"
#include "EmotionSprite.h"


#define I2C_WRITE                   B00000000
#define I2C_READ                    B00001000
#define I2C_READ_CONTINUOUSLY       B00010000
#define I2C_STOP_READING            B00011000
#define I2C_READ_WRITE_MODE_MASK    B00011000
#define I2C_10BIT_ADDRESS_MODE_MASK B00100000
#define I2C_MAX_QUERIES             8
#define I2C_REGISTER_NOT_SPECIFIED  -1

// AJOUT FONCTIONALITES SCRATCH
#define MOTOR 0xA1
#define LED 0xA2
#define BUZZER 0xA3
#define EMOTIONS 0xA4

// the minimum interval for sampling analog input
#define MINIMUM_SAMPLING_INTERVAL 10

using namespace std;
int choice = -1;
int locations = 5;

//------Motors--------://
Motor motor_l = Motor(9, 8, 10);
Motor motor_r = Motor(6, 7, 5);


//------Sensors-------://
Sensor sensorAvG = Sensor(37, A10, 130, SensorType::infraR, -1);
Sensor sensorAvM = Sensor(39, A11, 130, SensorType::infraR, 0);
Sensor sensorAvD = Sensor(41, A12, 130, SensorType::infraR, 1);

Sensor sensorLineLeft = Sensor(40, A0, 500, SensorType::line, 1);
Sensor sensorLineRight = Sensor(42, A1, 500, SensorType::line, -1);


//------LEDs----------://
Led backLeds = Led(12, 2);
Led frontLeds = Led(44, 6);

//------buttons-------://
Button btn_up = Button(22);
Button btn_left = Button(23);
Button btn_right = Button(24);
Button btn_down = Button(25);
Button btn_valid = Button(26);

//------Controls-------://
ControlPanel controls(5);

//------Buzzer---------://
Buzzer buzzer(11);

//------Son------------://
Buzzer speaker = Buzzer(11);

//------menu-------://
Menu menu = Menu(locations);

//------robot-------://
Program robot = Program(2, 7);

//------Init-Emotions--://

TabPattern tabPattern;
LedMatrix ledMatrix = LedMatrix(30, 31, 32, 2);

std::vector<int>
indexOfHappyEye1 { 0, 1, 2, 1, 0, 3, 4, 3, 0 },
                 indexOfInLoveEye1 { 0, 9, 10, 10, 11, 11, 10, 10, 11, 11, 10, 10, 11, 11, 10, 10, 11, 11, 10, 10, 10, 9, 0 },
                 indexOfEyeClosesEye1 { 0, 5, 6, 7, 8, 8, 8, 8, 8, 8, 8, 7, 6, 5, 0 },
                 indexOfCrazyEye1 { 0, 3, 4, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 4, 3, 0 },
                 indexOfDeadEye1 { 0, 6, 7, 44, 45, 45, 45, 45, 45, 44, 7, 6, 0 },
                 indexOfSnowEye1 { 47, 48, 49, 46, 46, 46, 46, 49, 48, 47 },
                 indexOfStarEye1 { 51, 51, 52, 52, 53, 53, 52, 52 },
                 indexOfALLREDEye1 { 54 };

EmotionSprite
happy("happy", sizeof(indexOfHappyEye1), 150, indexOfHappyEye1, &ledMatrix, &tabPattern),
      inLove("inLove", sizeof(indexOfInLoveEye1), 150, indexOfInLoveEye1, &ledMatrix, &tabPattern),
      eyeCloses("inLove", sizeof(indexOfEyeClosesEye1), 150, indexOfEyeClosesEye1, &ledMatrix, &tabPattern),
      crazyEye("crazyEye", sizeof(indexOfCrazyEye1), 150, indexOfCrazyEye1, &ledMatrix, &tabPattern),
      deadEye("deadEye", sizeof(indexOfDeadEye1), 150, indexOfDeadEye1, &ledMatrix, &tabPattern),
      snowEye("inLove", sizeof(indexOfSnowEye1), 150, indexOfSnowEye1, &ledMatrix, &tabPattern),
      starEye("starEye", sizeof(indexOfStarEye1), 150, indexOfStarEye1, &ledMatrix, &tabPattern),
      ALLREDEye("inLove", sizeof(indexOfALLREDEye1), 150, indexOfALLREDEye1, &ledMatrix, &tabPattern);

std::vector<EmotionSprite> emotionsTab {happy, inLove, eyeCloses, crazyEye, deadEye, snowEye, starEye, ALLREDEye};


/*==============================================================================
  GLOBAL VARIABLES
  ============================================================================*/

/* analog inputs */
int analogInputsToReport = 0; // bitwise array to store pin reporting

/* digital input ports */
byte reportPINs[TOTAL_PORTS];       // 1 = report this port, 0 = silence
byte previousPINs[TOTAL_PORTS];     // previous 8 bits sent

/* pins configuration */
byte pinConfig[TOTAL_PINS];         // configuration of every pin
byte portConfigInputs[TOTAL_PORTS]; // each bit: 1 = pin in INPUT, 0 = anything else
int pinState[TOTAL_PINS];           // any value that has been written

/* timer variables */
unsigned long currentMillis;        // store the current value from millis()
unsigned long previousMillis;       // for comparison with currentMillis
unsigned int samplingInterval = 19; // how often to run the main loop (in ms)

/* i2c data */
struct i2c_device_info {
  byte addr;
  int reg;
  byte bytes;
};

/* for i2c read continuous more */
i2c_device_info query[I2C_MAX_QUERIES];

byte i2cRxData[32];
boolean isI2CEnabled = false;
signed char queryIndex = -1;
// default delay time between i2c read request and Wire.requestFrom()
unsigned int i2cReadDelayTime = 0;

byte servoPinMap[TOTAL_PINS];

boolean isResetting = false;

/* utility functions */
void wireWrite(byte data)
{
#if ARDUINO >= 100
  Wire.write((byte)data);
#else
  Wire.send(data);
#endif
}

byte wireRead(void)
{
#if ARDUINO >= 100
  return Wire.read();
#else
  return Wire.receive();
#endif
}

/*==============================================================================
  FUNCTIONS
  ============================================================================*/

void readAndReportData(byte address, int theRegister, byte numBytes) {
  // allow I2C requests that don't require a register read
  // for example, some devices using an interrupt pin to signify new data available
  // do not always require the register read so upon interrupt you call Wire.requestFrom()
  if (theRegister != I2C_REGISTER_NOT_SPECIFIED) {
    Wire.beginTransmission(address);
    wireWrite((byte)theRegister);
    Wire.endTransmission();
    // do not set a value of 0
    if (i2cReadDelayTime > 0) {
      // delay is necessary for some devices such as WiiNunchuck
      delayMicroseconds(i2cReadDelayTime);
    }
  }
  else {
    theRegister = 0;  // fill the register with a dummy value
  }

  Wire.requestFrom(address, numBytes);  // all bytes are returned in requestFrom

  // check to be sure correct number of bytes were returned by slave
  if (numBytes < Wire.available()) {
    Firmata.sendString("I2C: Too many bytes received");
  }
  else if (numBytes > Wire.available()) {
    Firmata.sendString("I2C: Too few bytes received");
  }

  i2cRxData[0] = address;
  i2cRxData[1] = theRegister;

  for (int i = 0; i < numBytes && Wire.available(); i++) {
    i2cRxData[2 + i] = wireRead();
  }

  // send slave address, register and received bytes
  Firmata.sendSysex(SYSEX_I2C_REPLY, numBytes + 2, i2cRxData);
}

void outputPort(byte portNumber, byte portValue, byte forceSend)
{
  // pins not configured as INPUT are cleared to zeros
  portValue = portValue & portConfigInputs[portNumber];
  // only send if the value is different than previously sent
  if (forceSend || previousPINs[portNumber] != portValue) {
    Firmata.sendDigitalPort(portNumber, portValue);
    previousPINs[portNumber] = portValue;
  }
}

/* -----------------------------------------------------------------------------
  check all the active digital inputs for change of state, then add any events
  to the Serial output queue using Serial.print() */
void checkDigitalInputs(void)
{
  /* Using non-looping code allows constants to be given to readPort().
    The compiler will apply substantial optimizations if the inputs
    to readPort() are compile-time constants. */
  if (TOTAL_PORTS > 0 && reportPINs[0]) outputPort(0, readPort(0, portConfigInputs[0]), false);
  if (TOTAL_PORTS > 1 && reportPINs[1]) outputPort(1, readPort(1, portConfigInputs[1]), false);
  if (TOTAL_PORTS > 2 && reportPINs[2]) outputPort(2, readPort(2, portConfigInputs[2]), false);
  if (TOTAL_PORTS > 3 && reportPINs[3]) outputPort(3, readPort(3, portConfigInputs[3]), false);
  if (TOTAL_PORTS > 4 && reportPINs[4]) outputPort(4, readPort(4, portConfigInputs[4]), false);
  if (TOTAL_PORTS > 5 && reportPINs[5]) outputPort(5, readPort(5, portConfigInputs[5]), false);
  if (TOTAL_PORTS > 6 && reportPINs[6]) outputPort(6, readPort(6, portConfigInputs[6]), false);
  if (TOTAL_PORTS > 7 && reportPINs[7]) outputPort(7, readPort(7, portConfigInputs[7]), false);
  if (TOTAL_PORTS > 8 && reportPINs[8]) outputPort(8, readPort(8, portConfigInputs[8]), false);
  if (TOTAL_PORTS > 9 && reportPINs[9]) outputPort(9, readPort(9, portConfigInputs[9]), false);
  if (TOTAL_PORTS > 10 && reportPINs[10]) outputPort(10, readPort(10, portConfigInputs[10]), false);
  if (TOTAL_PORTS > 11 && reportPINs[11]) outputPort(11, readPort(11, portConfigInputs[11]), false);
  if (TOTAL_PORTS > 12 && reportPINs[12]) outputPort(12, readPort(12, portConfigInputs[12]), false);
  if (TOTAL_PORTS > 13 && reportPINs[13]) outputPort(13, readPort(13, portConfigInputs[13]), false);
  if (TOTAL_PORTS > 14 && reportPINs[14]) outputPort(14, readPort(14, portConfigInputs[14]), false);
  if (TOTAL_PORTS > 15 && reportPINs[15]) outputPort(15, readPort(15, portConfigInputs[15]), false);
}

// -----------------------------------------------------------------------------
/* sets the pin mode to the correct state and sets the relevant bits in the
  two bit-arrays that track Digital I/O and PWM status
*/
void setPinModeCallback(byte pin, int mode)
{
  if (pinConfig[pin] == IGNORE)
    return;

  if (pinConfig[pin] == I2C && isI2CEnabled && mode != I2C) {
    // disable i2c so pins can be used for other functions
    // the following if statements should reconfigure the pins properly
    disableI2CPins();
  }
  if (IS_PIN_ANALOG(pin)) {
    reportAnalogCallback(PIN_TO_ANALOG(pin), mode == ANALOG ? 1 : 0); // turn on/off reporting
  }
  if (IS_PIN_DIGITAL(pin)) {
    if (mode == INPUT) {
      portConfigInputs[pin / 8] |= (1 << (pin & 7));
    }
    else {
      portConfigInputs[pin / 8] &= ~(1 << (pin & 7));
    }
  }
  pinState[pin] = 0;
  switch (mode) {
    case ANALOG:
      if (IS_PIN_ANALOG(pin)) {
        if (IS_PIN_DIGITAL(pin)) {
          pinMode(PIN_TO_DIGITAL(pin), INPUT);    // disable output driver
          digitalWrite(PIN_TO_DIGITAL(pin), LOW); // disable internal pull-ups
        }
        pinConfig[pin] = ANALOG;
      }
      break;
    case INPUT:
      if (IS_PIN_DIGITAL(pin)) {
        pinMode(PIN_TO_DIGITAL(pin), INPUT);    // disable output driver
        digitalWrite(PIN_TO_DIGITAL(pin), LOW); // disable internal pull-ups
        pinConfig[pin] = INPUT;
      }
      break;
    case OUTPUT:
      if (IS_PIN_DIGITAL(pin)) {
        digitalWrite(PIN_TO_DIGITAL(pin), LOW); // disable PWM
        pinMode(PIN_TO_DIGITAL(pin), OUTPUT);
        pinConfig[pin] = OUTPUT;
      }
      break;
    case PWM:
      if (IS_PIN_PWM(pin)) {
        pinMode(PIN_TO_PWM(pin), OUTPUT);
        analogWrite(PIN_TO_PWM(pin), 0);
        pinConfig[pin] = PWM;
      }
      break;
    case I2C:
      if (IS_PIN_I2C(pin)) {
        // mark the pin as i2c
        // the user must call I2C_CONFIG to enable I2C for a device
        pinConfig[pin] = I2C;
      }
      break;
    default:
      Firmata.sendString("Unknown pin mode"); // TODO: put error msgs in EEPROM
  }
  // TODO: save status to EEPROM here, if changed
}

void analogWriteCallback(byte pin, int value)
{
  if (pin < TOTAL_PINS) {
    switch (pinConfig[pin]) {
      case PWM:
        if (IS_PIN_PWM(pin))
          analogWrite(PIN_TO_PWM(pin), value);
        pinState[pin] = value;
        break;
    }
  }
}

void digitalWriteCallback(byte port, int value)
{
  byte pin, lastPin, mask = 1, pinWriteMask = 0;

  if (port < TOTAL_PORTS) {
    // create a mask of the pins on this port that are writable.
    lastPin = port * 8 + 8;
    if (lastPin > TOTAL_PINS) lastPin = TOTAL_PINS;
    for (pin = port * 8; pin < lastPin; pin++) {
      // do not disturb non-digital pins (eg, Rx & Tx)
      if (IS_PIN_DIGITAL(pin)) {
        // only write to OUTPUT and INPUT (enables pullup)
        // do not touch pins in PWM, ANALOG, SERVO or other modes
        if (pinConfig[pin] == OUTPUT || pinConfig[pin] == INPUT) {
          pinWriteMask |= mask;
          pinState[pin] = ((byte)value & mask) ? 1 : 0;
        }
      }
      mask = mask << 1;
    }
    writePort(port, (byte)value, pinWriteMask);
  }
}

// -----------------------------------------------------------------------------
/* sets bits in a bit array (int) to toggle the reporting of the analogIns
*/
//void FirmataClass::setAnalogPinReporting(byte pin, byte state) {
//}
void reportAnalogCallback(byte analogPin, int value)
{
  if (analogPin < TOTAL_ANALOG_PINS) {
    if (value == 0) {
      analogInputsToReport = analogInputsToReport & ~(1 << analogPin);
    }
    else {
      analogInputsToReport = analogInputsToReport | (1 << analogPin);
      // prevent during system reset or all analog pin values will be reported
      // which may report noise for unconnected analog pins
      if (!isResetting) {
        // Send pin value immediately. This is helpful when connected via
        // ethernet, wi-fi or bluetooth so pin states can be known upon
        // reconnecting.
        Firmata.sendAnalog(analogPin, analogRead(analogPin));
      }
    }
  }
  // TODO: save status to EEPROM here, if changed
}

void reportDigitalCallback(byte port, int value)
{
  if (port < TOTAL_PORTS) {
    reportPINs[port] = (byte)value;
    // Send port value immediately. This is helpful when connected via
    // ethernet, wi-fi or bluetooth so pin states can be known upon
    // reconnecting.
    if (value) outputPort(port, readPort(port, portConfigInputs[port]), true);
  }
  // do not disable analog reporting on these 8 pins, to allow some
  // pins used for digital, others analog.  Instead, allow both types
  // of reporting to be enabled, but check if the pin is configured
  // as analog when sampling the analog inputs.  Likewise, while
  // scanning digital pins, portConfigInputs will mask off values from any
  // pins configured as analog
}

/*==============================================================================
  SYSEX-BASED commands
  ============================================================================*/

void sysexCallback(byte command, byte argc, byte *argv)
{
  byte mode;
  byte slaveAddress;
  byte data;
  int slaveRegister;
  unsigned int delayTime;

  switch (command) {
    case I2C_REQUEST:
      mode = argv[1] & I2C_READ_WRITE_MODE_MASK;
      if (argv[1] & I2C_10BIT_ADDRESS_MODE_MASK) {
        Firmata.sendString("10-bit addressing not supported");
        return;
      }
      else {
        slaveAddress = argv[0];
      }

      switch (mode) {
        case I2C_WRITE:
          Wire.beginTransmission(slaveAddress);
          for (byte i = 2; i < argc; i += 2) {
            data = argv[i] + (argv[i + 1] << 7);
            wireWrite(data);
          }
          Wire.endTransmission();
          delayMicroseconds(70);
          break;
        case I2C_READ:
          if (argc == 6) {
            // a slave register is specified
            slaveRegister = argv[2] + (argv[3] << 7);
            data = argv[4] + (argv[5] << 7);  // bytes to read
          }
          else {
            // a slave register is NOT specified
            slaveRegister = I2C_REGISTER_NOT_SPECIFIED;
            data = argv[2] + (argv[3] << 7);  // bytes to read
          }
          readAndReportData(slaveAddress, (int)slaveRegister, data);
          break;
        case I2C_READ_CONTINUOUSLY:
          if ((queryIndex + 1) >= I2C_MAX_QUERIES) {
            // too many queries, just ignore
            Firmata.sendString("too many queries");
            break;
          }
          if (argc == 6) {
            // a slave register is specified
            slaveRegister = argv[2] + (argv[3] << 7);
            data = argv[4] + (argv[5] << 7);  // bytes to read
          }
          else {
            // a slave register is NOT specified
            slaveRegister = (int)I2C_REGISTER_NOT_SPECIFIED;
            data = argv[2] + (argv[3] << 7);  // bytes to read
          }
          queryIndex++;
          query[queryIndex].addr = slaveAddress;
          query[queryIndex].reg = slaveRegister;
          query[queryIndex].bytes = data;
          break;
        case I2C_STOP_READING:
          byte queryIndexToSkip;
          // if read continuous mode is enabled for only 1 i2c device, disable
          // read continuous reporting for that device
          if (queryIndex <= 0) {
            queryIndex = -1;
          }
          else {
            // if read continuous mode is enabled for multiple devices,
            // determine which device to stop reading and remove it's data from
            // the array, shifiting other array data to fill the space
            for (byte i = 0; i < queryIndex + 1; i++) {
              if (query[i].addr == slaveAddress) {
                queryIndexToSkip = i;
                break;
              }
            }

            for (byte i = queryIndexToSkip; i < queryIndex + 1; i++) {
              if (i < I2C_MAX_QUERIES) {
                query[i].addr = query[i + 1].addr;
                query[i].reg = query[i + 1].reg;
                query[i].bytes = query[i + 1].bytes;
              }
            }
            queryIndex--;
          }
          break;
        default:
          break;
      }
      break;
    case I2C_CONFIG:
      delayTime = (argv[0] + (argv[1] << 7));

      if (delayTime > 0) {
        i2cReadDelayTime = delayTime;
      }

      if (!isI2CEnabled) {
        enableI2CPins();
      }

      break;
    case SAMPLING_INTERVAL:
      if (argc > 1) {
        samplingInterval = argv[0] + (argv[1] << 7);
        if (samplingInterval < MINIMUM_SAMPLING_INTERVAL) {
          samplingInterval = MINIMUM_SAMPLING_INTERVAL;
        }
      }
      else {
        //Firmata.sendString("Not enough data");
      }
      break;
    case EXTENDED_ANALOG:
      if (argc > 1) {
        int val = argv[1];
        if (argc > 2) val |= (argv[2] << 7);
        if (argc > 3) val |= (argv[3] << 14);
        analogWriteCallback(argv[0], val);
      }
      break;
    case CAPABILITY_QUERY:
      Firmata.write(START_SYSEX);
      Firmata.write(CAPABILITY_RESPONSE);
      for (byte pin = 0; pin < TOTAL_PINS; pin++) {
        if (IS_PIN_DIGITAL(pin)) {
          Firmata.write((byte)INPUT);
          Firmata.write(1);
          Firmata.write((byte)OUTPUT);
          Firmata.write(1);
        }
        if (IS_PIN_ANALOG(pin)) {
          Firmata.write(ANALOG);
          Firmata.write(10); // 10 = 10-bit resolution
        }
        if (IS_PIN_PWM(pin)) {
          Firmata.write(PWM);
          Firmata.write(8); // 8 = 8-bit resolution
        }
        if (IS_PIN_DIGITAL(pin)) {
          Firmata.write(SERVO);
          Firmata.write(14);
        }
        if (IS_PIN_I2C(pin)) {
          Firmata.write(I2C);
          Firmata.write(1);  // TODO: could assign a number to map to SCL or SDA
        }
        Firmata.write(127);
      }
      Firmata.write(END_SYSEX);
      break;
    case PIN_STATE_QUERY:
      if (argc > 0) {
        byte pin = argv[0];
        Firmata.write(START_SYSEX);
        Firmata.write(PIN_STATE_RESPONSE);
        Firmata.write(pin);
        if (pin < TOTAL_PINS) {
          Firmata.write((byte)pinConfig[pin]);
          Firmata.write((byte)pinState[pin] & 0x7F);
          if (pinState[pin] & 0xFF80) Firmata.write((byte)(pinState[pin] >> 7) & 0x7F);
          if (pinState[pin] & 0xC000) Firmata.write((byte)(pinState[pin] >> 14) & 0x7F);
        }
        Firmata.write(END_SYSEX);
      }
      break;
    case ANALOG_MAPPING_QUERY:
      Firmata.write(START_SYSEX);
      Firmata.write(ANALOG_MAPPING_RESPONSE);
      for (byte pin = 0; pin < TOTAL_PINS; pin++) {
        Firmata.write(IS_PIN_ANALOG(pin) ? PIN_TO_ANALOG(pin) : 127);
      }
      Firmata.write(END_SYSEX);
      break;

    /*==============================================================================
      SCRATCH commands
      ============================================================================*/

    case MOTOR:
      switch (argv[0]) {
        case 1:
          switch (argv[1]) {
            case 1: motor_l.move(argv[2], argv[3]);
              break;
            case 2: motor_r.move(argv[2], argv[3]);
              break;
            case 3: motor_r.move(argv[2], argv[3]);
              motor_l.move(argv[2], argv[3]);
              break;
            default:
              break;
          }
          break;
        case 2:
          switch (argv[1]) {
            case 1: motor_l.stop();
              break;
            case 2: motor_r.stop();
              break;
            case 3: motor_r.stop();
              motor_l.stop();
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }

      break;
    case LED:
      switch (argv[0]) {
        case 1: frontLeds.setColorUnit(argv[1], argv[2], argv[3], argv[4]);
          break;
        case 2 :
          backLeds.setColorAll(argv[1], argv[2], argv[3]);
          frontLeds.setColorAll(argv[1], argv[2], argv[3]);
          break;
        case 3 :
          backLeds.setColor(argv[1]);
          frontLeds.setColor(argv[1]);
          break;
        case 4:
          backLeds.ledOnOff(argv[1]);
          frontLeds.ledOnOff(argv[1]);
          break;
        default:
          backLeds.ledOnOff(0);
          frontLeds.ledOnOff(0);
          break;
      }

      break;
    case BUZZER:

      switch (argv[0]) {
        case 1: speaker.playSon(argv[1]);
          break;
        case 2: speaker.playNote(argv[1], argv[2]);
          break;
        case 3:
          break;
        case 4: speaker.playSonDelay(argv[1], argv[2]);
          break;
        case 5: speaker.setDelayRythme(argv[1]);
          break;
        case 6: speaker.setDelayAttente(argv[1]);
          break;
        case 7: speaker.playMelody(argv[1]);
          break;
        case 8: speaker.buzzerOnOff(argv[1]);
          break;
        case 9: speaker.buzzerOnOffDelay(argv[1], argv[2]);
          break;
        default:
          break;
      }
      break;
    case EMOTIONS:

      switch (argv[0]) {
        case 1: happy.printSprite(argv[1]);
          happy.clear();
          break;
        case 2: inLove.printSprite(argv[1]);
          inLove.clear();
          break;
        case 3: crazyEye.printSprite(argv[1]);
          crazyEye.clear();
          break;
        case 4: deadEye.printSprite(argv[1]);
          deadEye.clear();
          break;
        case 5: snowEye.printSprite(argv[1]);
          snowEye.clear();
          break;
        case 6: starEye.printSprite(argv[1]);
          starEye.clear();
          break;
        default:
          break;
      }
      break;
  }
}



void enableI2CPins()
{
  byte i;
  // is there a faster way to do this? would probaby require importing
  // Arduino.h to get SCL and SDA pins
  for (i = 0; i < TOTAL_PINS; i++) {
    if (IS_PIN_I2C(i)) {
      // mark pins as i2c so they are ignore in non i2c data requests
      //	setPinModeCallback(i, I2C);
    }
  }

  isI2CEnabled = true;

  Wire.begin();
}

/* disable the i2c pins so they can be used for other functions */
void disableI2CPins() {
  isI2CEnabled = false;
  // disable read continuous mode for all devices
  queryIndex = -1;
}

/*==============================================================================
  SETUP()
  ============================================================================*/

void systemResetCallback()
{
  isResetting = true;

  // initialize a defalt state
  // TODO: option to load config from EEPROM instead of default

  if (isI2CEnabled) {
    disableI2CPins();
  }

  for (byte i = 0; i < TOTAL_PORTS; i++) {
    reportPINs[i] = false;    // by default, reporting off
    portConfigInputs[i] = 0;  // until activated
    previousPINs[i] = 0;
  }

  for (byte i = 0; i < TOTAL_PINS; i++) {
    // pins with analog capability default to analog input
    // otherwise, pins default to digital output
    if (IS_PIN_ANALOG(i)) {
      // turns off pullup, configures everything
      //	setPinModeCallback(i, ANALOG);
    }
    else {
      // sets the output to 0, configures portConfigInputs
      //	setPinModeCallback(i, OUTPUT);
    }

    servoPinMap[i] = 255;
  }
  // by default, do not report any analog inputs
  analogInputsToReport = 0;

  /* send digital inputs to set the initial state on the host computer,
    since once in the loop(), this firmware will only send on change */
  /*
    TODO: this can never execute, since no pins default to digital input
    but it will be needed when/if we support EEPROM stored config
    for (byte i=0; i < TOTAL_PORTS; i++) {
    outputPort(i, readPort(i, portConfigInputs[i]), true);
    }
  */
  isResetting = false;
}

void docount_1(int* encoder1Pos)  // counts from the speed sensor of Motor 1 (left)
{
  if (digitalRead(9) == HIGH && digitalRead(8) == LOW) {
    robot.setEncoder1PosDecrease(); // encoder1Pos-- ;  // decrease -1 the counter value
  }
  else if (digitalRead(9) == LOW && digitalRead(8) == HIGH) {
    robot.setEncoder1PosIncrease(); // encoder1Pos++ ;  // increase +1 the counter value
  }
  robot.setEncoder1PosIncrease();
}


void docount_2(int* encoder2Pos)  // counts from the speed sensor of Motor 2 (right)
{
  if (digitalRead(7) == HIGH && digitalRead(6) == LOW) {
    robot.setEncoder2PosIncrease(); //encoder2Pos++ ;
  }
  else if (digitalRead(7) == LOW && digitalRead(6) == HIGH) {
    robot.setEncoder2PosDecrease();  //encoder2Pos-- ;
  }
}

void setup() {
  attachInterrupt(5, docount_1, RISING);
  attachInterrupt(4, docount_2, RISING);

  //------motor init-------://
  motor_l.init();
  motor_r.init();

  //------LEDs init-------://
  frontLeds.init();
  backLeds.init();

  //------Add component to robot-------://
  controls.addButton(btn_right);
  controls.addButton(btn_left);
  controls.addButton(btn_down);
  controls.addButton(btn_up);
  controls.addButton(btn_valid);

  robot.addMotor(&motor_l);
  robot.addMotor(&motor_r);


  robot.addSensor(&sensorAvG);
  robot.addSensor(&sensorAvM);
  robot.addSensor(&sensorAvD);

  robot.addSensor(&sensorLineLeft);
  robot.addSensor(&sensorLineRight);



  robot.addLed(&frontLeds);
  robot.addLed(&backLeds);
  robot.setControls(&controls);


  //------turning bot-------://
  /*	backLeds.setColorAll(255, 255, 255);
  	frontLeds.setColorAll(255, 255, 255); */
  //-------Son de bienvenu----://
  //speaker_main.PlayMelody(WELCOMSONG);

  Firmata.setFirmwareVersion(FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);

  // Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
  // Firmata.attach(DIGITAL_MESSAGE, digitalWriteCallback);
  // Firmata.attach(REPORT_ANALOG, reportAnalogCallback);
  // Firmata.attach(REPORT_DIGITAL, reportDigitalCallback);
  // Firmata.attach(SET_PIN_MODE, setPinModeCallback);
  Firmata.attach(START_SYSEX, sysexCallback);
  // Firmata.attach(SYSTEM_RESET, systemResetCallback);

  // to use a port other than Serial, such as Serial1 on an Arduino Leonardo or Mega,
  // Call begin(baud) on the alternate serial port and pass it to Firmata to begin like this:
  // Serial1.begin(57600);
  // Firmata.begin(Serial1);
  // then comment out or remove lines 701 - 704 below



  Firmata.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Only needed for ATmega32u4-based boards (Leonardo, etc).
  }
  systemResetCallback();  // reset to default config


  /*

  	Serial.begin(115200);
  	while (!Serial) {
  		// wait for serial port to connect. Only needed for ATmega32u4-based boards (Leonardo, etc).
  	}
  	digitalWrite(13, HIGH);
  */
  int e = 0;
  //initiation of the max 7219
  ledMatrix.maxAll(max7219_reg_scanLimit, 0x07);
  ledMatrix.maxAll(max7219_reg_decodeMode, 0x00);  // using a led matrix (not digits)
  ledMatrix.maxAll(max7219_reg_shutdown, 0x01);    // not in shutdown mode
  ledMatrix.maxAll(max7219_reg_displayTest, 0x00); // no display test
  for (e = 1; e <= 8; e++) {    // empty registers, turn all LEDs off
    ledMatrix.maxAll(e, 0);
  }
  ledMatrix.maxAll(max7219_reg_intensity, 0x0f & 0x0f);    // the first 0x0f is the value you can set
  // range: 0x00 to 0x0f


  pinMode(24, INPUT);
  pinMode(22, INPUT);
  pinMode(23, INPUT);
  pinMode(25, INPUT);
  pinMode(26, INPUT);
}


/*==============================================================================
  LOOP()
  ============================================================================*/

void loop() {
  /*
    //digitalWrite(7,HIGH);
    //digitalWrite(6,LOW);
    //analogWrite(5,200);

    delay(1000);

    //digitalWrite(7,HIGH);
    //digitalWrite(6,LOW);
    //analogWrite(5,0);

    delay(1000);

    motor_r.setDirection(true);
    motor_r.setSpeed(255);

    delay(1000);

    motor_r.setSpeed(0);

    delay(1000);

    //motor_l.setDirection(true);
    //motor_l.setSpeed(200);
    //Serial.println(motor_l.getSpeed());
  */
  // put your main code here, to run repeatedly:

  //------Partie de CrashTEST------//

  byte pin, analogPin;

  // checkDigitalInputs();

  while (Firmata.available())
    Firmata.processInput();

  menu.runMenu(&robot, &controls, &frontLeds, &backLeds, &speaker);

  // TODO - ensure that Stream buffer doesn't go over 60 bytes

  currentMillis = millis();
  if (currentMillis - previousMillis > samplingInterval) {
    previousMillis += samplingInterval;
    for (pin = 0; pin < TOTAL_PINS; pin++) {
      if (IS_PIN_ANALOG(pin) && pinConfig[pin] == ANALOG) {
        analogPin = PIN_TO_ANALOG(pin);
        if (analogInputsToReport & (1 << analogPin)) {
          Firmata.sendAnalog(analogPin, analogRead(analogPin));
        }
      }
    }
    // report i2c data for all device with read continuous mode enabled
    if (queryIndex > -1) {
      for (byte i = 0; i < queryIndex + 1; i++) {
        readAndReportData(query[i].addr, query[i].reg, query[i].bytes);
      }
    }
  }

}
