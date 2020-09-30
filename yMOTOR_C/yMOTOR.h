/**
 *******************************************************************************
 * @file    yMOTOR.h
 * @brief   Controlleur pour Moteur par Pont-H (MC33886)"
 * @version 4.2
 * @author  Jean92
 * @note    pour formatter une entree analogique\n
 *          lecture DMA via ADC\n
 *          echelle et valeur engenieur calculée,\n
 *          echelle defaut = 0-4096\n
 *          filter y(n) = Coef * x(n) + (1 - Coef) * y(n-1)\n
 *          trim correction\n
 *          detection variation sur hysteresis
 *
 * v1.x Copyright (c) 2015 Jean92, https://developer.mbed.org/users/Jean92/
 * v2.0 migration ds TrueStudio et STM32L476RG
 *******************************************************************************
 * @date    sept-2015, sept-2020
 *******************************************************************************
 * @note
 *	H bridge  ---- MC33886 ----
 *           IN1   IN2   D1   /D2
 * Forward    H     L    0    pwm
 * Reverse    L     H    0    pwm
 * FreeWLow   L     L    0    pwm
 * FreeWHigh  H     H    0    pwm
 *
 *******************************************************************************
 */

#ifndef _yMOTOR_H
#define _yMOTOR_H

#include <sys/types.h>		//definir types mcu like uint32_t
#include "FreeRTOS.h"		//pour avoir acces aux fnc RTOS ds les objects
#include "cmsis_os.h"
#include "tim.h"

#define yARRET 0
#define yMARCHE 1
#define DEADBAND_DEFAULT	3.0     // dead band around 0.0 for nul speed
#define DUTY_CYCLE_DEFAULT	3	    // en %

/*---------------------
 * Structure d'un motor
 *--------------------*/

/*
 * yARRET ==> av = ar = 0
 * yMARCHE ==> av & ar = sign of speed

 * Vitesse effective
 * float [-100.0,+100.0]
 * negative ==> reverse, positive ==> forward
 *
 */

typedef struct {
	//--- Inputs
	uint8_t MarArr;		// Marche/Arret request
	float Speed_SP;		// Vitesse request
	//--- Outputs
	uint8_t inRun;		// Etat marche (running state)
    uint32_t Speed_MV;	// commande vitesse vers sortie
    int8_t Sens;		// sens de marche (AV/AR)
	float Velocity;		// Vitesse effective
	//--- Paramters
	uint32_t DutyCycle;	// PWM duty cycle in %
	float DeadBand;		// dead band (around 0.0 not action)
	//--- memories
    float Speed_memo;	//speed memory
    uint8_t Run_memo;	//running memory
    float DB_memo;		//deadband
    //--- Virtual outputs
    uint32_t _pwm;		//pour htim.instance.CCR2
    uint8_t _av;		//marche avant/stop
    uint8_t _ar;		//marche arrier/stop
	//--- Real outputs
    uint32_t _gpioPortIN1;
    uint16_t _gpioPinIN1;
    uint32_t _gpioPortIN2;
    uint16_t _gpioPinIN2;
    TIM_HandleTypeDef _htimpwm;
} yMOTOR;

//------------------------
// Methodes
//------------------------

/*
 * @brief  Initialisation data pour un moteur
 * @param  pointeur sur structure du moteur
 * @params gpioPort/Pin de ...
 * @retval status
*/
void yMOTOR_Init(yMOTOR* this,
			    uint32_t gpioPortIN1, uint16_t gpioPinIN1,
		    	uint32_t gpioPortIN2, uint16_t gpioPinIN2,
				TIM_HandleTypeDef htimpwm);

/*
 * @brief  Marche/Arret request
 * @param  pointeur sur structure du moteur
 * @param  request order
 * @retval status
*/
void yMOTOR_MarArr(yMOTOR* this, uint8_t mararr);

/*
 * @brief  Speed_SP request
 * @param  pointeur sur structure du moteur
 * @param  request value
 * @retval status
*/
void yMOTOR_Speed(yMOTOR* this, float speed);

/*
 * @brief  calcul moteur
 * @param  pointeur sur structure du moteur
 * @retval status
*/
void yMOTOR_Exec(yMOTOR* this);

/*
  * @brief  calcul Real Outputs
  * @param  pointeur sur structure du moteur
  * @retval none
*/
void yMOTOR_RealOutputs(yMOTOR* this);

    
//    /** PWM duty cycle periode in seconde
//     * wrapper for PwmOut::period()
//     * @param seconds (PWM duty cycle)
//     */
//    void Period(float seconds);

//    /** Change DeadBand
//     * @param db
//     * modify the default dead band (around 0.0 not action)
//     */
//    void DeadBand(float db);
    
//    /** Marche/Arret request
//     * yARRET ==> av = ar = 0
//     * yMARCHE ==> av & ar = sign of speed
//     */
//    //void mararr(int mararr = yARRET);
//    void MarArr(int mararr);
    


#endif	//_yMOTOR_H

/**
 * @note \n
 * That's all folks!
 */
