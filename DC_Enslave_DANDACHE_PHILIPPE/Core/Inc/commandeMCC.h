#include "main.h"

#define encodeurDefinition 1024
#define Mid_Period_TIM2 2.147483647E9
#define CURRENT_MAX_VALUE	1

#define TIM4_PERIOD 0.0000625


void Swing();
void Init_Onduleur();
void SetAlpha(char * argv);
void SetCurrent(char * argv);
float GetCurrent();
void ReadEncodeur();
void ReadSpeed();
void PrintData();
