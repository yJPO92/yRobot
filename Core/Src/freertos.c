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
#include <math.h>
#include "rtc.h"
#include "yTask.h"
#include "VT100.h"
#include "yMENU.h"
#include "yANALOG.h"
#include "yMOTOR.h"

#define NR_VAR_GLO_
#include "CubeMon.h"

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
char aTxBuffer[2048] = {};		//buffer d'emission
uint16_t uart2NbCar = 1;		//nb de byte attendu
uint8_t aRxBuffer[3] __attribute__((section(".myvars")));		//buffer de reception at specific address
char tmpBuffer[10];		    	//buffer temporaire pour switch/case
//----- buffer DMA / ADC1
uint32_t adcbuf[2];
uint32_t adc1_value[2];

uint8_t TkToStart = TkNone;		//pour scheduler le démarrage des taches
uint16_t WaitInTk;

/* Real Time Clock */
RTC_TimeTypeDef myTime;
RTC_DateTypeDef myDate;

extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim4;

yMENU_t mnuSTM;		// pour Menu VT100
yANALOG VRx, VRy;	// pour Joystick
yMOTOR Moteur_D;	// les moteurs

/* objet d'echange d'affichage via la queue */
yVTbuff_t VTbuffer = { .src = SrcNone, .VTbuff = "...\0" };

/* objet d'echange d'event via la queue */
yEvent_t yEvent = {.Topic = TopicNone, .PayLoadF = 0.0, .PayloadI = 0};

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
/* Definitions for t1s */
osTimerId_t t1sHandle;
const osTimerAttr_t t1s_attributes = {
  .name = "t1s"
};
/* Definitions for t250ms */
osTimerId_t t250msHandle;
const osTimerAttr_t t250ms_attributes = {
  .name = "t250ms"
};
/* Definitions for semUART */
osSemaphoreId_t semUARTHandle;
const osSemaphoreAttr_t semUART_attributes = {
  .name = "semUART"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
extern void Interrputs_Init(void);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void tk_Init_Fnc(void *argument);
void tk_CheckVR_Fnc(void *argument);
void tk_Process_Fnc(void *argument);
void tk_VTaffiche_Fnc(void *argument);
void t1s_Callback(void *argument);
void t250ms_Callback(void *argument);

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

  /* Create the timer(s) */
  /* creation of t1s */
  t1sHandle = osTimerNew(t1s_Callback, osTimerPeriodic, NULL, &t1s_attributes);

  /* creation of t250ms */
  t250msHandle = osTimerNew(t250ms_Callback, osTimerPeriodic, NULL, &t250ms_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */

  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of qEvents */
  qEventsHandle = osMessageQueueNew (8, sizeof(yEvent_t), &qEvents_attributes);

  /* creation of qVTaffiche */
  qVTafficheHandle = osMessageQueueNew (15, sizeof(yVTbuff_t), &qVTaffiche_attributes);

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

	WaitInTk = Wait500ms;
	osDelay(pdMS_TO_TICKS(WaitInTk));
	TkToStart = TkInit;
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
//yDOC: task 5 default
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	//-- Is it to me to start?
	while (TkToStart != TkDefault) {
		osDelay(pdMS_TO_TICKS(WaitInTk));
	}
	snprintf(aTxBuffer, 50, DECRC "\n tk_Default\t initialised" DECSC);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);

    /* Start PWM signals generation TIM4 channel 2 */
    if (HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2) != HAL_OK)
    {
    	Error_Handler();	//PWM Generation Error
    }
    htim4.Instance->CCR2 = 10;

	TkToStart++;
	osDelay(pdMS_TO_TICKS(WaitInTk));
	//osThreadTerminate(defaultTaskHandle);
	/* Infinite loop */
	for(;;)
	{
		osDelay(pdMS_TO_TICKS(241));

		//--- Récupérer date & heure
		HAL_RTC_GetTime(&hrtc, &myTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &myDate, RTC_FORMAT_BIN);	//need to read also the date!!!
		//--- afficher date & heure ds zone encadrée et mettre curseur sur ligne status
		snprintf(VTbuffer.VTbuff, 50, CUP(4,61) "%02d-%02d-%02d" CUP(5,61) "%02d:%02d:%02d",
									myDate.Date, myDate.Month, myDate.Year,
									myTime.Hours, myTime.Minutes, myTime.Seconds);
		osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);	//envoi vers task afficahge

		//--- Clear Status Bar & traces
		if (RTC_AlarmA_flag == 1) {
			mnuSTM.ClearStatusBar(&mnuSTM);	//CLS Bar
			snprintf(VTbuffer.VTbuff, 50, "%s", mnuSTM.Buffer);
			osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);	//envoi vers task afficahge
			RTC_AlarmModify();	//relancer alarme ds qq sec
		}

		//--- debug yMOTOR
		yMOTOR_Exec(&Moteur_D);		//calculer le moteur
		snprintf(VTbuffer.VTbuff, 50, CUP(17,50) "--Mot_D: run %d, speedSP %6.2f",
											Moteur_D.inRun, Moteur_D.Speed_SP);
		osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);	//envoi vers task afficahge

		//--- get/set data for STM32CubeMonitor
		yCopy2CubeMonitor(1U);		//set data
		yCopy2CubeMonitor(0U);		//get data

		//--- check PWM
		uint32_t yccr2 = (uint32_t) (adcbuf[0] * 100 / 4096);
		//uint32_t yccr2 = VRx.PVa;
		Moteur_D.Speed_MV = yccr2;
		snprintf(VTbuffer.VTbuff, 50, CUP(18,50) "--pwm %d  ", (int)yccr2);
		osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);	//envoi vers task afficahge

	    //htim4.Instance->CCR2 = yccr2;

	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_tk_Init_Fnc */
