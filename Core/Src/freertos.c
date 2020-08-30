/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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

#ifdef __cplusplus
extern "C" {
#endif

/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include <stdio.h>
#include <string.h>
//#include <math.h>
#include "yTask.h"
#include "VT100.h"
#include "yMENU.h"

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
/* USER CODE BEGIN Variables */
//----- pour usart2, gestion interface sur console VT
char aTxBuffer[2048];			//buffer d'emission
uint16_t uart2NbCar = 1;		//nb de byte attendu
uint8_t aRxBuffer[3] __attribute__((section(".myvars")));		//buffer de reception at specific address
char tmpBuffer[10];		    	//buffer temporaire pour switch/case
////----- buffer DMA / ADC1
#define ADCBUFSIZE	2
uint32_t adcbuf[ADCBUFSIZE];
uint32_t adc1_value[ADCBUFSIZE];

uint8_t TkToStart = TkNone;		//pour scheduler le démarrage des taches

extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim1;

uint16_t WaitInTk;

yMENU_t mnuSTM;


/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 4
};
/* Definitions for tk_Init */
osThreadId_t tk_InitHandle;
const osThreadAttr_t tk_Init_attributes = {
  .name = "tk_Init",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 4
};
/* Definitions for tk_CheckVR */
osThreadId_t tk_CheckVRHandle;
const osThreadAttr_t tk_CheckVR_attributes = {
  .name = "tk_CheckVR",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 4
};
/* Definitions for tk_Process */
osThreadId_t tk_ProcessHandle;
const osThreadAttr_t tk_Process_attributes = {
  .name = "tk_Process",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 4
};
/* Definitions for tk_VTaffiche */
osThreadId_t tk_VTafficheHandle;
const osThreadAttr_t tk_VTaffiche_attributes = {
  .name = "tk_VTaffiche",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 4
};
/* Definitions for qEvents */
osMessageQueueId_t qEventsHandle;
const osMessageQueueAttr_t qEvents_attributes = {
  .name = "qEvents"
};
/* Definitions for qVTaffiche */
osMessageQueueId_t qVTafficheHandle;
const osMessageQueueAttr_t qVTaffiche_attributes = {
  .name = "qVTaffiche"
};
/* Definitions for semUART */
osSemaphoreId_t semUARTHandle;
const osSemaphoreAttr_t semUART_attributes = {
  .name = "semUART"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void tk_Init_Fnc(void *argument);
void tk_CheckVR_Fnc(void *argument);
void tk_Process_Fnc(void *argument);
void tk_VTaffiche_Fnc(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Pre/Post sleep processing prototypes */
void PreSleepProcessing(uint32_t *ulExpectedIdleTime);
void PostSleepProcessing(uint32_t *ulExpectedIdleTime);

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);
void vApplicationDaemonTaskStartupHook(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
	return 0;
}
/* USER CODE END 1 */

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
	/* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN DAEMON_TASK_STARTUP_HOOK */
void vApplicationDaemonTaskStartupHook(void)
{
}
/* USER CODE END DAEMON_TASK_STARTUP_HOOK */

/* USER CODE BEGIN PREPOSTSLEEP */
__weak void PreSleepProcessing(uint32_t *ulExpectedIdleTime)
{
	/* place for user code */
}

__weak void PostSleepProcessing(uint32_t *ulExpectedIdleTime)
{
	/* place for user code */
}
/* USER CODE END PREPOSTSLEEP */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */


  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of semUART */
  semUARTHandle = osSemaphoreNew(1, 1, &semUART_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of qEvents */
  qEventsHandle = osMessageQueueNew (8, sizeof(yEvent_t), &qEvents_attributes);

  /* creation of qVTaffiche */
  qVTafficheHandle = osMessageQueueNew (15, sizeof(VTbuff_t), &qVTaffiche_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of tk_Init */
  tk_InitHandle = osThreadNew(tk_Init_Fnc, NULL, &tk_Init_attributes);

  /* creation of tk_CheckVR */
  tk_CheckVRHandle = osThreadNew(tk_CheckVR_Fnc, NULL, &tk_CheckVR_attributes);

  /* creation of tk_Process */
  tk_ProcessHandle = osThreadNew(tk_Process_Fnc, NULL, &tk_Process_attributes);

  /* creation of tk_VTaffiche */
  tk_VTafficheHandle = osThreadNew(tk_VTaffiche_Fnc, NULL, &tk_VTaffiche_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */

	osDelay(pdMS_TO_TICKS(WaitInTk));
	snprintf(aTxBuffer, 1024, clrscr homescr "\n FreeRTOS initialised" DECSC);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);
	osDelay(pdMS_TO_TICKS(WaitInTk));

	WaitInTk = Wait1s;
	TkToStart = TkInit;
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	//-- Is it to me to start?
	while (TkToStart != TkDefault) {
//		snprintf(aTxBuffer, 1024, DECRC "\n tk_Default\t waiting %d" DECSC, TkToStart);
//		osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
//		HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//		osSemaphoreRelease(semUARTHandle);
		osDelay(pdMS_TO_TICKS(WaitInTk));
	}	//wait here!

	snprintf(aTxBuffer, 1024, DECRC "\n tk_Default\t initialised" DECSC);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);

	osDelay(pdMS_TO_TICKS(WaitInTk));
	TkToStart++;
	osDelay(pdMS_TO_TICKS(5000));
	osThreadTerminate(defaultTaskHandle);
	/* Infinite loop */
	for(;;)
	{
		//start_cpp();
		osDelay(pdMS_TO_TICKS(250));
	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_tk_Init_Fnc */
/**
 * @brief Function implementing the tk_Init thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_tk_Init_Fnc */
void tk_Init_Fnc(void *argument)
{
  /* USER CODE BEGIN tk_Init_Fnc */
	//-- Is it to me to start?
	while (TkToStart != TkInit) {
//		snprintf(aTxBuffer, 1024, DECRC "\n tk_Init\t waiting %d" DECSC, TkToStart);
//		osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
//		HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//		osSemaphoreRelease(semUARTHandle);
		osDelay(pdMS_TO_TICKS(WaitInTk));
	}	//wait here!

	snprintf(aTxBuffer, 1024, DECRC "\n tk_Init\t initialised\n");
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);

	osDelay(pdMS_TO_TICKS(WaitInTk));
	TkToStart++;

	while (TkToStart != TkAll) {		//wait here!
		osDelay(pdMS_TO_TICKS(WaitInTk));
	}
	snprintf(aTxBuffer, 1024, DECRC "\n tk_Init\t All task initialised %d\n"
									yPROG " " yVER " " yCubeMX "\n\n" DECSC, TkToStart);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);

	mnuSTM.Init =Init_fnc;	//assigne function
	mnuSTM.Init(&mnuSTM);	//initialize structure
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) mnuSTM.Buffer, strlen(mnuSTM.Buffer), 5000);
	osSemaphoreRelease(semUARTHandle);
	mnuSTM.Infos(&mnuSTM);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) mnuSTM.Buffer, strlen(mnuSTM.Buffer), 5000);
	osSemaphoreRelease(semUARTHandle);
	mnuSTM.Display(&mnuSTM);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) mnuSTM.Buffer, strlen(mnuSTM.Buffer), 5000);
	osSemaphoreRelease(semUARTHandle);

	//--- start USART2
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	//--- start ADC acquisition via DMA
	HAL_ADC_Start_DMA(&hadc1,(uint32_t *)adcbuf,ADCBUFSIZE);
	//--- start TIM1 (ADC1 schedule)
	//HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Base_Start(&htim1);

	osDelay(WaitInTk);
	/* Infinite loop */
	for(;;)
	{

		osDelay(pdMS_TO_TICKS(500));

		/* Suspend me */
		//osThreadSuspend(tk_InitHandle);
		//osThreadTerminate(tk_InitHandle);
	}
  /* USER CODE END tk_Init_Fnc */
}

