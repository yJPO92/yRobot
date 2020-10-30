/**
 *******************************************************************************
 * @file    yTRAIN.h
 * @brief   Controller un train constitué de 2 Moteurs
 * @version 1.0
 * @author  Jean92
 * @note    recoie des ordres de marche, vitesse et direction\n
 * 			envoie des ordres aux 2 moteurs
 *
 * v1.x Copyright (c) 2015 Jean92, https://developer.mbed.org/users/Jean92/
 *******************************************************************************
 * @date    oct-2020
 *******************************************************************************
 * @note
 *
 *******************************************************************************
 */

#ifndef YTRAIN_H_
#define YTRAIN_H_

#include <sys/types.h>		//definir types mcu like uint32_t
#include "FreeRTOS.h"		//pour avoir acces aux fnc RTOS ds les objects
#include "cmsis_os.h"
#include "tim.h"

/*---------------------
 * Structure d'un train
 *--------------------*/

typedef struct {
	//--- Inputs
	uint8_t MaAr;		// Marche/Arret request
	float Vitesse;		// Vitesse request
	float Direction;	// Direction request
	//--- Outputs
	uint8_t inRun;		// Etat marche (running state)
	uint8_t MaAr_D;		// Moteur Droit Marche/Arret order
	float Vitesse_D;	// Moteur Droit Vitesse order
	uint8_t MaAr_G;		// Moteur Gauche Marche/Arret order
	float Vitesse_G;	// Moteur Gauche Vitesse order
	//--- Parameters
	float Dir_HH;		// limite
	float Dir_LL;		// Limite
	//--- memories
	float Vitesse_memo;		// speed memory
	float Direction_memo;   // direction memory
	uint8_t Sens_memo;		// sens smemory
	uint8_t Run_memo;		// running memory
	//--- Virtual outputs
	//none
	//--- Real outputs
	//none
} yTRAIN;

//------------------------
// Methodes
//------------------------

/*
 * @brief  Initialisation data pour un train
 * @param  pointeur sur structure du moteur
 * @retval status
*/
void yTRAIN_Init(yTRAIN* this);



#endif /* YTRAIN_H_ */

/**
 * @note \n
 * That's all folks!
 */