/**
 * @brief Function implementing the tk_Init thread.
 * @param argument: Not used
 * @retval None
 */
//yDOC task 1 Init
/* USER CODE END Header_tk_Init_Fnc */
void tk_Init_Fnc(void *argument)
{
  /* USER CODE BEGIN tk_Init_Fnc */
	//-- Is it to me to start?
	while (TkToStart != TkInit) {
		snprintf(aTxBuffer, 50, DECRC "\n tk_Init\t waiting %d" DECSC, TkToStart);
		osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
		HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
		osSemaphoreRelease(semUARTHandle);
		osDelay(pdMS_TO_TICKS(WaitInTk));
	}	//wait here!

	snprintf(aTxBuffer, 50, DECRC "\n tk_Init\t initialised" DECSC);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);

	TkToStart++;
	osDelay(pdMS_TO_TICKS(WaitInTk));

	while (TkToStart != TkAll) {		//wait here!
		osDelay(pdMS_TO_TICKS(WaitInTk));
	}
	snprintf(aTxBuffer, 1024, DECRC "\n tk_Init\t All task initialised %d\n"
									yPROG " " yVER " " yCubeMX "\n\n" DECSC, TkToStart);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);


	/* Initialize menu structure & Message de Bienvenue*/
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

	//--- Mise a heure RTC
	RTC_MiseAheure();
	//--- initialize Joystick (direction & vitesse pour moteurs)
	yANALOG_Init(&VRx);
	VRx.Trim = 6.30;
	yANALOG_Init(&VRy);
	VRy.Trim = 3.90;
	//--- initialize Moteurs
	//yMOTOR_Init(&Moteur_D, (uint32_t)LD2_GPIO_Port, (uint16_t)LD2_Pin);
	yMOTOR_Init(&Moteur_D, (uint32_t)MotDin1_GPIO_Port, (uint16_t)MotDin1_Pin,
					  (uint32_t)MotDin2_GPIO_Port, (uint16_t)MotDin2_Pin, htim4);


	//--- initialize interrupts
	Interrputs_Init();

	osDelay(WaitInTk);
	/* Infinite loop */
	for(;;)
	{

		osDelay(pdMS_TO_TICKS(503));

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
//yDOC task 3 check VR
/* USER CODE END Header_tk_CheckVR_Fnc */
void tk_CheckVR_Fnc(void *argument)
{
  /* USER CODE BEGIN tk_CheckVR_Fnc */
	static uint8_t i = 0, j = 0;

	//-- Is it to me to start?
	while (TkToStart != TkCheckVR) {
		osDelay(pdMS_TO_TICKS(WaitInTk));
	}

	snprintf(aTxBuffer, 50, DECRC "\n tk_CheckVR\t initialised" DECSC);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);

	TkToStart++;

	//-- attendre les autres taches
	while (TkToStart != TkAll) {		//wait here!
		osDelay(pdMS_TO_TICKS(WaitInTk));
	}
	/* Infinite loop */
	for(;;)
	{
		osDelay(pdMS_TO_TICKS(251));
		//--- VRx (direction)
		yANALOG_CalulerPV(&VRx);
		if (yANALOG_Variation(&VRx) != 0U) {
			snprintf(VTbuffer.VTbuff, 50, CUP(13,50) "--VRx  : %d", i++);
			osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);	//envoi vers task afficahge

			yEvent_t yEvent = {.Topic = VRX, .PayLoadF = VRx.PV, .PayloadI = 0};
			osMessageQueuePut(qEventsHandle, &yEvent, 0U, portMAX_DELAY);
		}

		//--- VRy (vitesse)
		yANALOG_CalulerPV(&VRy);
		if (yANALOG_Variation(&VRy) != 0U) {
			snprintf(VTbuffer.VTbuff, 50, CUP(14,50) "--VRy  : %d", j++);
			osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY); //envoi vers task afficahge

			yEvent_t yEvent = {.Topic = VRY, .PayLoadF = VRy.PV, .PayloadI = 0};
			osMessageQueuePut(qEventsHandle, &yEvent, 0U, portMAX_DELAY);
		}

		//snprintf(VTbuffer.VTbuff, 50, CUP(12,50) "--VR   : %6.2f \t %6.2f", VRx.PV, VRy.PV);	// 'IMPRECISERR' corrigé par modif syscall.c & .ld (v1.3)
		snprintf(VTbuffer.VTbuff, 50, CUP(12,50) "--VR   : (%d) %6.2f \t (%d) %6.2f",
														VRx.sens, VRx.PVa, VRy.sens, VRy.PVa);	// 'IMPRECISERR' corrigé par modif syscall.c & .ld (v1.3)
		osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);	//envoi vers task afficahge

	}
  /* USER CODE END tk_CheckVR_Fnc */
}

