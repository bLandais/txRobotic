// EmotionSprite.h

#ifndef _EMOTIONSPRITE_h
#define _EMOTIONSPRITE_h

#include <vector>
#include <StandardCplusplus.h>

#include "LedMatrix.h"
#include "TabPattern.h"

class EmotionSprite  {

  public:

    //	VARIABLES
    String name;	//	le nom de l emotion
    int pattern_number;	//	le nombre de pattern qu il contiendra
    int pattern_delay;	//	le temps d attente entre 2 pattern
    LedMatrix *ledMatrix;
    TabPattern *tabPattern;
    std::vector<int> emotion_array_eye;	//	un tableau contenant les indices du pattern

    //	CONSTRUCTOR
    EmotionSprite(String name, int pattern_number, int pattern_delay, std::vector<int> index, LedMatrix * ledMatrix, TabPattern * tabPattern);

    // SETTER
    void SetName(String name);
    void SetPattern_number(int pattern_number);
    void SetPattern_delay(int pattern_delay);
    void SetEmotion_array_eye(int indexOfEmotion[], int sizeOfArray);

    //	FUNCTIONS
    int printPattern(int partFace, int wichMatrix);
    void printSprite(int wichMatrix);
    void clear();
    
};
#endif

