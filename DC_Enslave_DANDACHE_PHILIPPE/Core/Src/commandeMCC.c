#include "commandeMCC.h"
#include "usart.h"
#include "shell2.h"

extern uint32_t adcBuffer[1];
extern int angle;
extern int speed;


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

	if (alpha>=0 && alpha<=100)
	{
		int CCR_value = alpha*ARR_MAX_VALUE/100;
		TIM1->CCR1 = CCR_value;
		TIM1->CCR2 = ARR_MAX_VALUE - 1 - CCR_value;
	}
	else
	{
		uint8_t text[]="Error : alpha 0-100";
		HAL_UART_Transmit(&huart2, text, sizeof(text), HAL_MAX_DELAY);
	}

}


void Init_Onduleur()
{
	TIM1->CCR1 = ARR_MAX_VALUE/2;
	TIM1->CCR2 = ARR_MAX_VALUE - 1 - ARR_MAX_VALUE/2;
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin,SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin,RESET);
}

float GetCurrent()
{
	return (3137-(float)(adcBuffer[0]))*3.3*12/4096;
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

	sprintf((char *)MSG, "Speed = %d incr/s\n\r", (speed));
	HAL_UART_Transmit(&huart2, MSG, sizeof(MSG), 100);

	sprintf( (char *)MSG, "current = %1.3f \r\n",(3137-(float)(adcBuffer[0]))*3.3*12/4096);
	HAL_UART_Transmit(&huart2, MSG, sizeof(MSG), HAL_MAX_DELAY);
}
