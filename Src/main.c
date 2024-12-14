/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include "sys_timer.h"
#include <string.h>
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define DELAY_VALUE (5000)
#define MAX_LED	(16)
#define PI 3.14159265
#define WS2812_USE_BRIGHTNESS

uint8_t g_aubLedData[MAX_LED][4];
uint8_t g_aubLedMod[MAX_LED][4];
int16_t g_aubPwmData[(24*MAX_LED)+20];
uint8_t g_ubDatasentflag = 0;

void Ws2812SetLed(uint8_t ubLedIndex, uint8_t ubRedColor, uint8_t ubGreenColor, uint8_t ubBlueColor)
{
	g_aubLedData[ubLedIndex][0] = ubLedIndex;
	g_aubLedData[ubLedIndex][1] = ubGreenColor;
	g_aubLedData[ubLedIndex][2] = ubRedColor;
	g_aubLedData[ubLedIndex][3] = ubBlueColor;
}

void Ws2812ClearAll(void)
{
	for(uint8_t i = 0 ; i < MAX_LED ; i++)
	{
		Ws2812SetLed(i, 0, 0, 0);
	}
}

void Ws2812SetAll(uint8_t ubRedColor, uint8_t ubGreenColor, uint8_t ubBlueColor)
{
	for(uint8_t i = 0 ; i < MAX_LED ; i++)
	{
		Ws2812SetLed(i, ubRedColor, ubGreenColor, ubBlueColor);
	}
}

void Ws2812Send(void)
{
	uint32_t color;
	uint32_t indx=0;
	for (int i= 0; i < MAX_LED; i++)
	{
#ifdef WS2812_USE_BRIGHTNESS
		color = ((g_aubLedMod[i][1]<<16) | (g_aubLedMod[i][2]<<8) | (g_aubLedMod[i][3]));
#else
		color = ((g_aubLedData[i][1]<<16) | (g_aubLedData[i][2]<<8) | (g_aubLedData[i][3]));
#endif
		for(int8_t i = 23 ; i >= 0 ; i--)
		{
			if (color&(1<<i))
			{
				g_aubPwmData[indx] = 53;
			}
			else
			{
				g_aubPwmData[indx] = 27;
			}
			indx++;
		}
	}
	for (int i=0; i<20; i++)
	{
		g_aubPwmData[indx] = 0;
		indx++;
	}
	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)g_aubPwmData, indx);
	while (!g_ubDatasentflag){};
	g_ubDatasentflag = 0;
}

void W2812SetBrightness (uint8_t brightness)  // 0-45
{
	if (brightness > 45) brightness = 45;
	for (int i=0; i<MAX_LED; i++)
	{
		g_aubLedMod[i][0] = g_aubLedData[i][0];
		for (int j=1; j<4; j++)
		{
			float angle = 90-brightness;  // in degrees
			angle = angle*PI / 180;  // in rad
			g_aubLedMod[i][j] = (g_aubLedData[i][j])/(tan(angle));
		}
	}
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
	g_ubDatasentflag = 1;
}

void hsv_to_rgb(int hue, float sat, float val, float *r, float *g, float *b);

void rainbow_effect(void) {
  static uint16_t color_offset = 0;
  for (int i = 0; i < MAX_LED; i++) {
    int hue = (i * 360 / MAX_LED + color_offset) % 360;
    float r, g, b;
    hsv_to_rgb(hue, 1.0, 1.0, &r, &g, &b); // Hàm chuyển đổi HSV sang RGB
    Ws2812SetLed(i, r * 255, g * 255, b * 255);
  }
  W2812SetBrightness(45);
  color_offset += 1;
}

