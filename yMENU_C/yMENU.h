/**
 *******************************************************************************
 * @file    yMENU.h
 * @brief   'yMENU' pour definir un menu et
 * 				les fonctions mini d'affichage
 * @version 5.0
 * @author  Jean92
 * @note
 *
 * v1.x Copyright (c) 2015 Jean92, https://developer.mbed.org/users/Jean92/
 * v2.0 migration ds TrueStudio et STM32L476RG
 *      on ne lit pas l'AI directement, on rÃ©cupÃ©re un item d'un tableau
 *      rempli par ADC/DMA du STM32L476
 * v3.0 C=>C++
 * v3.1 suppression des codes VT100, gérés par ailleurs!
 * v5.0 C++==>C !!
 * v5.2 affinage menu
 * v6.0 cde test yMOTOR
 *******************************************************************************
 * @date    Fev-2017, Juil-2020, aout-2020
 *******************************************************************************
 * @note    a adapter pour chaque appli!
 *******************************************************************************
 */

#ifndef YMENU_H_
#define YMENU_H_

#include "stm32l4xx_hal.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
//#include "usart.h"

#include "VT100.h"

/* Menu principal */
#define mmenu1 "\n\
----------------------------------\n\
\n\
 A (Arret yMOTOR) \n\
 B (start/speed 0 yMOTOR)\n\
 D (speed +15.5)\n\
 E (speed -20.2)\n\
 F (speed VRx)\n\
 H (display Horloge zone)\n\
 \n\
 0-9|a|b (affiche le chiffre)\n\
 +|- (change de chiffre)\n\
\n\
 W (saisie de 2 touches)\n\
\n\
 Veuillez utiliser le dashboard\n\
 STM32CubeMonitor / Node-RED\n\
\n\
----------------------------------\n\
 Cls | Menu | Quit\n"

/* menu d'attente */
#define mmenu2 " Faire un choix ...\n\n"

/* affichage colonnes de comptage */
#define mmenu3 "\x1B[00;00H\
0123456789|123456789|123456789|123456789|123456789*123456789|123456789|123456789|123456789|12345678|"

/* Dessine une boite a droite de l'écran */
#define screen1 "\
\x1B[2;50H       Horloge de Jean92\
\x1B[3;50H+---------+---------+---------+\
\x1B[4;50H|          jj-mm-aa           |\
\x1B[5;50H|          hh:mm:ss           |\
\x1B[6;50H+-----------------------------+\
\x1B[7;50H"

/*---------------------------------------------
 *  Structure d'acceuil du menu et ses methodes
 *-------------------------------------------*/
typedef struct yMENU_t {
	char Buffer[1024];	//buffer de preparation
	int m_version;
	int m_patch;
	int m_toto;

	void (*GetTouche)(struct yMENU_t *self);
	void (*Display)(struct yMENU_t *self);
	void (*Infos)(struct yMENU_t *self);
	void (*Init)(struct yMENU_t *self);
	void (*ClearStatusBar)(struct yMENU_t *self);


} yMENU_t;

void GetTouche_fnc(struct yMENU_t *self);
void Display_fnc(struct yMENU_t *self);
void Infos_fnc(struct yMENU_t *self);
void Init_fnc(struct yMENU_t *self);
void ClearStatusBar_fnc(struct yMENU_t *self);

/*--------------------------------------
 * Fonctions/methode applicables au Menu
 *-------------------------------------*/

//	/*
//	 * @brief  Gere la saisie de commande (1 ou 2 caracters)
//	 * @param  buffer de caracters (1|2)
//	 * @retval none
//	 */
//	void yMenuGetTouche(uint8_t aRxBuffer[]);
//
//	/*
//	 * @brief  Nettoie l'ecran de la VT, remet curseur en haut à droite
//	 * @param  none
//	 * @retval none
//	 */
//	void yMenuClearVT(void);
//
//	/*
//	 * @brief  Nettoie la ligne de status
//	 * @param  none
//	 * @retval none
//	 *		effacer les messages et revenir a l'endroit du curseur sauvé
//	 *		o Interrutp B1 en l 9, c 50
//	 *		o Interrutp SW du joystick en l 10, c 50
//	 *		o ligne status en DECRC (restore cursor)
//	 *		=> placer le curseur en DECRC
//	 */
//	void yMenuClearStatusBar(void);
//
//	/*
//	 * @brief  Affiche le menu a partir du curseur
//	 * @param  none
//	 * @retval none
//	 */
//	void yMenuDisplay(void);
//
//	/*
//	 * @brief  Affiche un message de bienvenue!
//	 * @param  none
//	 * @retval none
//	 */
//	void yMenuWelcome(void);
//
//	/*
//	  * @brief  afficher qq infos sur la classe yMENU
//	  * @param  none
//	  * @retval none
//	*/
//	void yMenuInfos(void);
//

#endif /* YMENU_H_ */
/*
 *   That's all folks!
 */