/* USER CODE BEGIN Header_tk_Process_Fnc */
/**
 * @brief Function implementing the tk_Process thread.
 * @param argument: Not used
 * @retval None
 */
//yDOC task 4 Process
/* USER CODE END Header_tk_Process_Fnc */
void tk_Process_Fnc(void *argument)
{
  /* USER CODE BEGIN tk_Process_Fnc */
	yEvent_t EvtRecu;
	osStatus_t status;
	//-- Is it to me to start?
	while (TkToStart != TkProcess) {
		osDelay(pdMS_TO_TICKS(WaitInTk));
	}
	snprintf(aTxBuffer, 1024, DECRC "\n tk_Process\t initialised" DECSC);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);

	TkToStart++;
	osDelay(pdMS_TO_TICKS(WaitInTk));
	/* Infinite loop */
	for(;;)
	{
		/* wait for message in queue */
		status = osMessageQueueGet(qEventsHandle, &EvtRecu, NULL, portMAX_DELAY);
		if (status == osOK) {
			//-- tracer l'event recu
			snprintf(VTbuffer.VTbuff, 100, DECRC "\tEventRec: %d %6.2f %d" ERASELINE,
					EvtRecu.Topic, EvtRecu.PayLoadF, EvtRecu.PayloadI);
			osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);

			switch (EvtRecu.Topic) {
			case BP1:
				snprintf(VTbuffer.VTbuff, 50, CUP(9,50) "--(tk_Proc) BP1 Interrupt");
				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
				//-- action?
				break;
			case SWXY:
				snprintf(VTbuffer.VTbuff, 50, CUP(10,50) "--(tk_Proc) Swxy Interrupt");
				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
				//-- request moteur Run
				(Moteur_D.inRun == 1U) ? (yMOTOR_MarArr(&Moteur_D, yARRET)) : (yMOTOR_MarArr(&Moteur_D, yMARCHE));
				break;
			case Kbd:
				//-- request moteur Run
				//Moteur_D.MarArr = EvtRecu.PayloadI;
				yMOTOR_MarArr(&Moteur_D, EvtRecu.PayloadI);
				break;
			case VRX:	//direction
				//-- action ?
				break;
			case VRY:	//Vitesse
				//-- request moteur consigne vitesse
				Moteur_D.Speed_SP = EvtRecu.PayLoadF;
				break;
			default:
				snprintf(VTbuffer.VTbuff, 50, DECRC "\ttk_Process: unknow event" ERASELINE);
				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
				break;
			}	// switch EvtRecu
		} else {
			snprintf(aTxBuffer, 512, DECRC "Queue empty" DECRC);
			osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
			HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
			osSemaphoreRelease(semUARTHandle);
		}
	}
  /* USER CODE END tk_Process_Fnc */
}

/* USER CODE BEGIN Header_tk_VTaffiche_Fnc */
/**
 * @brief Function implementing the tk_VTaffiche thread.
 * @param argument: Not used
 * @retval None
 */
//yDOC task 2 VT Affiche
/* USER CODE END Header_tk_VTaffiche_Fnc */
void tk_VTaffiche_Fnc(void *argument)
{
  /* USER CODE BEGIN tk_VTaffiche_Fnc */
	yVTbuff_t BuffAff;
	osStatus_t status;
	//-- Is it to me to start?
	while (TkToStart != TkVTaffiche) {
		osDelay(pdMS_TO_TICKS(WaitInTk));
	}

	snprintf(aTxBuffer, 1024, DECRC "\n tk_VTaffiche\t initialised" DECSC);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);

	TkToStart++;
	osDelay(pdMS_TO_TICKS(WaitInTk));
	osDelay(pdMS_TO_TICKS(2000));	//temps de voir l'affichage
	/* Infinite loop */
	for(;;)
	{
		/* wait for message in queue */
		status = osMessageQueueGet(qVTafficheHandle, &BuffAff, NULL, portMAX_DELAY);
		if (status == osOK) {
			//snprintf(aTxBuffer, 512, DECRC "%s" ERASELINE, BuffAff.VTbuff);
			snprintf(aTxBuffer, 512, DECRC "%s", BuffAff.VTbuff);
		} else {
			snprintf(aTxBuffer, 512, DECRC "Queue empty" DECRC);
		}
		//ici il faut bien ecrire via l'UART!!!
		osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);
		HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
		osSemaphoreRelease(semUARTHandle);
		__NOP();

	}
  /* USER CODE END tk_VTaffiche_Fnc */
}

/* t1s_Callback function */
__weak void t1s_Callback(void *argument)
{
  /* USER CODE BEGIN t1s_Callback */
  
  /* USER CODE END t1s_Callback */
}

/* t250ms_Callback function */
__weak void t250ms_Callback(void *argument)
{
  /* USER CODE BEGIN t250ms_Callback */
  
  /* USER CODE END t250ms_Callback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