void hsv_to_rgb(int hue, float sat, float val, float *r, float *g, float *b) {
  int h = hue / 60;
  float f = (float)hue / 60 - h;
  float p = val * (1 - sat);
  float q = val * (1 - f * sat);
  float t = val * (1 - (1 - f) * sat);

  switch (h) {
  case 0:
    *r = val, *g = t, *b = p;
    break;
  case 1:
    *r = q, *g = val, *b = p;
    break;
  case 2:
    *r = p, *g = val, *b = t;
    break;
  case 3:
    *r = p, *g = q, *b = val;
    break;
  case 4:
    *r = t, *g = p, *b = val;
    break;
  case 5:
  default:
    *r = val, *g = p, *b = q;
    break;
  }
}

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
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  while(1)
  {
	  if(!HAL_GPIO_ReadPin(USER_INPUT_GPIO_Port, USER_INPUT_Pin)) break;
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  SysTimerLoop();
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
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
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void SysTimer20msEvent(void)
{

}
void SysTimer50msEvent(void)
{
	static uint8_t s_ubLedIndex = 0;
	static uint8_t s_ubEffect = 0, s_ubPreEffect = 0;;
	static int8_t s_ubBrightness = 0;

	if(s_ubEffect == 0)
	{
		Ws2812SetLed(s_ubLedIndex, 255, 255, 0);
		W2812SetBrightness(45);

		s_ubLedIndex++;
		if(s_ubLedIndex >= MAX_LED) {
			s_ubLedIndex = 0;
			Ws2812ClearAll();
			s_ubEffect++;
		}
	}
	else if(s_ubEffect == 1)
	{
		Ws2812SetLed(s_ubLedIndex, 0, 255, 255);
		W2812SetBrightness(45);

		s_ubLedIndex++;
		if(s_ubLedIndex >= MAX_LED) {
			s_ubLedIndex = 0;
			Ws2812ClearAll();
			s_ubEffect++;
		}
	}
	else if(s_ubEffect == 2)
	{
		Ws2812SetLed(s_ubLedIndex, 255, 0, 255);
		W2812SetBrightness(45);

		s_ubLedIndex++;
		if(s_ubLedIndex >= MAX_LED) {
			s_ubLedIndex = 0;
			Ws2812ClearAll();
			s_ubEffect = 3;
		}
	}
	else if(s_ubEffect == 3)
	{
		for(uint8_t i = 0 ; i < MAX_LED; i++)
		{
			if(s_ubLedIndex%2)
			{
				if(i%2)
				{
					Ws2812SetLed(i, 128, 128, 0);
				} else {
					Ws2812SetLed(i, 128, 0, 200);
				}
			} else {
				if(i%2)
				{
					Ws2812SetLed(i, 128, 0, 200);
				} else {
					Ws2812SetLed(i, 128, 128, 0);
				}
			}
		}
		W2812SetBrightness(45);

		s_ubLedIndex++;
		if(s_ubLedIndex >= 20) {
			s_ubLedIndex = 0;
			Ws2812ClearAll();
			s_ubEffect = 4;
		}
	}
	else if(s_ubEffect == 4)
	{
		Ws2812SetLed(0, 255, 0, 0);
		Ws2812SetLed(1, 0, 255, 0);
		Ws2812SetLed(2, 0, 0, 255);
		Ws2812SetLed(3, 46, 89, 128);
		Ws2812SetLed(4, 156, 233, 100);
		Ws2812SetLed(5, 102, 0, 235);
		Ws2812SetLed(6, 47, 38, 77);
		Ws2812SetLed(7, 255, 200, 0);
		Ws2812SetLed(8, 255, 0, 0);
		Ws2812SetLed(9, 0, 255, 0);
		Ws2812SetLed(10, 0, 0, 255);
		Ws2812SetLed(11, 46, 89, 128);
		Ws2812SetLed(12, 156, 233, 100);
		Ws2812SetLed(13, 102, 0, 235);
		Ws2812SetLed(14, 47, 38, 77);
		Ws2812SetLed(15, 255, 200, 0);

		W2812SetBrightness(s_ubBrightness);
		Ws2812Send();
		s_ubBrightness+=10;
		if(s_ubBrightness > 45)
		{
			s_ubBrightness = 45;
			Ws2812ClearAll();
			s_ubEffect = 5;
		}
	}
	else if(s_ubEffect == 5)
	{
		Ws2812SetLed(0, 255, 0, 0);
		Ws2812SetLed(1, 0, 255, 0);
		Ws2812SetLed(2, 0, 0, 255);
		Ws2812SetLed(3, 46, 89, 128);
		Ws2812SetLed(4, 156, 233, 100);
		Ws2812SetLed(5, 102, 0, 235);
		Ws2812SetLed(6, 47, 38, 77);
		Ws2812SetLed(7, 255, 200, 0);
		Ws2812SetLed(8, 255, 0, 0);
		Ws2812SetLed(9, 0, 255, 0);
		Ws2812SetLed(10, 0, 0, 255);
		Ws2812SetLed(11, 46, 89, 128);
		Ws2812SetLed(12, 156, 233, 100);
		Ws2812SetLed(13, 102, 0, 235);
		Ws2812SetLed(14, 47, 38, 77);
		Ws2812SetLed(15, 255, 200, 0);

		W2812SetBrightness(s_ubBrightness);
		Ws2812Send();
		s_ubBrightness-=10;
		if(s_ubBrightness <= 0)
		{
			s_ubBrightness = 0;
			Ws2812ClearAll();
			s_ubEffect = 6;
		}
	}
	else if(s_ubEffect == 6)
	{
		rainbow_effect();
	}

	Ws2812Send();
	if(s_ubEffect != s_ubPreEffect)
	{
		while(1) if(!HAL_GPIO_ReadPin(USER_INPUT_GPIO_Port, USER_INPUT_Pin)) break;
		s_ubPreEffect = s_ubEffect;
	}
}
void SysTimer100msEvent(void)
{

}
void SysTimer500msEvent(void)
{

}
void SysTimer1000msEvent(void)
{

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
