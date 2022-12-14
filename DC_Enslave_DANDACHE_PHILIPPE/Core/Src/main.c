/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "PI.h"
#include "shell2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commandeMCC.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

int it_button = 0;
int adcCbck = 0;
int it_tim1 = 0;
int it_tim3 = 0;
int it_tim4 = 0;
int speed = 0;
int angle = 0;

PIController alphaPI;
float i_consigne = 0.0;

PIController currentPI;
float v_consigne = 0.0;

extern uint8_t uartRxReceived;
extern uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
extern uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];

uint32_t adcBuffer[1] = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int enPrint = 0;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 - Shell*/

  //Commande PWM Init
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

  //UART Init
  HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);

  //SHELL Init
  HAL_Delay(1);
  shellInit();

  //ADC Init
  HAL_ADCEx_Calibration_Start (&hadc1, ADC_SINGLE_ENDED);
  HAL_ADC_Start_DMA(&hadc1, adcBuffer, 1);

  //TIMERs Init
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

  HAL_TIM_Base_Start_IT(&htim3);

  HAL_TIM_Base_Start_IT(&htim4);

  //Asserv courant Init
  PIController_Init(&alphaPI);

  alphaPI.Kp = KP_ALPHA;
  alphaPI.Ki = KI_ALPHA;

  alphaPI.T = TIM1_PERIOD;

  alphaPI.integrator = ALPHA_MEM_I;

  alphaPI.limMax_integrator = ALPHA_OUT_MAX_VALUE;
  alphaPI.limMin_integrator = ALPHA_OUT_MIN_VALUE;

  alphaPI.limMax_output = ALPHA_OUT_MAX_VALUE;
  alphaPI.limMin_output = ALPHA_OUT_MIN_VALUE;

  //Asserv vitesse Init

  PIController_Init(&currentPI);

  currentPI.Kp = KP_CURRENT;
  currentPI.Ki = KI_CURRENT;

  currentPI.T = TIM4_PERIOD;

  currentPI.limMax_integrator = CURRENT_OUT_MAX_VALUE;
  currentPI.limMin_integrator = -CURRENT_OUT_MAX_VALUE;

  currentPI.limMax_output = CURRENT_OUT_MAX_VALUE;
  currentPI.limMin_output = -CURRENT_OUT_MAX_VALUE;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
// SuperLoop inside the while(1), only flag changed from interrupt could launch functions
 		if(uartRxReceived)
 		{
  			if(shellGetChar())
  			{
  				shellExec();
  				shellPrompt();
  			}
  			uartRxReceived = 0;
  		}

 		if(it_button==1)
 		{
 			//uint8_t text[]="interruption \r\nquali>";
 			//HAL_UART_Transmit(&huart2, text, sizeof(text), HAL_MAX_DELAY);
 			//Init_Onduleur();

 			if (enPrint) enPrint = 0;
 			else enPrint = 1;

 			it_button = 0;
 		}

 		if(adcCbck == 1)
 		{
 			adcCbck = 0;
 		}

 		if(it_tim1)
 		{

 			PIController_Update(&alphaPI, i_consigne, GetCurrent());

 			setPWM((int)(alphaPI.out*100));

 			it_tim1 = 0;
 		}

 		if(it_tim3)
 		{



			if(enPrint)
				{
					/*ReadEncodeur();
					ReadSpeed();
					PrintData();
					uint8_t MSG[CMD_BUFFER_SIZE] = {'\0'};
					sprintf((char *)MSG, "alphaPI = %f\n\r", alphaPI.out);
					HAL_UART_Transmit(&huart2, MSG, sizeof(MSG), 100);*/

					uint8_t MSG[CMD_BUFFER_SIZE] = {'\0'};

					sprintf((char *)MSG, "Speed = %f tick/periode\n\r", GetSpeed(TICK2SPEED_TIM4));
					HAL_UART_Transmit(&huart2, MSG, sizeof(MSG), 100);
				}


 			it_tim3 = 0;
 		}

 		if(it_tim4)
 		{

 			ReadEncodeur();
 			ReadSpeed();

 			PIController_Update(&currentPI, v_consigne, GetSpeed(TICK2SPEED_TIM4));

 			i_consigne = currentPI.out;

 			it_tim4 = 0;

 		}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
 * @brief Fonction appel??e ?? la fin de la conversion de l'ADC
 *
 * @param hadc
 *
 * @retval None
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	adcCbck = 1;
}

/**
 * @brief Fonction appel??e lors du Callback des Timers
 *
 * @param htim
 *
 * @retval
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM6) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */
	if (htim->Instance == TIM3)
	{
		it_tim3 = 1;
	}
	if (htim->Instance == TIM4)
	{
		it_tim4 = 1;
	}
	if (htim->Instance == TIM1)
	{
		it_tim1 = 1;
	}
	/* USER CODE END Callback 1 */
}

/**
 * @brief Fonction qui permet d'utiliser le printf avec l'USART2
 *
 * @param ch
 *
 * @return ch
 */
int __io_putchar(int ch) //COM Serial via printf
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
}

/**
 * @brief Fonction appel??e lors d'une interruption EXTI
 *
 * @param GPIO_Pin
 *
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)  // <----- The ISR Function We're Looking For!
{
	it_button = 1;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