/* USER CODE BEGIN Header_tk_CheckVR_Fnc */
/**
 * @brief Function implementing the tk_CheckVR thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_tk_CheckVR_Fnc */
void tk_CheckVR_Fnc(void *argument)
{
  /* USER CODE BEGIN tk_CheckVR_Fnc */
	//-- Is it to me to start?
	while (TkToStart != TkCheckVR) {
//		snprintf(aTxBuffer, 1024, DECRC "\n tk_CheckVR\t waiting %d" DECSC, TkToStart);
//		osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
//		HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//		osSemaphoreRelease(semUARTHandle);
		osDelay(pdMS_TO_TICKS(WaitInTk));
	}	//wait here!

	snprintf(aTxBuffer, 1024, DECRC "\n tk_CheckVR\t initialised" DECSC);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);

	TkToStart++;
	/* cree objets entree analog */

	//-- attendre encore un peu!
	osDelay(pdMS_TO_TICKS(WaitInTk));
	/* Infinite loop */
	for(;;)
	{
		osDelay(pdMS_TO_TICKS(250));
	}
  /* USER CODE END tk_CheckVR_Fnc */
}

/* USER CODE BEGIN Header_tk_Process_Fnc */
/**
 * @brief Function implementing the tk_Process thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_tk_Process_Fnc */
void tk_Process_Fnc(void *argument)
{
  /* USER CODE BEGIN tk_Process_Fnc */
	//-- Is it to me to start?
	while (TkToStart != TkProcess) {
//		snprintf(aTxBuffer, 1024, DECRC "\n tk_Process\t waiting %d" DECSC, TkToStart);
//		osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
//		HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//		osSemaphoreRelease(semUARTHandle);
		osDelay(pdMS_TO_TICKS(WaitInTk));
	} //wait here!

	snprintf(aTxBuffer, 1024, DECRC "\n tk_Process\t initialised" DECSC);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);

	osDelay(pdMS_TO_TICKS(WaitInTk));
	TkToStart++;
	/* Infinite loop */
	for(;;)
	{
		/* toggle LD2 */
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		osDelay(pdMS_TO_TICKS(250));
	}
  /* USER CODE END tk_Process_Fnc */
}

/* USER CODE BEGIN Header_tk_VTaffiche_Fnc */
/**
 * @brief Function implementing the tk_VTaffiche thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_tk_VTaffiche_Fnc */
void tk_VTaffiche_Fnc(void *argument)
{
  /* USER CODE BEGIN tk_VTaffiche_Fnc */
	//-- Is it to me to start?
	while (TkToStart != TkVTaffiche) {
//		snprintf(aTxBuffer, 1024, DECRC "\n tk_VTaffiche\t waiting %d" DECSC, TkToStart);
//		osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
//		HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//		osSemaphoreRelease(semUARTHandle);
		osDelay(pdMS_TO_TICKS(WaitInTk));
	} //wait here!

	snprintf(aTxBuffer, 1024, DECRC "\n tk_VTaffiche\t initialised" DECSC);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);

	osDelay(pdMS_TO_TICKS(WaitInTk));
	TkToStart++;
	/* Infinite loop */
	for(;;)
	{
		osDelay(pdMS_TO_TICKS(250));
	}
  /* USER CODE END tk_VTaffiche_Fnc */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


#ifdef __cplusplus
}
#endif
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
