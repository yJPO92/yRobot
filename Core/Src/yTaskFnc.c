/**
 *******************************************************************************
 * @file    yTaskFnc.c
 * @author  Jean
 * @brief   
 * @version    
 *******************************************************************************
 * Modified :
 * Created  : 23 sept. 2020
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

extern UART_HandleTypeDef huart2;
extern osThreadId_t tk_VTafficheHandle;
extern osMessageQueueId_t qVTafficheHandle;
extern osSemaphoreId_t semUARTHandle;

/* USER CODE BEGIN Header_tk_VTaffiche_Fnc */
/**
 * @brief Function implementing the tk_VTaffiche thread.
 * @param argument: Not used
 * @retval None
 */
//yDOC task 8 VT Affiche
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



//That's all folks!!
