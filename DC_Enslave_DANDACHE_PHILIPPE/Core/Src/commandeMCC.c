#include "commandeMCC.h"
#include "usart.h"
#include "shell2.h"
#include "PI.h"
#include "main.h"

extern uint32_t adcBuffer[1];
extern int angle;
extern int speed;
extern float i_consigne;
extern float v_consigne;
extern PIController alphaPI;
extern PIController currentPI;

void Swing()
{
	for(int j=0; j<3; j++)
	{
		for (int i = ARR_MAX_VALUE/2; i<ARR_MAX_VALUE; i++)
		{
			TIM1->CCR1 = i;
			TIM1->CCR2 = ARR_MAX_VALUE-1-i;
			HAL_Delay(5);
		}
		for (int i = 0; i<ARR_MAX_VALUE; i++)
		{
			TIM1->CCR1 = ARR_MAX_VALUE-1-i;
			TIM1->CCR2 = i;
			HAL_Delay(5);
		}
		for (int i = 0; i<ARR_MAX_VALUE/2; i++)
		{
			TIM1->CCR1 = i;
			TIM1->CCR2 = ARR_MAX_VALUE-1-i;
			HAL_Delay(5);
		}
	}
}

void SetAlpha(char * argv)
{
	int alpha = atoi(argv);

	if (alpha>=ALPHA_MIN && alpha<=ALPHA_MAX)
	{
		setPWM(alpha);
	}
	else
	{
		uint8_t text[]="Error : alpha 0-100";
		HAL_UART_Transmit(&huart2, text, sizeof(text), HAL_MAX_DELAY);
	}

}

void setPWM(int alpha)
{
	int CCR_value = alpha*ARR_MAX_VALUE/ALPHA_MAX;
	TIM1->CCR1 = CCR_value;
	TIM1->CCR2 = ARR_MAX_VALUE - 1 - CCR_value;
}

void SetCurrent(char * argv)
{
	i_consigne = (float)strtod(argv,NULL); //converti un char* 2 float
}

void SetSpeed(char * argv)
{
	v_consigne = (float)strtod(argv,NULL); //converti un char* 2 float
}


void Init_Onduleur()
{

	TIM1->CCR1 = ARR_MAX_VALUE/2;
	TIM1->CCR2 = ARR_MAX_VALUE - 1 - ARR_MAX_VALUE/2;
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin,SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin,RESET);

	alphaPI.prevError = 0.0;		//reset les valeurs initiales afin de ne pas integrer d'erreur avant l'init et provoquer un overcurrent
	alphaPI.integrator = 0.5;
	currentPI.prevError = 0.0;
	currentPI.integrator = 0.0;
}

float GetCurrent()
{
	return -((float)(adcBuffer[0])-3137) * 3.3 * GAIN_COURANT_HACHEUR / ADC_MAX_VALUE;
}

float GetSpeed()
{
	return -(float)(TICK2SPEED * speed);
}

void ReadEncodeur()
{
	angle = ((TIM2->CNT));
}

void ReadSpeed()
{
	speed = ((TIM2->CNT))- Mid_Period_TIM2;
	TIM2->CNT = Mid_Period_TIM2;
}


void PrintData()
{
	uint8_t MSG[CMD_BUFFER_SIZE] = {'\0'};

	sprintf((char *)MSG, "Encoder Ticks = %d\n\r", angle);
	HAL_UART_Transmit(&huart2, MSG, sizeof(MSG), 100);

	sprintf((char *)MSG, "Speed = %f tr/min\n\r", -TICK2SPEED * speed);
	HAL_UART_Transmit(&huart2, MSG, sizeof(MSG), 100);

	sprintf( (char *)MSG, "current = %1.3f \r\n",-((float)(adcBuffer[0])-3137) * 3.3 * GAIN_COURANT_HACHEUR / ADC_MAX_VALUE);
	HAL_UART_Transmit(&huart2, MSG, sizeof(MSG), HAL_MAX_DELAY);
}
