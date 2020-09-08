/**
 *******************************************************************************
 * @file    Interrupts_Callbacks.c
 * @author  Jean
 * @brief   Callback: BP 1, Keyboard, ADC, RTC_AlarmA
 * @version    
 *******************************************************************************
 * Modified :
 * Created  : 29 août 2020
 *******************************************************************************
 * @note    
 *
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "rtc.h"
//#include <stdio.h>
//#include <string.h>
//#include <math.h>
#include "yTask.h"
#include "VT100.h"
#include "yMENU.h"
#include "yANALOG.h"

extern UART_HandleTypeDef huart2;
extern osSemaphoreId_t semUARTHandle;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim1;
extern osTimerId_t t1sHandle;
extern osTimerId_t t250msHandle;
extern osMessageQueueId_t qVTafficheHandle;

extern yVTbuff_t VTbuffer;
extern yMENU_t mnuSTM;
extern uint32_t adcbuf[];
extern yANALOG VRx, VRy;
extern uint8_t aRxBuffer[];

/*
  * @brief  Initialiser routes les interrupts du projet
  * @param  none
  * @retval none
*/
void Interrputs_Init(void) {
	//--- start USART2
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	//--- start ADC acquisition via DMA
	HAL_ADC_Start_DMA(&hadc1,(uint32_t *)adcbuf,2);
	//--- start TIM1 (ADC1 schedule)
	HAL_TIM_Base_Start(&htim1);
	//--- timers set period & start
	osTimerStart(t1sHandle, pdMS_TO_TICKS(1000));
	osTimerStart(t250msHandle, pdMS_TO_TICKS(250));
	return;
}

/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/** PC13 GPIO_EXTI15_10 (B1 blue button) */
	if(GPIO_Pin == B1_Pin) {
		snprintf(mnuSTM.Buffer, 1024, CUP(9,50) "--BP1 Interrupt");
		osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
		HAL_UART_Transmit(&huart2,(uint8_t *) mnuSTM.Buffer, strlen(mnuSTM.Buffer), 5000);
		osSemaphoreRelease(semUARTHandle);
		//MU flag detection interrupt, traitement ds le while du main
		//BP1Detected = 1;
	}

	/** PB2	GPIO_EXTI2	(Joystick button) */
	if(GPIO_Pin == SWxy_Pin) {
		snprintf(mnuSTM.Buffer, 1024, CUP(10,50) "--Swxy Interrupt");
		osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
		HAL_UART_Transmit(&huart2,(uint8_t *) mnuSTM.Buffer, strlen(mnuSTM.Buffer), 5000);
		osSemaphoreRelease(semUARTHandle);
	}

	/** autre entree interrupt */
}

/*
 * USARTs callback
 * don't forget to start!
 * 		 __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);	//start USART2
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/** USART2 - console interface */
	if (huart->Instance == USART2) {
		// interruption qd le nb de caracteres recu est correct
//		yMenuGetTouche(aRxBuffer);
		mnuSTM.GetTouche(&mnuSTM);
		osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
		HAL_UART_Transmit(&huart2,(uint8_t *) mnuSTM.Buffer, strlen(mnuSTM.Buffer), 5000);
		osSemaphoreRelease(semUARTHandle);
		//TODO: afficher le nvx choix
	} //if usart2
	/** manage an other usart if any! */
}	//callback


/* interrupt methode (via callback function)
 * be careful, interrupt & callback start immediately before end of inits!!!!
 * adcbuf[0]  : PC2, IN3 = VRx
 * adcbuf[1]  : PC3, IN4 = VRy
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if(hadc->Instance == ADC1) {
		//debug
		//snprintf(mnuSTM.Buffer, 1024, CUP(11,50) "--ADC1 : %4d \t %4d", (int)adcbuf[0], (int)adcbuf[1]);
		//osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
		//HAL_UART_Transmit(&huart2,(uint8_t *) mnuSTM.Buffer, strlen(mnuSTM.Buffer), 5000);
		//osSemaphoreRelease(semUARTHandle);

		VRx.Raw = adcbuf[0];
		VRy.Raw = adcbuf[1];

		VTbuffer.src = SrcVRxy;
		snprintf(VTbuffer.VTbuff, 50, CUP(11,50) "--ADC1 : %4d \t %4d", (int)VRx.Raw, (int)VRy.Raw);
		osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, 0U);	//envoi vers task afficahge

	}
}

/**
  * @brief  Alarm A callback.
  * @param  hrtc RTC handle
  * @retval None
  */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	//---reveillé par RTC AlarmA
	snprintf(mnuSTM.Buffer, 1024, CUP(7,50) "RTC Alarm A flag");
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) mnuSTM.Buffer, strlen(mnuSTM.Buffer), 5000);
	osSemaphoreRelease(semUARTHandle);
	RTC_AlarmA_flag = 1;
}

/* t1s_Callback function */
/*
  * @brief  FreeRTOS timer t1s_Callback function
  * @param
  * @retval status
*/
void t1s_Callback(void *argument)
{
  /* USER CODE BEGIN t1s_Callback */
	//debug/test timer
	//VTbuffer.src = SrcNone;
	//snprintf(VTbuffer.VTbuff, 50, DECRC "\t--FreeRTOS timer 1s");
	//osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);	//envoi vers task afficahge
	//--- gestion menu STM32Monitor
//	if (aRxBuffer[0] != aRxBuffer[2]) {
//		mnuSTM.GetTouche(&mnuSTM);		//utilise aRxBuffer
//		VTbuffer.src = SrcNone;
//		snprintf(VTbuffer.VTbuff, 50, DECRC "%s", mnuSTM.Buffer);
//		osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);	//envoi vers task afficahge
//		aRxBuffer[2] = aRxBuffer[0];		//memoriser la cde pour ne l'appliquer qu'une seule fois!
//	}

	/* toggle LD2 */
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  /* USER CODE END t1s_Callback */
}

/* t250ms_Callback function */
void t250ms_Callback(void *argument)
{
  /* USER CODE BEGIN t250ms_Callback */
	/* gestion menu STM32Monitor - check CubeMonitor VT100 emulation Keyboard */
	if (aRxBuffer[0] != aRxBuffer[2]) {
		mnuSTM.GetTouche(&mnuSTM);		//utilise aRxBuffer
		VTbuffer.src = SrcNone;
		snprintf(VTbuffer.VTbuff, 50, DECRC "%s", mnuSTM.Buffer);
		osMessageQueuePut(qVTafficheHandle, &VTbuffer, 0U, portMAX_DELAY);	//envoi vers task afficahge
		aRxBuffer[2] = aRxBuffer[0];		//memoriser la cde pour ne l'appliquer qu'une seule fois!
	}
  /* USER CODE END t250ms_Callback */
}

//That's all folks!!
