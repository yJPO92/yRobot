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

///** Constructeur de la class yMOTOR */
//yMOTOR::yMOTOR(PinName pwm, PinName av, PinName ar):
//                _pwm(pwm), _av(av), _ar(ar) {
//     // herite de PWM et DO
//     // init PWM, init deadband
//    _pwm.period(yPeriod_def);
//    _pwm = 0.0;
//
//    // moteur a l'arret
//    _ar = 0;
//    _av = 0;
//

//}

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
	this->Velocity = 0.0;
	//--- Paramters
	this->Period = PERIODE_DEFAULT;
	this->DeadBand = DEADBAND_DEFAULT;
	//--- memories
	this->Speed_memo = 0.0;
	this->Run_memo = 0;
	this->DB_memo = DEADBAND_DEFAULT;
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
        this->inRun = 1;        	// état 'enMarche'
        //Speed(this->m_speed);   // appliquer la vitesse demandée
    }
    else if (mararr == yARRET) {
        //_ar = 0;                // mettre les 2 cdes a zero
        //_av = 0;                // ==> arret rotation
        this->inRun = 0;        	// état 'enArret'

    }
}

/* Speed_SP request */
void yMOTOR_Speed(yMOTOR* this, float speed) {
	//TODO check speed limits! & running
	this->Speed_SP = speed;
}

/* calcul moteur */
void yMOTOR_Exec(yMOTOR* this) {
	if (this->inRun == 1) {
		HAL_GPIO_WritePin(this->_gpioPortIN1, this->_gpioPinIN1, GPIO_PIN_SET);
		//HAL_GPIO_WritePin(MotDin1_GPIO_Port, MotDin1_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(this->_gpioPortIN2, this->_gpioPinIN2, GPIO_PIN_RESET);	//????

		//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);					//ok
	} else {
		HAL_GPIO_WritePin(this->_gpioPortIN1, this->_gpioPinIN1, GPIO_PIN_RESET);
		//HAL_GPIO_WritePin(MotDin1_GPIO_Port, MotDin1_Pin, GPIO_PIN_RESET);			//????

		HAL_GPIO_WritePin(this->_gpioPortIN2, this->_gpioPinIN2, GPIO_PIN_SET);

		//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);					//ok
	}

	this->_htimpwm.Instance->CCR2 = this->Speed_MV;
}

///** Traiter le changement de vitesse */
//void yMOTOR::Speed(float speed) {
//    if (this->m_run) {              // moteur running?
//        if (abs(speed) < this->m_DB) {     // oui, consigne suffisante?
//            _ar = 0;                    // non, les 2 cdes à zéro
//            _av = 0;
//            _pwm = 0.0;                 // reset PWM
//            this->m_speed = 0.0;        // memoriser vitesse nulle
//        }
//        else {                             // oui, en dehors de la bande morte
//            _av = (speed > this->m_DB);    // en avant
//            _ar = (speed < -this->m_DB);   // ou en arriere
//            _pwm = abs(speed);             // a la vitesse demandée
//            this->m_speed = speed;         // memoire vitesse
//        }
//    }
//}

///** Changer la periode de PWM */
//void yMOTOR::Period(float seconds) {
//    _pwm.period(seconds);
//}

///** Change DeadBand */
//void yMOTOR::DeadBand(float db) {
//    this->m_DB = db;
//}


/** Etat marche (running state) */
//bool yMOTOR::Run() {
//    //return (abs(this->m_speed) > yDB_def);
//    //return (_av != _ar);        //Marche si av et ar non nul
//    return this->m_run;     // repondre de mémoire!
//}

///** Vitesse effective */
//float yMOTOR::Velocity() {
//    return this->m_speed;
//}

 
/**
 * @note \n
 *         That's all folks!
 */
 
