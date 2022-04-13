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
#include "fatfs.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "File_Handling.h"
RTC_AlarmTypeDef sAlarm;
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;
FILINFO ano;
FRESULT afresult;
FATFS *apfs;
FATFS afs;
FIL afil;
UINT abr;

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
HAL_SD_CardInfoTypeDef SDCardInfo;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 RTC_HandleTypeDef hrtc;

SD_HandleTypeDef hsd;
DMA_HandleTypeDef hdma_sdio_rx;
DMA_HandleTypeDef hdma_sdio_tx;

TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart7;
UART_HandleTypeDef huart8;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_UART8_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM7_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_UART7_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t buffer[100]={0};
int indx = 0;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char aShowTime[10] = {0};
char aShowDate[9] = {0};
char aSensData[150]={0};
uint8_t SensData=0;
uint8_t SensData_index=0;
char aSensDatacall[24]="{CMD=20,DADDR=1,CHS=999}";
char Temp[10]={0};	//Temperature
char Hum[10]={0};	//humidity
char Sun[10]={0};	// sunshine
char Rain[2]={0};	// rain direction
char Wind_direction[10];
char Wind_d[10];	// wind direction
char Wind_s[7]={0};	// wind speed
char SMO[10]={0};	// soil moisture
char STP[10]={0};	// soil Temperature
char SEC[10]={0};	// soil EC
int i=0;
int alram_cnt=0;

char Fname[20]={0};	//file name save

/*command for file read, RTC setup*/
#define rx_buf 64
uint8_t recvbf[rx_buf]={0};
uint8_t cm_Rx=0;
uint8_t rx_index=0;
uint8_t readF[64];

/* RTC Setup */

uint8_t h;
uint8_t m;
uint8_t s;
uint8_t Y;
uint8_t M;
uint8_t D;

