/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */
RTC_AlarmTypeDef sAlarm = {0};
uint8_t RTC_AlarmA_flag = 0;

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
    
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date 
  */
  sTime.Hours = 0x12;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_SET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
  sDate.Month = RTC_MONTH_AUGUST;
  sDate.Date = 0x1;
  sDate.Year = 0x20;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the Alarm A 
  */
  sAlarm.AlarmTime.Hours = 0x12;
  sAlarm.AlarmTime.Minutes = 0x12;
  sAlarm.AlarmTime.Seconds = 0x3;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_SET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS
                              |RTC_ALARMMASK_MINUTES;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */
	  //----- appelé par stm32l4xx_hal_rtc.c/HAL_RTC_Init, mais avant mise a l'heure de CubeMX!!
  /* USER CODE END RTC_MspInit 0 */
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
/*
  * @brief   Mise a l'heure RTC with the compilation date & time
  * @param  none
  * @retval none
*/
void RTC_MiseAheure(void)
{
	  /*
	   *  init RTC with the compilation date & time
	   */
	  RTC_TimeTypeDef sTime = {0};
	  RTC_DateTypeDef sDate = {0,0,0,20};
	  //----- __TIME__ compil time is a string
	  uint8_t conv2d(const char* p) {
		  uint8_t v = 0;
		  if ('0' <= *p && *p <= '9')
			  v = *p - '0';
		  return 10 * v + *++p - '0';
	  }
	  sTime.Hours = conv2d(__TIME__);
	  sTime.Minutes = conv2d(__TIME__ + 3);
	  sTime.Seconds = conv2d(__TIME__ + 6);

	  //----- __DATE__ compil date is a string
	  // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
	  switch (__DATE__[0]) {
	  case 'J': sDate.Month = (__DATE__[1] == 'a') ? 1 : ((__DATE__[2] == 'n') ? 6 : 7); break;
	  case 'F': sDate.Month = 2; break;
	  case 'A': sDate.Month = __DATE__[2] == 'r' ? 4 : 8; break;
	  case 'M': sDate.Month = __DATE__[2] == 'r' ? 3 : 5; break;
	  case 'S': sDate.Month = 9; break;
	  case 'O': sDate.Month = 10; break;
	  case 'N': sDate.Month = 11; break;
	  case 'D': sDate.Month = 12; break;
	  }
	  sDate.Date = conv2d(__DATE__ + 4);

	  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

/*
  * @brief  modifier le setup de RTC_AlarmA, ajouter qq seconds en BCD
  * @param  none
  * @retval status
*/
void RTC_AlarmModify(){
	sAlarm.Alarm = RTC_ALARM_A;
	//sAlarm.AlarmTime.Hours = 0x12;		//HH:MN peu importe
	//sAlarm.AlarmTime.Minutes = 0x1;
	sAlarm.AlarmTime.Seconds += 0x04;		//add 4 seconds en BCD
	if ((sAlarm.AlarmTime.Seconds & 0x000F) > 0x9) { sAlarm.AlarmTime.Seconds += 0x6; }	//check & correct over 9
	if (sAlarm.AlarmTime.Seconds >= 0x59) { sAlarm.AlarmTime.Seconds = 0x01; }		//check & reset after 1 minute
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 0x1;

	HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD);		//activate interrupt RTC AlarmA

	RTC_AlarmA_flag = 0;	//reset flag
}


/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
