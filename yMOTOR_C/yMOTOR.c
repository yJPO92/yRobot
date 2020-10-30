/**
 *******************************************************************************
 * @file    yMOTOR.c
 * @brief   Controlleur pour Moteur par Pont-H (MC33886)"
 * @version 4.2
 * @author  Jean92
 * @note    pour formatter une entree analogique\n
 *          lecture DMA via ADC\n
 *          echelle et valeur engenieur calculÃ©e,\n
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

#include "yMOTOR.h"
#include "stm32l4xx_hal.h"		//pour les types uint32_t...
//#include "FreeRTOS.h"
//#include "cmsis_os.h"

/** Initialisation de la structure Motor */
void yMOTOR_Init(yMOTOR* this,
				uint32_t gpioPortIN1, uint16_t gpioPinIN1,
				uint32_t gpioPortIN2, uint16_t gpioPinIN2,
				TIM_HandleTypeDef htimpwm) {
	//--- Inputs
	this->MarArr = 0;
	this->Speed_SP = 0.0;
	//--- Outputs
	this->inRun = 0;
	//this->Velocity = 0.0;
	//--- Paramters
	this->DutyCycle = DUTY_CYCLE_DEFAULT;
	this->DeadBand = DEADBAND_DEFAULT;
	//--- memories
	this->Speed_memo = 0.0;
	this->Sens_memo = 0;
	this->Run_memo = 0;
	this->DB_memo = DEADBAND_DEFAULT;
	this->FeeWheel = 0U;
    //--- Virtual outputs
	this->Speed_MV = 0;
	this->Sens = 0;
	this->_pwm = 0;
	this->_av = 0;	//a verifier
	this->_ar = 0;
	//--- Real outputs
    this->_gpioPortIN1 = gpioPortIN1;
    this->_gpioPinIN1 = gpioPinIN1;
    this->_gpioPortIN2 = gpioPortIN2;
    this->_gpioPinIN2 = gpioPinIN2;
    this->_htimpwm= htimpwm;
}

/** Marche/Arret request */
void yMOTOR_MarArr(yMOTOR* this, uint8_t mararr) {
    if (mararr == yMARCHE) {
    	this->MarArr = 1;	// request Marche
        //this->inRun = 1;	// état 'enMarche'
    }
    else if (mararr == yARRET) {
    	this->MarArr = 0;	// request Arret
        //this->inRun = 0;    // état 'enArret'
        this->Speed_SP = 0;
        this->Sens = 0;
    }
	//-- re exec motor
	yMOTOR_Exec(this);
}

/** Speed_SP request */
void yMOTOR_Speed(yMOTOR* this, float speed) {
	//TODO check speed limits!
	//-- take it only if motor requested to run
	if (this->MarArr == 1) {
		this->Speed_SP = speed;
		//-- check SP outside Deaband
		if (this->Speed_SP >= this->DeadBand) {
			this->Speed_MV = (uint32_t)(this->Speed_SP + 0.5);	//ABS
			this->Sens = 1;
			this->inRun = 1;		//etat 'enMarche'
		} else if (this->Speed_SP <= -this->DeadBand) {
			this->Speed_MV = (uint32_t)(- this->Speed_SP - 0.5);	//ABS
			this->Sens = -1;
			this->inRun = 1;		//etat 'enMarche'
		} else {
			this->Speed_MV = 0;
			this->inRun = 0;		//etat 'enArret'
			//on ne sait pas si on va changer de sens
		}
		//-- memoriser
		this->Speed_memo = this->Speed_SP;
		//this->Sens_memo = this->Sens;
	}else {	//not in run
		this->Speed_MV = 0;
		this->Sens = 0;
		this->inRun = 0;		//etat 'enArret'
	}
	//-- re exec motor
	yMOTOR_Exec(this);
}

/** calcul Virtuals Outputs */
void yMOTOR_VirtualOutputs(yMOTOR* this) {
	if (this->inRun == 1) {
		switch (this->Sens) {
		case 1:
			this->_av = 1;
			this->_ar = 0;
			break;
		case 0:
			this->_av = 0;
			this->_ar = 0;
			break;
		case -1:
			this->_av = 0;
			this->_ar = 1;
			break;
		default:
			break;
		}
		this->_pwm = this->Speed_MV;
	} else {	// inRun=0
		this->_ar = 0;
		this->_av = 0;
		this->_pwm = 0;
	} //if inRun
}

/** calcul Real Outputs */
void yMOTOR_RealOutputs(yMOTOR* this) {
	uint8_t tmp_in1 = this->_av & !(this->_ar);
	HAL_GPIO_WritePin((uint32_t)this->_gpioPortIN1, (uint16_t)this->_gpioPinIN1, tmp_in1);

	uint8_t tmp_in2 = (!this->_av) & this->_ar;
	HAL_GPIO_WritePin(this->_gpioPortIN2, this->_gpioPinIN2, tmp_in2);

	this->_htimpwm.Instance->CCR2 = this->_pwm;		// duty cycle du timer
}

/** calcul moteur */
void yMOTOR_Exec(yMOTOR* this) {
	//--- a faire apres que qq chose ait changé!
	//	osDelay(pdMS_TO_TICKS(10));	// test d'une fnc FreeRTOS ds s/fnc: OK
	yMOTOR_VirtualOutputs(this);
	//-- check sens de marche
	if (((this->Sens == 1) && (this->Sens_memo == -1))
			|| ((this->Sens == -1) && (this->Sens_memo == 1))
			|| this->Sens == 0) {	//-- sens de marche a changé ou passage en deadband
		//-- marquer un temps en roue libre
		this->_ar = 0;
		this->_av = 0;
		this->_pwm = 0;
		this->FeeWheel = 1U;
		yMOTOR_RealOutputs(this);
		//osDelay(pdMS_TO_TICKS(2000));	//laisser le temps au courant de s'inverser
		osDelay(pdMS_TO_TICKS(2));	//TODO v3.4a DBG a supprimer
		this->FeeWheel = 0U;
		yMOTOR_VirtualOutputs(this);
		yMOTOR_RealOutputs(this);
	} else {	// sens de marche inchangé
		this->FeeWheel = 0U;
		yMOTOR_RealOutputs(this);
	} //if sens

	//-- memoriser le sens de marche
	this->Sens_memo = this->Sens;
}

/**
 * @note \n
 *         That's all folks!
 */
 
