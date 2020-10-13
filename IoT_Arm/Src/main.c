/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define H1 GPIO_PIN_1//no GPIOA
#define H2 GPIO_PIN_4//no GPIOA
#define H3 GPIO_PIN_10//no GPIOA
#define V1 GPIO_PIN_0//no GPIOB
#define V2 GPIO_PIN_0//no GPIOC
#define V3 GPIO_PIN_1//no GPIOC
#define C1 GPIO_PIN_10//no GPIOB

#define B1 GPIO_PIN_8//no GPIOB
#define B2 GPIO_PIN_9//no GPIOB
#define B3 GPIO_PIN_8//no GPIOA
#define B4 GPIO_PIN_7//no GPIOA
#define B5 GPIO_PIN_7//no GPIOC
#define B6 GPIO_PIN_9//no GPIOA
#define B7 GPIO_PIN_5//no GPIOB
#define B8 GPIO_PIN_4//no GPIOB
#define ALARM GPIO_PIN_6//no GPIOA


#define ESW GPIO_PIN_0//no GPIOA

#define J1 GPIO_PIN_15//no GPIOA
#define J2 GPIO_PIN_2//no GPIOB

#define JANELA_FECHADA 900
#define JANELA_ABERTA 500
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int  l=0;
uint8_t pos=0, cool=0;
uint8_t lamps[6]={0,0,0,0,0,0}, ap[]={0,0,0,0,0,0,0,0,0,1}, janelas[]={0,0};
uint8_t lastEnc=0, Enc=0, temperatura=30, tempmedida, tstring[2]="  ";
uint8_t MSG[10] = {};