void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);



  /* Display time Format: hh:mm:ss */
  sprintf((char*)showtime,"%02d:%02d:%02d",stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  /* Display date Format: mm-dd-yy */
  sprintf((char*)showdate,"%02d%02d%02d",2000+sdatestructureget.Year, sdatestructureget.Month,sdatestructureget.Date);

}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
	if(htim->Instance ==TIM7)
	{

		i++;
		if(i>9)
		{
			HAL_UART_Transmit(&huart7, aSensDatacall, sizeof(aSensDatacall), 1);
			i=0;
		}
	}
}
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BIN);
	if (sTime.Minutes==00 || sTime.Minutes==10 || sTime.Minutes==20 || sTime.Minutes==30 || sTime.Minutes==40 || sTime.Minutes==50)
	{
	strcpy(Fname, aShowDate);
	strcat(Fname,".csv");
	Mount_SD("/");
	afresult  =f_stat(Fname,&ano);
	if(afresult != FR_OK)
	{
		Create_File(Fname);
		Update_File(Fname, "\r\nTime,Temp,Hum,Sun,Rain,Wind_d,Wind_s,SMO,STP,SEC\r\n");
		Update_File(Fname, buffer);
		Unmount_SD("/");
		Mount_SD("/");
	}

	RTC_CalendarShow(aShowTime, aShowDate);
	strcat(buffer, aShowTime);
	strcat(buffer,",");
	strcat(buffer, Temp);
	strcat(buffer,",");
	strcat(buffer, Hum);
	strcat(buffer,",");
	strcat(buffer, Sun);
	strcat(buffer,",");
	strcat(buffer, Rain);
	strcat(buffer,",");
	strcat(buffer, Wind_d);
	strcat(buffer,",");
	strcat(buffer, Wind_s);
	strcat(buffer,",");
	strcat(buffer, SMO);
	strcat(buffer,",");
	strcat(buffer, STP);
	strcat(buffer,",");
	strcat(buffer, SEC);
	strcat(buffer,"\r\n");


	Update_File(Fname, buffer);
	memset(buffer, 0, sizeof(buffer));

	Unmount_SD("/");
	}
}
void command();
void sensdataget()
{
	if (SensData!= '\0' && SensData_index<150)
	{
		aSensData[SensData_index]=SensData;
		SensData_index++;
	}
	else
	{
		char *str;
		char *str2=NULL;
		str=strtok_r(aSensData,"{=,}",&str2);

		while(str != NULL)
		{
			str=strtok_r(NULL,"{=,}",&str2);
			if(strcmp(str,"ST001")==0)
			{
				str=strtok_r(NULL,"{=,}",&str2);
				strcpy(Temp, str);
			}
			else if(strcmp(str,"ST002")==0)
			{
				str=strtok_r(NULL,"{=,}",&str2);
				strcpy(Hum, str);
			}
			else if(strcmp(str,"ST008")==0)
			{
				str=strtok_r(NULL,"{=,}",&str2);
				strcpy(Sun, str);
			}
			else if(strcmp(str,"ST010")==0)
			{
				str=strtok_r(NULL,"{=,}",&str2);
				strcpy(Wind_direction, str);
				strcpy(Wind_d, str);
				strcpy(Wind_direction, str);

			}
			else if(strcmp(str,"ST009")==0)
			{
				str=strtok_r(NULL,"{=,}",&str2);
				strcpy(Wind_s, str);
			}
			else if(strcmp(str,"ST014")==0)
			{
				str=strtok_r(NULL,"{=,}",&str2);
				strcpy(Rain, str);
			}
			else if(strcmp(str,"SMO")==0)
			{
				str=strtok_r(NULL,"{=,}",&str2);
				strcpy(SMO, str);
			}
			else if(strcmp(str,"STP")==0)
			{
				str=strtok_r(NULL,"{=,}",&str2);
				strcpy(STP, str);
			}
			else if(strcmp(str,"SEC")==0)
			{
				str=strtok_r(NULL,"{=,}",&str2);
				strcpy(SEC, str);
			}
		}
		SensData_index=0;
		memset(aSensData,0,sizeof(aSensData));
	}
	HAL_UART_Receive_IT(&huart7, &SensData, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==huart7.Instance)
	{
		sensdataget();
	}
	if(huart->Instance==huart3.Instance)
	{
		command();
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
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_UART8_Init();
  MX_RTC_Init();
  MX_TIM7_Init();
  MX_USART3_UART_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_UART7_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, RESET);
  afresult = f_mount(&afs, "/", 1);
  if (afresult!=FR_OK)
  {
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, SET);
  }

  Unmount_SD("/");

  HAL_TIM_Base_Start_IT(&htim7);
  sAlarm.AlarmTime.Seconds=0x00;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
  sAlarm.Alarm = RTC_ALARM_A;
  HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD);
  HAL_UART_Receive_IT(&huart3, &cm_Rx, 1);
  HAL_UART_Receive_IT(&huart7, &SensData, 1);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  RTC_CalendarShow(aShowTime, aShowDate);


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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) !=0x32F2)
    {
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 23;
  sTime.Minutes = 59;
  sTime.Seconds = 59;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 1;
  sDate.Year = 22;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0;
  sAlarm.AlarmTime.Minutes = 0;
  sAlarm.AlarmTime.Seconds = 0;
  sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
 	  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x32F2);
   }
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 10;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 8399;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 9999;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief UART7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART7_Init(void)
{

  /* USER CODE BEGIN UART7_Init 0 */

  /* USER CODE END UART7_Init 0 */

  /* USER CODE BEGIN UART7_Init 1 */

  /* USER CODE END UART7_Init 1 */
  huart7.Instance = UART7;
  huart7.Init.BaudRate = 115200;
  huart7.Init.WordLength = UART_WORDLENGTH_8B;
  huart7.Init.StopBits = UART_STOPBITS_1;
  huart7.Init.Parity = UART_PARITY_NONE;
  huart7.Init.Mode = UART_MODE_TX_RX;
  huart7.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart7.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART7_Init 2 */

  /* USER CODE END UART7_Init 2 */

}

