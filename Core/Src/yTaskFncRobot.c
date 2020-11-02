/**
 *******************************************************************************
 * @file    yTaskFncRobot.c
 * @author  Jean
 * @brief   Taches relatives au Robot
 * @version 1.0
 *******************************************************************************
 * Modified :
 * Created  : 31 oct. 2020
 *******************************************************************************
 * @note    
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "rtc.h"
#include "yTask.h"
#include "VT100.h"
#include "yMENU.h"
#include "yANALOG.h"
#include "yMOTOR.h"
#include "yTRAIN.h"

#define NR_VAR_GLO_
#include "CubeMon.h"

/* Variables -----------------------------------------------------------------*/
extern uint8_t TkToStart;
extern uint16_t WaitInTk;
extern char aTxBuffer[];
extern yVTbuff_t VTbuffer;

extern UART_HandleTypeDef huart2;
extern osThreadId_t tk_VTafficheHandle;
extern osSemaphoreId_t semUARTHandle;

extern osMessageQueueId_t qVTafficheHandle;
extern osMessageQueueId_t qEventsHandle;
extern osMessageQueueId_t qTrainHandle;
extern osMessageQueueId_t qMotDHandle;
extern osMessageQueueId_t qMotGHandle;

// TODO v34a DBG a supprimer
yMOTOR Moteur_D;
yMOTOR Moteur_G;
yTRAIN TrainDG;

/* Tasks ---------------------------------------------------------------------*/

/* USER CODE BEGIN Header_tk_Process_Fnc */
/**
 * @brief Function implementing the tk_Process thread.
 * @param argument: Not used
 * @retval None
 */
//yDOC task 6 Process
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
//			snprintf(VTbuffer.VTbuff, 100, DECRC "\tEventRec: %d %6.2f %d" ERASELINE,
//					EvtRecu.Topic, EvtRecu.PayLoadF, EvtRecu.PayloadI);
//			osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
			//-- analyser le msg par le Topic
			//en attendant le traitement
			snprintf(VTbuffer.VTbuff, 50, CUP(18,50) "--(tk_Train) pass Event");
			osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
