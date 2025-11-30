/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include <stdbool.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CANSPI.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_DATA_SIZE 14
#define TX_DATA_SIZE 20
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uCAN_MSG txMessage;
uCAN_MSG rxMessage;
uint8_t RX_DATA[RX_DATA_SIZE];
volatile uint8_t TX_DATA[TX_DATA_SIZE];
bool is_new_message = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	txMessage.frame.idType = RX_DATA[0]; /* STD (1) or EXT (2) */
	txMessage.frame.id = (RX_DATA[4] << 24) | (RX_DATA[3] << 16) | (RX_DATA[2] << 8) | RX_DATA[1];
	txMessage.frame.dlc = RX_DATA[5];
	txMessage.frame.data0 = RX_DATA[6];
	txMessage.frame.data1 = RX_DATA[7];
	txMessage.frame.data2 = RX_DATA[8];
	txMessage.frame.data3 = RX_DATA[9];
	txMessage.frame.data4 = RX_DATA[10];
	txMessage.frame.data5 = RX_DATA[11];
	txMessage.frame.data6 = RX_DATA[12];
	txMessage.frame.data7 = RX_DATA[13];
	is_new_message = true;
	HAL_UART_Receive_DMA(&huart2, RX_DATA, RX_DATA_SIZE);
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
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* In this function you can set the filter and the baudrate for the MCP2515 modle */
  CANSPI_Initialize();	

  /* Listen for RX_DATA_SIZE bytes from USB */
  HAL_UART_Receive_DMA(&huart2, RX_DATA, RX_DATA_SIZE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if(CANSPI_Receive(&rxMessage)){
    	TX_DATA[0] = rxMessage.frame.idType;
        TX_DATA[1] = (rxMessage.frame.id >> 24) & 0xFF;
        TX_DATA[2] = (rxMessage.frame.id >> 16) & 0xFF;
        TX_DATA[3] = (rxMessage.frame.id >> 8) & 0xFF;
        TX_DATA[4] = rxMessage.frame.id & 0xFF;
        TX_DATA[5] = rxMessage.frame.dlc;
        TX_DATA[6] = rxMessage.frame.data0;
        TX_DATA[7] = rxMessage.frame.data1;
        TX_DATA[8] = rxMessage.frame.data2;
        TX_DATA[9] = rxMessage.frame.data3;
        TX_DATA[10] = rxMessage.frame.data4;
        TX_DATA[11] = rxMessage.frame.data5;
        TX_DATA[12] = rxMessage.frame.data6;
        TX_DATA[13] = rxMessage.frame.data7;
        TX_DATA[14] = 'S';
        TX_DATA[15] = 'T';
        TX_DATA[16] = 'M';
        TX_DATA[17] = '3';
        TX_DATA[18] = '2';
        TX_DATA[19] = '\0';
        HAL_UART_Transmit(&huart2, TX_DATA, TX_DATA_SIZE, 100);
    }  
    
    if(is_new_message){
        CANSPI_Transmit(&txMessage);
    	is_new_message = false;
    }

    HAL_Delay(1);
    
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