uint8_t rcv[10]="         \n";
uint8_t rx, j=0, g=0;
uint8_t intruder=0;

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
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOA, H1, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, H2, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, H3, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, V1, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, V2, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, V3, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, C1, GPIO_PIN_RESET);
  snprintf(MSG, sizeof(MSG), "sta     \n");//informa ao sistema que foi reiniciado
  HAL_UART_Transmit(&huart2, MSG, sizeof(MSG),100);

  HAL_TIM_Base_Start_IT(&htim11);
  HAL_TIM_Base_Start_IT(&htim10);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
  HAL_UART_Receive_IT(&huart2, &rx, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void conv_temp(){//converte o valor em caracteres da temperatura para um valor inteiro
	tempmedida=0;
	tempmedida+=(tstring[1]-48);
	tempmedida+=(tstring[0]-48)*10;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	rcv[j]=rx;
	j++;
	if(j==10)j=0;
	if(rx==13){
		snprintf(rcv, sizeof(rcv), "          ");//"limpa" o vetor de recepção
		j=0;//zera a posição do vetor de recepção
	}

	if(rcv[0]=='l'){//para as lâmpadas
		if(rcv[6]=='0'||rcv[6]=='3'){
			for(g=0;g<7;g++){
				if(rcv[g+1]=='0')lamps[g]=0;
				if(rcv[g+1]=='3')lamps[g]=1;
			}
			snprintf(rcv, sizeof(rcv), "          ");//"limpa" o vetor de recepção
			j=0;//zera a posição do vetor de recepção
		}
	}
	if(rcv[0]=='j'){//para as janelas
		if(rcv[2]== '0'||rcv[2]=='1'){
			if(rcv[1]=='0')janelas[0]=0;
			if(rcv[1]=='1')janelas[0]=1;
			if(rcv[2]=='0')janelas[1]=0;
			if(rcv[2]=='1')janelas[1]=1;
			snprintf(rcv, sizeof(rcv), "          ");//"limpa" o vetor de recepção
			j=0;//zera a posição do vetor de recepção
		}
	}
	if(rcv[0]=='t'){//referente à temperatura
		if(rcv[1]>=48&&rcv[2]>=48){
			tstring[0]=rcv[1];
			tstring[1]=rcv[2];
			conv_temp();
			if(tempmedida>=temperatura){
				cool=1;//liga o cooler quando a temperatura medida é maior que a temperatura configurada
			}
			else if(tempmedida<temperatura){
				cool=0;
			}
			snprintf(rcv, sizeof(rcv), "          ");//"limpa" o vetor de recepção
			j=0;//zera a posição do vetor de recepção
		}
	}
	if(rcv[0]=='i'&&rcv[1]=='0'){//referente à segurança ou intruso
		intruder=0;
	}
	HAL_UART_Receive_IT(&huart2, &rx, 1);//reinicia a recepção de valores pela serial
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim==&htim11 && intruder==0){
		if(HAL_GPIO_ReadPin (GPIOB, B1)&&ap[0]==0){
			lamps[0]=!lamps[0], ap[0]=1;
			if(lamps[0]==1)snprintf(MSG, sizeof(MSG), "l11     \n");//envia o estado da lâmpada
			if(lamps[0]==0)snprintf(MSG, sizeof(MSG), "l10     \n");
			HAL_UART_Transmit_IT(&huart2, MSG, sizeof(MSG));
		}
		if(!HAL_GPIO_ReadPin (GPIOB, B1))ap[0]=0;

		if(HAL_GPIO_ReadPin (GPIOB, B2)&&ap[1]==0){
			lamps[1]=!lamps[1], ap[1]=1;
			if(lamps[1]==1)snprintf(MSG, sizeof(MSG), "l21     \n");//envia o estado da lâmpada
			if(lamps[1]==0)snprintf(MSG, sizeof(MSG), "l20     \n");
			HAL_UART_Transmit_IT(&huart2, MSG, sizeof(MSG));
		}
		if(!HAL_GPIO_ReadPin (GPIOB, B2))ap[1]=0;

		if(HAL_GPIO_ReadPin (GPIOA, B3)&&ap[2]==0){
			lamps[2]=!lamps[2], ap[2]=1;
			if(lamps[2]==1)snprintf(MSG, sizeof(MSG), "l31     \n");//envia o estado da lâmpada
			if(lamps[2]==0)snprintf(MSG, sizeof(MSG), "l30     \n");
			HAL_UART_Transmit_IT(&huart2, MSG, sizeof(MSG));
		}
		if(!HAL_GPIO_ReadPin (GPIOA, B3))ap[2]=0;

		if(HAL_GPIO_ReadPin (GPIOA, B4)&&ap[3]==0){
			lamps[3]=!lamps[3], ap[3]=1;
			if(lamps[3]==1)snprintf(MSG, sizeof(MSG), "l41     \n");//envia o estado da lâmpada
			if(lamps[3]==0)snprintf(MSG, sizeof(MSG), "l40     \n");
			HAL_UART_Transmit_IT(&huart2, MSG, sizeof(MSG));
		}
		if(!HAL_GPIO_ReadPin (GPIOA, B4))ap[3]=0;

		if(HAL_GPIO_ReadPin (GPIOC, B5)&&ap[4]==0){
			lamps[4]=!lamps[4], ap[4]=1;
			if(lamps[4]==1)snprintf(MSG, sizeof(MSG), "l51     \n");//envia o estado da lâmpada
			if(lamps[4]==0)snprintf(MSG, sizeof(MSG), "l50     \n");
			HAL_UART_Transmit_IT(&huart2, MSG, sizeof(MSG));
		}
		if(!HAL_GPIO_ReadPin (GPIOC, B5)) ap[4]=0;

		if(HAL_GPIO_ReadPin (GPIOA, B6)&&ap[5]==0){
			lamps[5]=!lamps[5], ap[5]=1;
			if(lamps[5]==1)snprintf(MSG, sizeof(MSG), "l61     \n");//envia o estado da lâmpada
			if(lamps[5]==0)snprintf(MSG, sizeof(MSG), "l60     \n");
			HAL_UART_Transmit_IT(&huart2, MSG, sizeof(MSG));
		}
		if(!HAL_GPIO_ReadPin (GPIOA, B6))ap[5]=0;

		if(HAL_GPIO_ReadPin (GPIOB, B7)&&ap[6]==0){
			janelas[0]=!janelas[0], ap[6]=1;
			if(janelas[0]==1)snprintf(MSG, sizeof(MSG), "j11     \n");//envia o estado da janela
			if(janelas[0]==0)snprintf(MSG, sizeof(MSG), "j10     \n");
			HAL_UART_Transmit_IT(&huart2, MSG, sizeof(MSG));
		}
		if(!HAL_GPIO_ReadPin (GPIOB, B7))ap[6]=0;

		if(HAL_GPIO_ReadPin (GPIOB, B8)&&ap[7]==0){
			janelas[1]=!janelas[1], ap[7]=1;
			if(janelas[1]==1)snprintf(MSG, sizeof(MSG), "j21     \n");//envia o estado da janela
			if(janelas[1]==0)snprintf(MSG, sizeof(MSG), "j20     \n");
			HAL_UART_Transmit_IT(&huart2, MSG, sizeof(MSG));
		}
		if(!HAL_GPIO_ReadPin (GPIOB, B8))ap[7]=0;

		if(!HAL_GPIO_ReadPin (GPIOA, ALARM)&&ap[9]==0){
			ap[9]=1;
			intruder=1;
			snprintf(MSG, sizeof(MSG), "i1      \n\r");//avisa que há um intruso
			HAL_UART_Transmit_IT(&huart2, MSG, sizeof(MSG));
		}
		if(HAL_GPIO_ReadPin (GPIOA, ALARM))ap[9]=0;
	}


	if(htim==&htim10){
		//ajusta a temperatura na qual se quer que ligue o cooler com o encoder
		Enc=(TIM4->CNT)>>2;
		if(Enc==0&&lastEnc==124) {
			if(temperatura!=0)temperatura--;
		}
		if(Enc==124&&lastEnc==0){
			if(temperatura!=70)temperatura++;
		}
		else{
			if(Enc-lastEnc<0&&temperatura!=0)temperatura+=Enc-lastEnc;
			if(Enc-lastEnc>0&&temperatura!=70)temperatura+=Enc-lastEnc;
		}


		if(tempmedida>=temperatura){
			cool=1;//liga o cooler quando a temperatura medida é maior que a temperatura configurada
		}
		else if(tempmedida<temperatura){
			cool=0;
		}

		//multiplexação das lâmpadas
		if(pos==0){
			if(lamps[0]==1)HAL_GPIO_WritePin(GPIOA, H1, GPIO_PIN_SET);
			if(lamps[0]==0)HAL_GPIO_WritePin(GPIOA, H1, GPIO_PIN_RESET);
			if(lamps[3]==1)HAL_GPIO_WritePin(GPIOA, H2, GPIO_PIN_SET);
			if(lamps[3]==0)HAL_GPIO_WritePin(GPIOA, H2, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, H3, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, V1, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, V2, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, V3, GPIO_PIN_SET);
		}
		if(pos==1){
			if(lamps[1]==1)HAL_GPIO_WritePin(GPIOA, H1, GPIO_PIN_SET);
			if(lamps[1]==0)HAL_GPIO_WritePin(GPIOA, H1, GPIO_PIN_RESET);
			if(lamps[4]==1)HAL_GPIO_WritePin(GPIOA, H2, GPIO_PIN_SET);
			if(lamps[4]==0)HAL_GPIO_WritePin(GPIOA, H2, GPIO_PIN_RESET);
			if(HAL_GPIO_ReadPin(GPIOA, ESW))ap[8]=0;
			if(!HAL_GPIO_ReadPin (GPIOA, ESW)&&ap[8]==0)cool=!cool,ap[8]=1;
			if(janelas[0]==1){
				//liga o led que representa a janela 1 e altera o valor de PWM para o Servomotor
				HAL_GPIO_WritePin(GPIOA, H3, GPIO_PIN_SET);
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, JANELA_ABERTA);
			}
			if(janelas[0]==0){
				HAL_GPIO_WritePin(GPIOA, H3, GPIO_PIN_RESET);
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, JANELA_FECHADA);
			}
			if(cool==1) HAL_GPIO_WritePin(GPIOB, C1, GPIO_PIN_SET);//liga o cooler

			if(cool==0) HAL_GPIO_WritePin(GPIOB, C1, GPIO_PIN_RESET);//desliga o cooler

			HAL_GPIO_WritePin(GPIOB, V1, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, V2, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, V3, GPIO_PIN_SET);
		}
		if(pos==2){
			if(lamps[2]==1)HAL_GPIO_WritePin(GPIOA, H1, GPIO_PIN_SET);
			if(lamps[2]==0)HAL_GPIO_WritePin(GPIOA, H1, GPIO_PIN_RESET);
			if(lamps[5]==1)HAL_GPIO_WritePin(GPIOA, H2, GPIO_PIN_SET);
			if(lamps[5]==0)HAL_GPIO_WritePin(GPIOA, H2, GPIO_PIN_RESET);

			if(janelas[1]==1){
				//liga o led que representa a janela 2 e altera o valor de PWM para o Servomotor
				HAL_GPIO_WritePin(GPIOA, H3, GPIO_PIN_SET);
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, JANELA_ABERTA);
			}
			if(janelas[1]==0){
				HAL_GPIO_WritePin(GPIOA, H3, GPIO_PIN_RESET);
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, JANELA_FECHADA);
			}
			HAL_GPIO_WritePin(GPIOB, V1, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, V2, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, V3, GPIO_PIN_RESET);
		}
		pos++;
		if(pos==3)pos=0;
	}
	lastEnc=Enc;
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
