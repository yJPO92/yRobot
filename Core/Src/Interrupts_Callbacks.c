/**
 *******************************************************************************
 * @file    Interrupts_Callbacks.c
 * @author  Jean
 * @brief   Callback: BP 1, Keyboard, ADC, RTC_AlarmA
 * @version    
 *******************************************************************************
 * Modified :
 * Created  : 29 ao�t 2020
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
//#include "yTask.h"
#include "VT100.h"
#include "yMENU.h"
#include "yANALOG.h"

extern UART_HandleTypeDef huart2;
extern osSemaphoreId_t semUARTHandle;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim1;

extern yMENU_t mnuSTM;
extern uint32_t adcbuf[];
extern yANALOG VRx, VRy;

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
		snprintf(mnuSTM.Buffer, 1024, CUP(9,50) "--BP1 Interrupt" DECRC);
		osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
		HAL_UART_Transmit(&huart2,(uint8_t *) mnuSTM.Buffer, strlen(mnuSTM.Buffer), 5000);
		osSemaphoreRelease(semUARTHandle);
		//MU flag detection interrupt, traitement ds le while du main
		//BP1Detected = 1;
	}

	/** PB2	GPIO_EXTI2	(Joystick button) */
	if(GPIO_Pin == SWxy_Pin) {
		snprintf(mnuSTM.Buffer, 1024, CUP(10,50) "--Swxy Interrupt" DECRC);
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
		/* clear reception buffer*/
//		aRxBuffer[0]='\0';
//		aRxBuffer[1]='\0';
//		aRxBuffer[2]='\0';

		//TODO: afficher le nvx choix

	}	//if usart2
	/* manage an other usart if any! */
}	//callback


/* interrupt methode (via callback function)
 * be careful, interrupt & callback start immediately before end of inits!!!!
 * adcbuf[0]  : PC2, IN3 = VRx
 * adcbuf[1]  : PC3, IN4 = VRy
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if(hadc->Instance == ADC1) {
		//debug
		snprintf(mnuSTM.Buffer, 1024, CUP(11,50) "--ADC1 : %4d \t %4d" DECRC, (int)adcbuf[0], (int)adcbuf[1]);
		osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
		HAL_UART_Transmit(&huart2,(uint8_t *) mnuSTM.Buffer, strlen(mnuSTM.Buffer), 5000);
		osSemaphoreRelease(semUARTHandle);

		VRx.Raw = adcbuf[0];
		VRy.Raw = adcbuf[1];

		//VRx.SetRaw(adcbuf[0]);
		//VRx.CalulerMesure();

		//VRy.SetRaw(adcbuf[1]);
		//VRy.CalulerMesure();
	}
}

/**
  * @brief  Alarm A callback.
  * @param  hrtc RTC handle
  * @retval None
  */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	//---reveill� par RTC AlarmA
	snprintf(mnuSTM.Buffer, 1024, CUP(7,50) "RTC Alarm A flag" DECRC);
	osSemaphoreAcquire(semUARTHandle, portMAX_DELAY);  //timeout 0 if from ISR, else portmax
	HAL_UART_Transmit(&huart2,(uint8_t *) mnuSTM.Buffer, strlen(mnuSTM.Buffer), 5000);
	osSemaphoreRelease(semUARTHandle);
	RTC_AlarmA_flag = 1;
}

//That's all folks!!
