#ifndef Data_h
#define Data_h
#include <SoftwareSerial.h>

class Data
{
  public:
    int a; // Angle
    int p; // Puissance
    int memory = 0;
  public:
    Data();
    Data(int, int);
    void setData(int, int);
    void setA(int);
    void setP(int);
    int getA();
    int getP();
    int analyze();

};

#endif
