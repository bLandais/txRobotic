/*
   File:   Led.h
*/

#ifndef LED_H
#define LED_H


#include <Adafruit_NeoPixel.h>
#include <StandardCplusplus.h>
#include <vector>

class Led {

  private: int pin;

  public:
    int number;
    bool state;
    bool blink;
    std::vector<int> color {r, v, b};
    Adafruit_NeoPixel strip;
    int r;
    int v;
    int b;
    const int tabColor[10][3] = {
      { 255, 255, 255 },
      { 255, 0, 0 },
      { 0, 255, 0 },
      { 0, 0, 255 },
      { 37, 253, 233 },
      { 223, 109, 20 },
      { 96, 96, 96 },
      { 255, 255, 0 },
      { 255, 0, 255 },
      { 102, 0, 153 }
    };
    //TODO: classe color ac nom, code rvb convertisseur hexa


  public:
    Led(int, int);
    bool init();
    std::vector<int> getColor();
    void show(int, int, int, int);
    void setColorAll(int = 0, int = 0, int = 0);
    void setColorUnit(int = 5, int = 0, int = 0, int = 0);
    void setColor(int couleur);
    void ledOnOff(bool state);
};

#endif /* LED_H */