/**
  * @brief UART8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART8_Init(void)
{

  /* USER CODE BEGIN UART8_Init 0 */

  /* USER CODE END UART8_Init 0 */

  /* USER CODE BEGIN UART8_Init 1 */

  /* USER CODE END UART8_Init 1 */
  huart8.Instance = UART8;
  huart8.Init.BaudRate = 115200;
  huart8.Init.WordLength = UART_WORDLENGTH_8B;
  huart8.Init.StopBits = UART_STOPBITS_1;
  huart8.Init.Parity = UART_PARITY_NONE;
  huart8.Init.Mode = UART_MODE_TX_RX;
  huart8.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart8.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart8) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART8_Init 2 */

  /* USER CODE END UART8_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_8;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void command()
{
	int cnt_read=0;
	if (cm_Rx!= '*' && rx_index < 64)
	{
		recvbf[rx_index++]=cm_Rx;
	}
	else
	{
		if(recvbf[0]=='#')
		{
			if(recvbf[1]=='$')
			{
				for( cnt_read=0; cnt_read<64; cnt_read++)
				{
					if(recvbf[2+cnt_read]!='%')
					{
						readF[cnt_read]=recvbf[2+cnt_read];
					}
				}
				cnt_read=0;
				Mount_SD("/");
				Read_File(readF);
				Unmount_SD("/");
				memset(readF, 0, sizeof(readF));
				memset(recvbf, 0, sizeof(recvbf));

			}
			else if(recvbf[1]=='t')//time setup
			{
				for(int i=0; i<64; i++)
				{
					if(recvbf[2+i]=='%')
					{
						break;
					}
					else
					{
						readF[i]=recvbf[2+i];
					}
				}
				HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

				h=atoi(strtok(readF,"."));
				m=atoi(strtok(NULL,"."));
				s=atoi(strtok(NULL,"."));

				sTime.Hours=h;
				sTime.Minutes=m;
				sTime.Seconds=s;
				sTime.TimeFormat = RTC_HOURFORMAT_24;
				char bshowtime[32]={0};
				sprintf(bshowtime,"\r\n**Time Setup %02d:%02d:%02d \r\n",sTime.Hours, sTime.Minutes, sTime.Seconds);
				HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				HAL_UART_Transmit(&huart3, bshowtime, sizeof(bshowtime), HAL_MAX_DELAY);
				rx_index =0;
				memset(readF, 0, sizeof(readF));
				memset(recvbf, 0, sizeof(recvbf));

			}
			else if(recvbf[1]=='d')//date setup
			{
				for(int i=0; i<64; i++)
				{
					if(recvbf[2+i]=='%')
					{
						break;
					}
					else
					{
						readF[i]=recvbf[2+i];
					}
				}

				Y=atoi(strtok(readF,"."));
				M=atoi(strtok(NULL,"."));
				D=atoi(strtok(NULL,"."));
				HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				sDate.Year=Y;
				sDate.Month=M;
				sDate.Date=D;
				char bshowdate[32]={0};
				sprintf(bshowdate,"\r\n**Date Setup %02d.%02d.%02d. \r\n",sDate.Year, sDate.Month, sDate.Date);
				HAL_UART_Transmit(&huart3, bshowdate, sizeof(bshowdate), HAL_MAX_DELAY);
				HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				rx_index =0;
				memset(readF, 0, sizeof(readF));
				memset(recvbf, 0, sizeof(recvbf));

			}
			else if(recvbf[1]=='s')//scan file
			{
				for(int i=0; i<64; i++)
				{
					if(recvbf[2+i]=='%')
					{
						break;
					}
					else
					{
						readF[i]=recvbf[2+i];
					}
				}
				Mount_SD("/");
				Scan_SD(readF);
				Unmount_SD("/");
				rx_index =0;
				memset(readF, 0, sizeof(readF));
				memset(recvbf, 0, sizeof(recvbf));

			}
		}
		rx_index =0;
	}
	HAL_UART_Receive_IT(&huart3, &cm_Rx, 1);
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
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, SET);
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
