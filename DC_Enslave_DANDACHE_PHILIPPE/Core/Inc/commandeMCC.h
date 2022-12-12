#include "main.h"

#define encodeurDefinition 1024
#define Mid_Period_TIM2 2.147483647E9
#define TICK2SPEED 0.0153

void Swing();
void Init_Onduleur();
void SetAlpha(char * argv);
void setPWM(int alpha);
void SetCurrent(char * argv);
void SetSpeed(char * argv);
float GetCurrent();
void ReadEncodeur();
void ReadSpeed();
void PrintData();
