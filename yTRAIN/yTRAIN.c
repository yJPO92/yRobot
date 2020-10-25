/**
 *******************************************************************************
 * @file    yTRAIN.c
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

#include "yTrain.h"
#include "stm32l4xx_hal.h"		//pour les types uint32_t...
//#include "FreeRTOS.h"
//#include "cmsis_os.h"

/** Initialisation de la structure Train */
void yTRAIN_Init(yTRAIN* this) {
	//--- Inputs
	this->MaAr = 0;
	this->Vitesse = 0.0;
	this->Direction = 0.0;
	//--- Outputs
	this->inRun = 0;
	this->MaAr_D = 0U;
	this->Vitesse_D = 0.0;
	this->MaAr_G = 0U;
	this->Vitesse_G = 0.0;
	//--- Parameters
	this->Dir_HH = 90.0;	//TODO verifier limite
	this->Dir_LL = -90.0;
	//--- memories
	this->Vitesse_memo = 0.0;
	this->Direction_memo = 0.0;
	this->Sens_memo = 0;
	this->Run_memo = 0;
    //--- Virtual outputs
		//none
	//--- Real outputs
		//none
}


/**
 * @note \n
 *         That's all folks!
 */
