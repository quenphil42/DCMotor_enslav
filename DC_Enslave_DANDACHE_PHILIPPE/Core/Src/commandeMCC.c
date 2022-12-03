#include "commandeMCC.h"
#include "usart.h"

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
	return 0;
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
