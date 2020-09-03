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

extern uint8_t aRxBuffer[3];		//buffer de reception
extern uint16_t uart2NbCar;			//nb de byte attendu
extern char tmpBuffer[10];		    //buffer temporaire pour switch/case

/*--------------------------------------
 * Fonctions/methode applicables au Menu
 *-------------------------------------*/

/*
  * @brief  Affiche le menu
  * @param  la structure
  * @retval none (ecrit ds la structure elle meme)
*/
void Display_fnc(struct yMENU_t *self) {
	snprintf(self->Buffer, 1024,
							"\n   " REVERSE " " yPROG  " \n" OFF
							mmenu1
							mmenu2  DECSC);	//and save cursor position
	return;
}

/*
  * @brief  Affiche la version du  menu
  * @param  la structure
  * @retval none (ecrit ds la structure elle meme)
*/
void Infos_fnc(struct yMENU_t *self) {
 snprintf(self->Buffer, 1024, DECRC ERASELINE
		 	 	 	 	"\n Menu version %d.%d (Autre %d)\n" DECSC,
						self->m_version, self->m_patch, (int)self->m_toto);
 return;
}

/*
  * @brief  Lit le clavier
  * @param  la structure
  * @retval none (ecrit ds la structure elle meme)
*/
void GetTouche_fnc(struct yMENU_t *self) {
	//TODO: menu get touche, mettre un flag cde erronée!!
	if(uart2NbCar == 1) {		/* on attend 1 seule touche */
		switch (aRxBuffer[0]) {
		case 'R': case 'r':		/* ???? */
			snprintf(self->Buffer, 1024, ERASELINE "\tFrom Node-RED (R) %d     " DECRC, aRxBuffer[0]);
			break;

		case 48: case 49: case 50:		/* chiffre de 0 a 9 */
		case 51: case 52: case 53:
		case 54: case 55: case 56: case 57:
			snprintf(self->Buffer, 1024, ERASELINE "\tFrom Node-RED (0-9) %d     " DECRC, aRxBuffer[0]);
			break;

		case 'T': case 't':		/* ???????e */
			snprintf(self->Buffer, 1024, ERASELINE "\tFrom Node-RED (T) %d     " DECRC, aRxBuffer[0]);
			break;

		case 'L': case 'l':		/* ?????? */
			snprintf(self->Buffer, 1024, ERASELINE "\tFrom Node-RED (L) %d     " DECRC, aRxBuffer[0]);
			break;

////		case 'E': case 'e':		/* vider la queue de RTOS */
////			//msg = 101;
////			//status = osMessageQueuePut(QueueTestHandle, &msg, 0U, 0U);	//ne fonctionne pas! (doit être appelée depuis une interrupt)
////			break;
//
		case 'S': case 's':		/* ?????? */
			snprintf(self->Buffer, 1024, ERASELINE "\tFrom Node-RED (S) %d     " DECRC, aRxBuffer[0]);
			break;

		case 'I': case 'i':		/* afficher qqs infos sur la classe yMENU) */
//			yMenuInfos();
			break;

		case 'C': case 'c':		/* Clear Status Bar & +++ */
			snprintf(self->Buffer, 1024, CUP(9,50) ERASELINE		/*BP1*/
					  	  	  	  	  	 CUP(10,50) ERASELINE		/*SWxy*/
										 /*CUP(11,50) ERASELINE*/	/*ADC1*/
										 CUP(7,50) ERASELINE		/*AlarmA*/
									     DECRC ERASELINE			/*Status line*/
									     DECRC );
			break;

		case 'M': case 'm':		/* Display menu */
//			yMenuClearVT();	 yMenuWelcome();  yMenuDisplay();
			break;

		case 'X': case 'x':		/* adjust VRx Trim */
//			snprintf(self->Buffer, 1024, ERASELINE "\tFrom Node-RED (X) %d; VRx trim %2.1f" ERASELINE DECRC, aRxBuffer[0], ymx_VRx_trim);
			break;

		case 'Y': case 'y':		/* adjust VRy Trim */
//			snprintf(self->Buffer, 1024, ERASELINE "\tFrom Node-RED (Y) %d; VRy trim %2.1f" ERASELINE DECRC, aRxBuffer[0], ymx_VRy_trim);
			break;

//		case 'h': case 'H':		/* affiche cadre pour horloge */
//			snprintf(self->Buffer, 1024, screen1 DECRC);
//			break;
//
//		case 'W': case 'w':		/* passage en modee 2 touches */
//			// x ? y : z // y if x is true (nonzero), else z
//			(uart2NbCar == 1) ? (uart2NbCar = 2) : (uart2NbCar = 1);
//			snprintf(self->Buffer, 1024, "\tuart2 NbCar %d" DECRC, uart2NbCar);
//			break;
//
//		case 'Q': case 'q':		/* Quit, shutdown */
//			HAL_PWREx_EnterSHUTDOWNMode();
//			break;
//
		default:				/* Cde erronée */
			snprintf(self->Buffer, 1024, "\tCde erronée! (%d)" ERASELINE DECRC, aRxBuffer[0]);
			break;
		}	//switch
	}	//if flag
	else {		/* on attend 2 touches */
		//not used
	}	//else flag
	return;
}


//void yMENU::yMenuClearVT(void) {
//	snprintf(self->Buffer, 1024, OFF clrscr homescr);
//	HAL_UART_Transmit(&huart2,(uint8_t *) self->Buffer, strlen(self->Buffer), 5000);
//	return;
//}

void ClearStatusBar_fnc(struct yMENU_t *self) {
	//erase trace du BP1 jusqu'a fin de ligne
	//erase trace du SWxy  jusqu'a fin de ligne
	//erase trace ADC  jusqu'a fin de ligne
	//puis go to status line
	//erase status line
	snprintf(self->Buffer, 1024, CUP(9,50) ERASELINE		/*BP1*/
			  	  	  	  	  	 CUP(10,50) ERASELINE		/*SWxy*/
								 /*CUP(11,50) ERASELINE*/	/*ADC1*/
								 CUP(7,50) ERASELINE		/*AlarmA*/
							     DECRC ERASELINE			/*Status line*/
							     DECRC );
	return;
}


/*
  * @brief  Initialiser la structure
  * @param  struture elle meme
  * @retval none
*/
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
	self->GetTouche = GetTouche_fnc;
	self->Display = Display_fnc;
	self->Infos = Infos_fnc;
	self->ClearStatusBar = ClearStatusBar_fnc;

	return;
}

/*
 *   That's all folks!
 */
