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
	this->Speed_MV = 0;
	this->Sens = 0;
	this->Velocity = 0.0;
	//--- Paramters
	this->DutyCycle = DUTY_CYCLE_DEFAULT;
	this->DeadBand = DEADBAND_DEFAULT;
	//--- memories
	this->Speed_memo = 0.0;
	this->Run_memo = 0;
	this->DB_memo = DEADBAND_DEFAULT;
    //--- Virtual outputs
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
        this->inRun = 1;     // état 'enMarche'
        //Speed(this->m_speed);   // appliquer la vitesse demandée
    }
    else if (mararr == yARRET) {
        this->inRun = 0;    // état 'enArret'
        this->Speed_MV = 0;
    }
	//-- re exec motor
	yMOTOR_Exec(this);
}

/** Speed_SP request */
void yMOTOR_Speed(yMOTOR* this, float speed) {
	//TODO check speed limits! & running
	this->Speed_SP = speed;
	if (this->Speed_SP >= 0.0) {
		this->Speed_MV = (uint32_t)(this->Speed_SP + 0.5);
		this->Sens = 1;
	} else {
		this->Speed_MV = (uint32_t)(- this->Speed_SP - 0.5);
		this->Sens = -1;
	}
	//-- re exec motor
	yMOTOR_Exec(this);
}

/** calcul Real Outputs */
void yMOTOR_RealOutputs(yMOTOR* this) {
	if (this->inRun == 1) {
		HAL_GPIO_WritePin((uint32_t)this->_gpioPortIN1, (uint16_t)this->_gpioPinIN1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(this->_gpioPortIN2, this->_gpioPinIN2, GPIO_PIN_RESET);
		this->_htimpwm.Instance->CCR2 = this->Speed_MV;
	} else {
		HAL_GPIO_WritePin(this->_gpioPortIN1, this->_gpioPinIN1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(this->_gpioPortIN2, this->_gpioPinIN2, GPIO_PIN_SET);
		this->_htimpwm.Instance->CCR2 = 0;
	}

}

/** calcul moteur */
void yMOTOR_Exec(yMOTOR* this) {
	//--- a faire apres que qq chose ait changé!
	osDelay(pdMS_TO_TICKS(1000));	// test d'une fnc FreeRTOS ds s/fnc: OK
	yMOTOR_RealOutputs(this);
}

/**
 *
  	if (this.inRun == 1) {
		if (this->Speed_SP >= 0.0) {
			this->Speed_MV = (uint32_t)(this->Speed_SP + 0.5);
			this->Sens = 1;
		} else {
			this->Speed_MV = (uint32_t)(- this->Speed_SP - 0.5);
			this->Sens = -1;
		}
	} else {
		this->Speed_MV = 0;
        this->_ar = 0;      // mettre les 2 cdes a zero
        this->_av = 0;
        this->_pwm = 0;		// ==> arret rotation
	}
  *
 */

/**
 * @note \n
 *         That's all folks!
 */
 
