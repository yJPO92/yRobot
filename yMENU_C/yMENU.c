/**
 *******************************************************************************
 * @file    yMENU.c
 * @brief   'yMENU' pour definir un menu et
 * 				les fonctions mini d'affichage
 * @version 5.0
 * @author  Jean92
 * @note    gestion des touches clavier en fonction du menu
 *
 * v1.x Copyright (c) 2015 Jean92, https://developer.mbed.org/users/Jean92/
 * v2.0 migration ds TrueStudio et STM32L476RG
 *      on ne lit pas l'AI directement, on rÃ©cupÃ©re un item d'un tableau
 *      rempli par ADC/DMA du STM32L476
 * v3.0 C=>C++
 * v3.2 suppression des codes VT100, gérés par ailleurs!
  * v5.0 C++==>C !!
 *******************************************************************************
 * @date    Fev-2017, Juil-2020, aout-2020
 *******************************************************************************
 * @note    a adapter pour chaque appli!
 *******************************************************************************
 */

#include "yMENU.h"
#include "main.h"
#include "VT100.h"

/*
//----- pour STM32CubeMonitor
#define NR_VAR_GLO_
#include "CubeMon.h"
*/

/*--------------------------------------
 * Fonctions/methode applicables au Menu
 *-------------------------------------*/

//void Welcome_fnc(struct yMENU_t *self) {
//	snprintf(self->Buffer, 1024, "\nHello Welcome et Bienvenue");
////	snprintf(self->Buffer, 1024, clrscr homescr
////		  	  	  	  	    "\nBonjour maître!"
////		  	  	  	  	    "\n(c)Jean92, " yDATE
////							"\n" yPROG " " yVER " (" yCubeMX ")"
////						    "\nCompil: " __TIME__ " le " __DATE__
////						    "\nSTmicro NUCLEO_L476RG"
////							"\r\n");
//}

void Display_fnc(struct yMENU_t *self) {
	snprintf(self->Buffer, 1024,
							"\n   " REVERSE " " yPROG  " \n" OFF
							mmenu1
							mmenu2  DECSC);	//and save cursor position
}

void Infos_fnc(struct yMENU_t *self) {
 snprintf(self->Buffer, 1024, DECRC ERASELINE
		 	 	 	 	"\n Menu version %d.%d (Autre %d)\n" DECSC,
						self->m_version, self->m_patch, (int)self->m_toto);
}

void Init_fnc(struct yMENU_t *self)
{
//	snprintf(self->Buffer, 1024, "\tInit yMENU_t done");
	snprintf(self->Buffer, 1024, clrscr homescr
		  	  	  	  	    "\nBonjour maître!"
		  	  	  	  	    "\n(c)Jean92, " yDATE
							"\n" yPROG " " yVER " (" yCubeMX ")"
						    "\nCompil: " __TIME__ " le " __DATE__
						    "\nSTmicro NUCLEO_L476RG"
							"\n" DECSC);

	self->m_version = 5;
	self->m_patch = 1;
	self->m_toto = 77;
	//self->Welcome = Welcome_fnc;
	self->Display = Display_fnc;
	self->Infos = Infos_fnc;
}