//			//transmettre le msg à la tache suivante 'tk_Train'!!!
//			yEvent = EvtRecu;
//			osMessageQueuePut(qTrainHandle,&yEvent, 0U, portMAX_DELAY);

			switch (EvtRecu.Topic) {
//			case BP1:
//				snprintf(VTbuffer.VTbuff, 50, CUP(17,50) "--(tk_Proc) Interrupt BP1  ");
//				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
//				//-- action?
//				//-- debug envoyer msg à tk_Train via qTrain
//				yEvent_t yEvent = {.Topic= BP1, .PayLoadF = 0.0, .PayloadI = 0};
//				osMessageQueuePut(qTrainHandle,&yEvent, 0U, portMAX_DELAY);
//				break;
//			case SWXY:
//				snprintf(VTbuffer.VTbuff, 50, CUP(17,50) "--(tk_Proc) Interrupt SWxy  ");
//				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
//				break;
			case Kbd:
				//-- request Train Run or not
				__NOP();
				yEvent_t yEvent = {.Topic = MarArr, .PayLoadF = 0.0, .PayloadI = EvtRecu.PayloadI};
				osMessageQueuePut(qTrainHandle,&yEvent, 0U, portMAX_DELAY);
				//TODO v3.4a DBG debug test
				__NOP();
				yEvent.Topic = VRY;
				yEvent.PayLoadF = 15.0;
				osMessageQueuePut(qTrainHandle,&yEvent, 0U, portMAX_DELAY);

				break;
//			case VRX:	//Direction
//				//-- action ?
//				//use queue to send to tk_Train
//				break;
//			case VRY:	//Vitesse
//				//-- request moteur consigne vitesse
//				//TODO futur
//				//use queue to send to tk_Train
//				break;
			default:
				snprintf(VTbuffer.VTbuff, 50, DECRC "\ttk_Process: pass event" ERASELINE);
				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
				//transmettre le msg à la tache suivante 'tk_Train'!!!
				yEvent = EvtRecu;
				osMessageQueuePut(qTrainHandle,&yEvent, 0U, portMAX_DELAY);
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

/* USER CODE BEGIN Header_tk_Train_Fnc */
/**
* @brief Function implementing the tk_Train thread.
* @param argument: Not used
* @retval None
*/
//yDOC task 5 Train
/* USER CODE END Header_tk_Train_Fnc */
void tk_Train_Fnc(void *argument)
{
  /* USER CODE BEGIN tk_Train_Fnc */
	yEvent_t EvtRecu;
	osStatus_t status;
	//-- Declare structure du Train
	//TODO v3.4a global for debug
	//yTRAIN TrainDG;
	//-- Is it to me to start?
	while (TkToStart != TkTrain) {
		osDelay(pdMS_TO_TICKS(WaitInTk));
	}
	//-- Initialize structure of Moteur_D
	yTRAIN_Init(&TrainDG);
	//-- msg on VT
	snprintf(aTxBuffer, 1024, DECRC "\n tk_Train\t initialised" DECSC);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);

	TkToStart++;
	osDelay(pdMS_TO_TICKS(WaitInTk));
  /* Infinite loop */
  for(;;)
  {
		/* wait for message in queue */
		status = osMessageQueueGet(qTrainHandle, &EvtRecu, NULL, portMAX_DELAY);
		if (status == osOK) {
			//-- tracer l'event recu
//			snprintf(VTbuffer.VTbuff, 100, DECRC "\tEventRec: %d %6.2f %d" ERASELINE,
//								EvtRecu.Topic, EvtRecu.PayLoadF, EvtRecu.PayloadI);
//			osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
			//-- en fonction du Topic faire...
			switch (EvtRecu.Topic) {
//			case BP1:
//				//-- action?
//				__NOP();
//				//TODO traiter les Topic pour faire action yTRAIN
//				//-- debug envoyer msg à tk_MoteurD via qMotD
//				//yEvent_t yEvent = {.Topic= BP1, .PayLoadF = 0.0, .PayloadI = 0};
//				yEvent_t yEvent = {.Topic = BP1, .PayLoadF = 0.0, .PayloadI = 0};
//				osMessageQueuePut(qMotDHandle,&yEvent, 0U, portMAX_DELAY);
//				break;
			case SWXY:
				snprintf(VTbuffer.VTbuff, 50, CUP(17,50) "--(tk_Proc) Interrupt SWxy  ");
				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
				(TrainDG.MaAr == 1) ? (TrainDG.MaAr = 0) : (TrainDG.MaAr = 1);	//toggle running request of TrainDG
				//-- following Train running state prepare event marche/arret to send to tk_MoteurD & tk_MoteurG
				if (TrainDG.MaAr == 0U) {
					TrainDG.MaAr_D = 0U;
					TrainDG.MaAr_G = 0U;
					 yEvent_t yEvent = {.Topic = Arr, .PayLoadF = 0.0, .PayloadI = 0};
					osMessageQueuePut(qMotDHandle, &yEvent, 0U, portMAX_DELAY);
					osMessageQueuePut(qMotGHandle, &yEvent, 0U, portMAX_DELAY);
				} else {
					TrainDG.MaAr_D = 1U;
					TrainDG.MaAr_G = 1U;
					yEvent_t yEvent = {.Topic = Mar, .PayLoadF = 0.0, .PayloadI = 1};
					osMessageQueuePut(qMotDHandle, &yEvent, 0U, portMAX_DELAY);
					osMessageQueuePut(qMotGHandle, &yEvent, 0U, portMAX_DELAY);
				}
				break;
			case MarArr:
				snprintf(VTbuffer.VTbuff, 50, CUP(17,50) "--(tk_Proc) recu MarArr  ");
				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
				//-- state of 'Train'
				TrainDG.MaAr = EvtRecu.PayloadI;
				//-- following Train running state prepare event marche/arret to send to tk_MoteurD & tk_MoteurG
				if (TrainDG.MaAr == 0U) {
					TrainDG.MaAr_D = 0U;
					TrainDG.MaAr_G = 0u;
					yEvent_t yEvent = {.Topic = Arr, .PayLoadF = 0.0, .PayloadI = 0};
					osMessageQueuePut(qMotDHandle, &yEvent, 0U, portMAX_DELAY);
					osMessageQueuePut(qMotGHandle, &yEvent, 0U, portMAX_DELAY);
				} else {
					TrainDG.MaAr_D = 1U;
					TrainDG.MaAr_G = 1U;
					yEvent_t yEvent = {.Topic = Mar, .PayLoadF = 0.0, .PayloadI = 1};
					osMessageQueuePut(qMotDHandle, &yEvent, 0U, portMAX_DELAY);
					osMessageQueuePut(qMotGHandle, &yEvent, 0U, portMAX_DELAY);
				}
				break;
//			case VRX:	//Direction
//				//-- action ?
//				//use queue to send to tk_MoteurD & tk_MoteurG
//				break;
			case VRY:	//Vitesse
				__NOP();
				if ((EvtRecu.PayLoadF >= 0.5) || (EvtRecu.PayLoadF <= -0.5)) {
					TrainDG.inRun = 1U;
					TrainDG.Vitesse = EvtRecu.PayLoadF;
					//-- prepare event vitesse to send to tk_MoteurD & tk_MoteurG
					yEvent_t yEvent = {.Topic = SP, .PayLoadF = EvtRecu.PayLoadF, .PayloadI = 0};
					osMessageQueuePut(qMotDHandle, &yEvent, 0U, portMAX_DELAY);
					osMessageQueuePut(qMotGHandle, &yEvent, 0U, portMAX_DELAY);
				} else {
					TrainDG.inRun = 0U;
					TrainDG.Vitesse = 0.0;
				}
				break;
			default:
				snprintf(VTbuffer.VTbuff, 50, DECRC "\ttk_Train: unknow event" ERASELINE);
				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
//				osMessageQueuePut(qMotDHandle, &yEvent, 0U, portMAX_DELAY);
//				osMessageQueuePut(qMotGHandle, &yEvent, 0U, portMAX_DELAY);
				break;
			}	// switch EvtRecu

			//-- puis envoyer un msg à l'afficheur
			snprintf(VTbuffer.VTbuff, 100, CUP(22,50) "--Train: ma %d, r %d, Vit %6.1f, Dir %6.1f  ",
												TrainDG.MaAr ,TrainDG.inRun, TrainDG.Vitesse, TrainDG.Direction);
			osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);

//			//--- trace via LD2
//			//TODO LD2 as running motors how???
//			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, TrainDG.inRun);

		} else {
			snprintf(aTxBuffer, 512, DECRC "Queue qTrain empty" DECRC);
			osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
			HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
			osSemaphoreRelease(semUARTHandle);
		}	//if status
  }
  /* USER CODE END tk_Train_Fnc */
}

