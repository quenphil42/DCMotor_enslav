#include "main.h"

#define encodeurDefinition 1024
#define Mid_Period_TIM2 2.147483647E9


void Swing();
void Init_Onduleur();
void SetAlpha(char * argv);
void setPWM(int alpha);
void SetCurrent(char * argv);
float GetCurrent();
void ReadEncodeur();
void ReadSpeed();
void PrintData();