//
///* methodes */
//void yMENU::yMenuGetTouche(uint8_t aRxBuffer[]) {
//	//TODO: menu get touche, mettre un flag cde erronée!!
//	if(uart2NbCar == 1) {		/* on attend 1 seule touche */
//		switch (aRxBuffer[0]) {
//		case 'R': case 'r':		/* ADC methode 3 afficher à la demande */
//			snprintf(aTxBuffer, 1024, ERASELINE "\tFrom Node-RED (R) %d     " DECRC, aRxBuffer[0]);
//			HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//			break;
//
//		case 48: case 49: case 50:		/* chiffre de 0 a 9 */
//		case 51: case 52: case 53:
//		case 54: case 55: case 56: case 57:
//			snprintf(aTxBuffer, 1024, ERASELINE "\tFrom Node-RED (0-9) %d     " DECRC, aRxBuffer[0]);
//			HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//			break;
//
//		case 'T': case 't':		/* ???????e */
//			snprintf(aTxBuffer, 1024, ERASELINE "\tFrom Node-RED (T) %d     " DECRC, aRxBuffer[0]);
//			HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//			break;
//
//		case 'L': case 'l':		/* ?????? */
//			snprintf(aTxBuffer, 1024, ERASELINE "\tFrom Node-RED (L) %d     " DECRC, aRxBuffer[0]);
//			HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//			break;
//
//////		case 'E': case 'e':		/* vider la queue de RTOS */
//////			//msg = 101;
//////			//status = osMessageQueuePut(QueueTestHandle, &msg, 0U, 0U);	//ne fonctionne pas! (doit être appelée depuis une interrupt)
//////			break;
////
//		case 'S': case 's':		/* ?????? */
//			snprintf(aTxBuffer, 1024, ERASELINE "\tFrom Node-RED (S) %d     " DECRC, aRxBuffer[0]);
//			HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//			break;
//
//		case 'I': case 'i':		/* afficher qqs infos sur la classe yMENU) */
//			yMenuInfos();
//			break;
//
//		case 'C': case 'c':		/* Clear screen */
//			yMenuClearVT();
//			break;
//
//		case 'M': case 'm':		/* Display menu */
//			yMenuClearVT();	 yMenuWelcome();  yMenuDisplay();
//			break;
//
//		case 'X': case 'x':		/* adjust VRx Trim */
//			snprintf(aTxBuffer, 1024, ERASELINE "\tFrom Node-RED (X) %d; VRx trim %2.1f" ERASELINE DECRC, aRxBuffer[0], ymx_VRx_trim);
//			HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//			break;
//
//		case 'Y': case 'y':		/* adjust VRy Trim */
//			snprintf(aTxBuffer, 1024, ERASELINE "\tFrom Node-RED (Y) %d; VRy trim %2.1f" ERASELINE DECRC, aRxBuffer[0], ymx_VRy_trim);
//			HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//			break;
//
////		case 'h': case 'H':		/* affiche cadre pour horloge */
////			snprintf(aTxBuffer, 1024, screen1 DECRC);
////			HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
////			break;
////
////		case 'W': case 'w':		/* passage en modee 2 touches */
////			// x ? y : z // y if x is true (nonzero), else z
////			(uart2NbCar == 1) ? (uart2NbCar = 2) : (uart2NbCar = 1);
////			snprintf(aTxBuffer, 1024, "\tuart2 NbCar %d" DECRC, uart2NbCar);
////			HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
////			break;
////
////		case 'Q': case 'q':		/* Quit, shutdown */
////			HAL_PWREx_EnterSHUTDOWNMode();
////			break;
////
//		default:				/* Cde erronée */
//			snprintf(aTxBuffer, 1024, "\tCde erronée! (%d)" ERASELINE DECRC, aRxBuffer[0]);
//			HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//			break;
//		}	//switch
//	}	//if flag
//	else {		/* on attend 2 touches */
//		//not used
//	}	//else flag
//}
//
//
//void yMENU::yMenuClearVT(void) {
//	snprintf(aTxBuffer, 1024, OFF clrscr homescr);
//	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//	return;
//}
//
//void yMENU::yMenuClearStatusBar(void) {
//	//erase fin de ligne de trace du BP1 puis go to status line
//	//erase fin de ligne de trace du SWxy puis go to status line
//	//erase status line
//	snprintf(aTxBuffer, 1024, CUP(9,50) ERASELINE
//							  CUP(10,50) ERASELINE
//							  DECRC ERASELINE
//							  DECRC );
//	HAL_UART_Transmit(&huart2,(uint8_t *) aTxBuffer, strlen(aTxBuffer), 5000);
//	return;
//}
//


/*
 *   That's all folks!
 */