/* USER CODE BEGIN Header_tk_MoteurD_Fnc */
/**
* @brief Function implementing the tk_MoteurD thread.
* @param argument: Not used
* @retval None
*/
//yDOC task 3 MoteurD
/* USER CODE END Header_tk_MoteurD_Fnc */
void tk_MoteurD_Fnc(void *argument)
{
  /* USER CODE BEGIN tk_MoteurD_Fnc */
	yEvent_t EvtRecu;
	osStatus_t status;
	//-- Declare structure du Moteur_D
	//TODO v3.4a global for debug
	//yMOTOR Moteur_D;
	//-- Is it to me to start?
	while (TkToStart != TkMoteurD) {
		osDelay(pdMS_TO_TICKS(WaitInTk));
	}
	//-- Initialize structure of Moteur_D
	yMOTOR_Init(&Moteur_D, (uint32_t)MotDin1_GPIO_Port, (uint16_t)MotDin1_Pin,
					  (uint32_t)MotDin2_GPIO_Port, (uint16_t)MotDin2_Pin, htim4);
	//-- msg on VT
	snprintf(aTxBuffer, 1024, DECRC "\n tk_MoteurD\t initialised" DECSC);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);

	TkToStart++;
	osDelay(pdMS_TO_TICKS(WaitInTk));
	/* Infinite loop */
	for(;;)
	{
		/* wait for message in queue */
		status = osMessageQueueGet(qMotDHandle, &EvtRecu, NULL, portMAX_DELAY);
		if (status == osOK) {
			//-- tracer l'event recu
//			snprintf(VTbuffer.VTbuff, 100, DECRC "\tEventRec: %d %6.2f %d" ERASELINE,
//								EvtRecu.Topic, EvtRecu.PayLoadF, EvtRecu.PayloadI);
//			osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
			//-- en fonction du Topic faire...
			//TODO traiter les Topic pour faire action yMOTOR
			switch (EvtRecu.Topic) {
			case Mar:
				//-- action?
				//-- request moteurs Run or not
				yMOTOR_MarArr(&Moteur_D, yMARCHE);
				//-- msg to VT
				snprintf(VTbuffer.VTbuff, 50, CUP(19,50) "--(tk_MotD) Marche request   ");
				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
				break;
			case Arr:
				//-- action?
				//-- request moteurs Run or not
				yMOTOR_MarArr(&Moteur_D, yARRET);
				//-- msg to VT
				snprintf(VTbuffer.VTbuff, 50, CUP(19,50) "--(tk_MotD) Arret request   ");
				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
				break;
			case SP:
				//-- action?
				yMOTOR_Speed(&Moteur_D, EvtRecu.PayLoadF);
				//-- msg to VT
				snprintf(VTbuffer.VTbuff, 50, CUP(19,50) "--(tk_MotD) speed request   ");
				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
				break;
			default:
				snprintf(VTbuffer.VTbuff, 50, DECRC "\ttk_MoteurD: unknow event" ERASELINE);
				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
				break;
			}	// switch EvtRecu
			//-- puis envoyer un msg d'état du moteur à l'afficheur
			snprintf(VTbuffer.VTbuff, 100, CUP(23,50) "--Mot_D: ma %d, r %d, SP %6.1f, s %d, MV %3ld, rl %d ",
												Moteur_D.MarArr, Moteur_D.inRun, Moteur_D.Speed_SP,
												Moteur_D.Sens, Moteur_D.Speed_MV, Moteur_D.FeeWheel);
			osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);

			//--- trace via LD2
			//TODO LD2 as running motors how???
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, Moteur_D.inRun);

		} else {
			snprintf(aTxBuffer, 512, DECRC "Queue qMotD empty" DECRC);
			osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
			HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
			osSemaphoreRelease(semUARTHandle);
		}	//if status

		//-- send data to STM32CubeMonitor
		ymx_MotD = Moteur_D;
	}
  /* USER CODE END tk_MoteurD_Fnc */
}

