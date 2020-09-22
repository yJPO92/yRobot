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
 *******************************************************************************
 */

#ifndef _yMOTOR_H
#define _yMOTOR_H

#include <sys/types.h>			//definir types mcu like uint32_t
#include "tim.h"

#define yARRET 0
#define yMARCHE 1
#define DEADBAND_DEFAULT	3.0     // dead band around 0.0 for nul speed
#define PERIODE_DEFAULT		0.5     // en seconds

/*---------------------
 * Structure d'un motor
 *--------------------*/

/* yARRET ==> av = ar = 0
 * yMARCHE ==> av & ar = sign of speed

 * Vitesse request
 * @param speed (float normalised [-100.0,+100.0]
 * negative ==> reverse, positive ==> forward

 * Vitesse effective
 * float normalised [-100.0,+100.0]
 * negative ==> reverse, positive ==> forward

 * PWM duty cycle periode in seconde
 * wrapper for PwmOut::period()
 * @param seconds (PWM duty cycle)

 * Change DeadBand
 * @param db
 * modify the default dead band (around 0.0 not action)
 */

typedef struct {
	//--- Inputs
	uint8_t MarArr;		// Marche/Arret request
	float Speed_SP;		// Vitesse request
	//--- Outputs
	uint8_t inRun;		// Etat marche (running state)
    uint32_t Speed_MV;	// commande vitesse vers sortie
	float Velocity;		// Vitesse effective
	//--- Paramters
	float Period;		// PWM duty cycle periode in seconde
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

/*------------------------
 * Methodes
 *----------------------*/

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

//    /** Interface de controle du moteur
//     * @param pwm (Pwmout, vitesse variable)
//     * @param av  (DigitalOut, marche avant)
//     * @param ar  (DigitalOut, marche arriere)
//     */
//    yMOTOR(PinName pwm, PinName av, PinName ar);        /// Constructeur par defaut

//    /** Vitesse request
//     * @param speed (float normalised [-1.0,+1.0]
//     * negative ==> reverse, positive ==> forward
//     */
//    void Speed(float speed);
    
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
    
    

//private:    /// Attributs - m_membres (visibles par les methodes)
//    float m_speed;  //spedd memory
//    bool m_run;     //running memory
//    float m_DB;     //deadband

//protected:  /// Attibuts (accessibles seulement via les methodes)
//    PwmOut _pwm;
//    DigitalOut _av;
//    DigitalOut _ar;
        

#endif	//_yMOTOR_H

/**
 * @note \n
 * That's all folks!
 */
