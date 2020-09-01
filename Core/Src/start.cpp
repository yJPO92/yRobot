/**
 *******************************************************************************
 * @file    start.cpp
 * @author  Jean
 * @brief   point de depart des prog en C++
 * @version 2.0
 *******************************************************************************
 * Modified :
 * Created  : 4 juil. 2020
 *******************************************************************************
 * @note
 *******************************************************************************
 */


#include "main.h"
#include "adc.h"
#include "dma.h"
#include "rtc.h"
#include "usart.h"
#include "yANALOG_FLT.h"
#include <stdio.h>
#include <string.h>
#include "VT100.h"
#include "yMENU.h"

//----- pour SW trace
//#define ITM_Port32(n) (*((volatile unsigned long *) (0xE0000000+4*n)))
//		ITM_Port32(29) = 1;
//		ITM_Port32(29) = 2;

//----- pour STM32CubeMonitor
//		definir uniquement la fonction de recopie des data et qq data
#define NR_VAR_GLO_
#include "CubeMon.h"

//////----- buffer DMA / ADC1
//#define ADCBUFSIZE	2
//uint32_t adcbuf[ADCBUFSIZE];
//uint32_t adc1_value[ADCBUFSIZE];
//----- pour usart2, gestion interface sur console VT
//extern char aTxBuffer[2048];		//buffer d'emission
//extern uint16_t uart2NbCar;	//nb de byte attendu
//extern uint8_t aRxBuffer[3];		//buffer de reception at specific address

///* Real Time Clock */
//RTC_HandleTypeDef hrtc;
//RTC_TimeTypeDef myTime;
//RTC_DateTypeDef myDate;
//RTC_AlarmTypeDef sAlarm = {0};
//uint8_t RTC_AlarmA_flag = 0;

/* cree objets entree analog */
//yANALOG_FLT VRx(&adcbuf[0],-100.0,100.0,0.3);
//yANALOG_FLT VRy(&adcbuf[1],-100.0,100.0,0.3);
//
///* class pour menu */
//yMENU menuSTM;


/* Private function prototypes */
//static void RTC_AlarmModify(void);

/*
 * Starting point for all C++ code
 */
extern "C" int start_cpp() {

//	//--- Activate USART2
//	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);

	//--- reset ecran, message de bienvenue (avec nom prog & version)
//	menuSTM.yMenuClearVT();
//	menuSTM.yMenuWelcome();
//	menuSTM.yMenuDisplay();
//	//--- petit msg en fin d'inits
//	//TODO faire ds yMenu une écriture sur ligne de commande!!
//	snprintf(aTxBuffer, 1024, DECRC "\t=>Inits STM finis, ==> C++");		//trace sur ligne de status
//	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);

	//--- init compteur de secondes
//	RTC_MiseAheure();

//	//--- start ADC acquisition via DMA
//	HAL_ADC_Start_DMA(&hadc1,(uint32_t *)adcbuf,ADCBUFSIZE);

	while(1) /* Assuming you don't wish to return to main() in main.c */
	{
//		HAL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);
		//--- debug Joystick
//		snprintf(aTxBuffer, 1024, CUP(5,50) "VRx %4d " CUP(5,60) "VRy %4d " DECRC, (int)adcbuf[0], (int)adcbuf[1]);
//		HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
		//--- afficher les objets
		snprintf(aTxBuffer, 1024, CUP(7,50) "VRx %5.2f " CUP(7,60) "VRy %5.2f " DECRC, VRx.GetPV(), VRy.GetPV());
		HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);

		//--- adjust Trim ds zone -10/+10
		if ((ymx_VRx_trim >= -10.0) && (ymx_VRx_trim <= 10.0)) {
			VRx.AdjustTrim(ymx_VRx_trim);
		}
		if ((ymx_VRy_trim >= -10.0) && (ymx_VRy_trim <= 10.0)) {
			VRy.AdjustTrim(ymx_VRy_trim);
		}

		//--- maj Filtre 0/1
		if ((ymx_Coef > 0.0) && (ymx_Coef <= 1.0)) {
			VRx.majFiltre(ymx_Coef);
			VRy.majFiltre(ymx_Coef);
		}

		//--- calculer objets
		VRx.CalulerMesure();
		VRy.CalulerMesure();
		//--
		VRx.Exec();
		VRy.Exec();

		//--- gestion menu STM32Monitor
		if (aRxBuffer[0] != aRxBuffer[2]) {
			menuSTM.yMenuGetTouche(aRxBuffer);		//menu VT (buffer usart)
			aRxBuffer[2] = aRxBuffer[0];			//memoriser la cde pour ne l'appliquer qu'une seule fois!
		}

//		//--- get/set data for STM32CubeMonitor
//		yCopy2CubeMonitor(1U);		//set data
//		yCopy2CubeMonitor(0U);		//get data

//		//--- Effacer lignes status toutes les x secondes
//		HAL_RTC_GetTime(&hrtc, &myTime, RTC_FORMAT_BIN);
//		HAL_RTC_GetDate(&hrtc, &myDate, RTC_FORMAT_BIN);	//need to read also the date!!!
//
//		//--- afficher date & heure ds zone encadrée et mettre curseur sur ligne status
//		snprintf(aTxBuffer, 1024, CUP(12,60) "%02d-%02d-%02d" CUP(13,60) "%02d:%02d:%02d" DECRC,
//									  myDate.Date, myDate.Month, myDate.Year,
//									  myTime.Hours, myTime.Minutes, myTime.Seconds);
//		HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);

		//--- sur interrupt RTC Alarm A, clear status line
//		if (RTC_AlarmA_flag == 1) {
//			menuSTM.yMenuClearStatusBar();
//			RTC_AlarmModify();
		}

		//---  Attendre un peu
		//HAL_Delay(500);
	}
}	//start_cpp

/*----------------------------
 * s/fonction
 * ---------------------------
 */

///*
//  * @brief  modifier le setup de RTC_AlarmA, ajouter qq seconds en BCD
//  * @param  none
//  * @retval status
//*/
//void RTC_AlarmModify(){
//
//	sAlarm.Alarm = RTC_ALARM_A;
//	//sAlarm.AlarmTime.Hours = 0x12;		//HH:MN peu importe
//	//sAlarm.AlarmTime.Minutes = 0x1;
//	sAlarm.AlarmTime.Seconds += 0x04;		//add 4 seconds en BCD
//	if ((sAlarm.AlarmTime.Seconds & 0x000F) > 0x9) { sAlarm.AlarmTime.Seconds += 0x6; }	//check & correct over 9
//	if (sAlarm.AlarmTime.Seconds >= 0x59) { sAlarm.AlarmTime.Seconds = 0x01; }		//check & reset after 1 minute
//	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
//	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
//	sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
//	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
//	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
//	sAlarm.AlarmDateWeekDay = 0x1;
//
//	HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD);		//activate interrupt RTC AlarmA
//
//	RTC_AlarmA_flag = 0;	//reset flag
//	}

/*------------------------------------------
 * Callback: BP 1, Keyboard, ADC, RTC_AlarmA
 *------------------------------------------
 */



///**
//  * @brief  Alarm A callback.
//  * @param  hrtc RTC handle
//  * @retval None
//  */
//void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
//{
//	//---reveillé par RTC AlarmA
//	snprintf(aTxBuffer, 1024, CUP(15,50) "RTC Alarm A flag" DECRC);
//	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//	RTC_AlarmA_flag = 1;
//}


//That's all folks!!