/* USER CODE BEGIN Header_tk_MoteurG_Fnc */
/**
* @brief Function implementing the tk_MoteurG thread.
* @param argument: Not used
* @retval None
*/
//yDOC task 4 MoteurG
/* USER CODE END Header_tk_MoteurG_Fnc */
void tk_MoteurG_Fnc(void *argument)
{
  /* USER CODE BEGIN tk_MoteurG_Fnc */
	yEvent_t EvtRecu;
	osStatus_t status;
	//-- Declare structure du Moteur_G
	//TODO v3.4a global for debug
	//yMOTOR Moteur_G;
	//-- Is it to me to start?
	while (TkToStart != TkMoteurG) {
		osDelay(pdMS_TO_TICKS(WaitInTk));
	}
	//-- Initialize structure of Moteur_G
//	yMOTOR_Init(&Moteur_G, (uint32_t)MotGin1_GPIO_Port, (uint16_t)MotGin1_Pin,
//					  (uint32_t)MotGin2_GPIO_Port, (uint16_t)MotGin2_Pin, htim3);
	yMOTOR_Init(&Moteur_G, 0, 0, 0, 0, htim4);
	//-- msg on VT
	snprintf(aTxBuffer, 1024, DECRC "\n tk_MoteurG\t initialised" DECSC);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
	osSemaphoreRelease(semUARTHandle);

	TkToStart++;
	osDelay(pdMS_TO_TICKS(WaitInTk));
  /* Infinite loop */
  for(;;)
  {
		/* wait for message in queue */
		status = osMessageQueueGet(qMotGHandle, &EvtRecu, NULL, portMAX_DELAY);
		if (status == osOK) {
			//-- tracer l'event recu
//			snprintf(VTbuffer.VTbuff, 100, DECRC "\tEventRec: %d %6.2f %d" ERASELINE,
//								EvtRecu.Topic, EvtRecu.PayLoadF, EvtRecu.PayloadI);
//			osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
			//-- en fonction du Topic faire...
			//TODO traiter les Topic pour faire action yMOTOR
			switch (EvtRecu.Topic) {
			case Mar:
				//-- action?
				//-- request moteurs Run or not
				yMOTOR_MarArr(&Moteur_G, yMARCHE);
				//-- msg to VT
				snprintf(VTbuffer.VTbuff, 50, CUP(20,50) "--(tk_MotG) Marche request   ");
				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
				break;
			case Arr:
				//-- action?
				//-- request moteurs Run or not
				yMOTOR_MarArr(&Moteur_G, yARRET);
				//-- msg to VT
				snprintf(VTbuffer.VTbuff, 50, CUP(20,50) "--(tk_MotG) Arret request   ");
				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
				break;
			default:
				snprintf(VTbuffer.VTbuff, 50, DECRC "\ttk_MoteurG: unknow event" ERASELINE);
				osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);
				break;
			}	// switch EvtRecu
			//-- puis envoyer un msg d'état du moteur à l'afficheur
			snprintf(VTbuffer.VTbuff, 100, CUP(24,50) "--Mot_G: ma %d, r %d, SP %6.1f, s %d, MV %3ld, rl %d ",
												Moteur_G.MarArr, Moteur_G.inRun, Moteur_G.Speed_SP,
												Moteur_G.Sens, Moteur_G.Speed_MV, Moteur_G.FeeWheel);
			osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);

			//--- trace via LD2
			//TODO LD2 as running motors how???
			//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, Moteur_D.inRun);

		} else {
			snprintf(aTxBuffer, 512, DECRC "Queue qMotG empty" DECRC);
			osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
			HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
			osSemaphoreRelease(semUARTHandle);
		}	//if status

		//-- send data to STM32CubeMonitor
		ymx_MotG = Moteur_G;
  }
  /* USER CODE END tk_MoteurG_Fnc */
}

/**
 * @note \n
 *         That's all folks!
 */
