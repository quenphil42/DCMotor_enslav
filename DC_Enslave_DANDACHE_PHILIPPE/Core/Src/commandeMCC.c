/**
  ******************************************************************************
  * @file    commandeMCC.c
  * @brief   Ce fichier contient le code en rapport avec l'utilisation de l'onduleur et la commande de la MCC
  ******************************************************************************
  *  Created on: Dec 8, 2022
  *  Author: quent
  *
  ******************************************************************************
  */

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

/**
 *  @brief Fonction bloquante qui permet de modifier la PWM du moteur de manière sinusoidale sur trois période
 *
 *  @param None
 *
 *  @retval None
 */
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

/**
 *
 * @brief Fonction qui permet de fixer la valeur du rapport alpha de la PWM depuis le shell
 *
 * @param argv
 *
 * @retval None
 */
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

/**
 * @brief Fonction qui permet de fixer la valeur du rapport alpha de la PWM
 *
 * @param alpha (entier entre 0 et 100)
 *
 * @retval None
 */
void setPWM(int alpha)
{
	int CCR_value = alpha*ARR_MAX_VALUE/ALPHA_MAX;
	TIM1->CCR1 = CCR_value;
	TIM1->CCR2 = ARR_MAX_VALUE - 1 - CCR_value;
}

/**
 * @brief Fonction qui permet de fixer la consigne de courant (en A) depuis le shell pour eprouver l'asservissement en courant
 *
 * @param argv
 *
 * @retval None
 */
void SetCurrent(char * argv)
{
	i_consigne = (float)strtod(argv,NULL); //converti un char* 2 float
}

/**
 * @brief Fonction qui permet de fixer la consigne de vitesse (en tr/min) depuis le shell pour eprouver l'asservissement en vitesse
 *
 * @param argv
 *
 * @retval None
 */
void SetSpeed(char * argv)
{
	v_consigne = (float)strtod(argv,NULL); //converti un char* 2 float
}


/**
 *  @brief Fonction bloquante qui permet d'initialiser l'onduleur et reinitialiser les mémoires des asservissements de vitesse et courant
 *
 *  @param None
 *
 *  @retval None
 */
void Init_Onduleur()
{

	TIM1->CCR1 = ARR_MAX_VALUE/2;
	TIM1->CCR2 = ARR_MAX_VALUE - 1 - ARR_MAX_VALUE/2;
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin,SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin,RESET);

	alphaPI.prevError = ALPHA_MEM_P;		//reset les valeurs initiales afin de ne pas integrer d'erreur avant l'init et provoquer un overcurrent
	alphaPI.integrator = ALPHA_MEM_I;
	currentPI.prevError = CURRENT_MEM_P;
	currentPI.integrator = CURRENT_MEM_I;
}

/**
 * @brief Fonction qui permet d'obtenir la valeur du courant (en A) au sein de la MCC depuis le Buffer "adcBuffer" où le DMA stock periodiquement la valeur lue par l'ADC
 *
 * @param None
 *
 * @return courant (en A)
 */
float GetCurrent()
{
	return -((float)(adcBuffer[0])-3137) * 3.3 * GAIN_COURANT_HACHEUR / ADC_MAX_VALUE;
}

/**
 * @brief Foction qui permet d'obtenir la valeur de la vitesse (en tr/min) de la MCC en tenant compte de la periode d'echantillonage
 *
 * @param timercoef (ratio entre la variation de tick et la periode d'echantillonage)
 *
 * @return vitesse en tr/min
 */
float GetSpeed(int timercoef)
{
	return -(float)(timercoef * speed);
}

/**
 * @brief Fonction qui met à jour la variable global "angle" en lui associant la valeur du compteur du Timer 2
 *
 * @param None
 *
 * @retval None
 */
void ReadEncodeur()
{
	angle = ((TIM2->CNT));
}

/**
 * @brief Fonction qui met à jour la variable global "speed" en lui associant la variation d'increment issue du codeur incremental depuis le dernier appel de la fonction
 *
 * @param None
 *
 * @retval None
 */
void ReadSpeed()
{
	speed = ((TIM2->CNT))- Mid_Period_TIM2;
	TIM2->CNT = Mid_Period_TIM2;
}

/**
 * @brief Fonction qui imprime sur le shell la valeur du CNT de Tim2, de la vitesse en tr/min et du courant mesuré en A
 *
 * @param None
 *
 * @retval None
 */
void PrintData()
{
	uint8_t MSG[CMD_BUFFER_SIZE] = {'\0'};

	sprintf((char *)MSG, "Encoder Ticks = %d\n\r", angle);
	HAL_UART_Transmit(&huart2, MSG, sizeof(MSG), 100);

	sprintf((char *)MSG, "Speed = %f tick/periode\n\r", -TICK2SPEED_TIM4 * speed);
	HAL_UART_Transmit(&huart2, MSG, sizeof(MSG), 100);

	sprintf( (char *)MSG, "current = %1.3f \r\n",-((float)(adcBuffer[0])-3137) * 3.3 * GAIN_COURANT_HACHEUR / ADC_MAX_VALUE);
	HAL_UART_Transmit(&huart2, MSG, sizeof(MSG), HAL_MAX_DELAY);
}
