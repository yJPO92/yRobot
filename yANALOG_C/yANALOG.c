/**
 *******************************************************************************
 * @file    yANALOG.c
 * @brief   Entree analogique elaboree et filtree
 * @version 4.0
 * @author  Jean92
 * @note    pour formatter une entree analogique\n
 *          lecture DMA via ADC\n
 *          echelle et valeur engenieur calcul√©e,\n
 *          echelle defaut = 0-4096\n
 *          filter y(n) = Coef * x(n) + (1 - Coef) * y(n-1)\n
 *          trim correction\n
 *          detection variation sur hysteresis
 *
 * v1.x Copyright (c) 2015 Jean92, https://developer.mbed.org/users/Jean92/
 * v2.0 migration ds TrueStudio et STM32L476RG
 *      on ne lit pas l'AI directement, on r√©cup√©re un item d'un tableau
 *      rempli par ADC/DMA du STM32L476
 * v3.0 add threshold & hysteresis to found a variation
 * 		and add output indicate modification
 * v4.0 migration to C
 *******************************************************************************
 * @date    Fev-2017, Juil-2020, aout-2020, sept-2020
 *******************************************************************************
 * @note    
 *******************************************************************************
 */

#include "yANALOG.h"
#include "stm32l4xx_hal.h"		//pour les types uint32_t...
//#include <stdlib.h>     /* abs */
//#include <cmath>


/* Initialisation data pour entrÈe analogique */
void yANALOG_Init(yANALOG* this){
	this->Ech_Mini = DEFAULT_ECH_MIN;
	this->Ech_Maxi = DEFAULT_ECH_MAX;
	this->DeadBand = DEFAULT_DEADBAND;
	this->Hysteresis = DEFAULT_HYSTERESIS;
	this->Trim = DEFAULT_TRIM;
	this->Coef_Filtre = DEFAUlT_COEF_FILTRAGE;
	this->UnMoinsCoef = 1.0 - DEFAUlT_COEF_FILTRAGE;
	this->Raw = 0U;
	this->PrevRaw = 0U;
	this->PVmemo = 0.0;
	this->PV = 0.0;
	/* A = (MAXscale - MINscale) / (MAXpoint - MINpoint)
	 * B = MAXscale-(A * MAXpoint) */
	this->A = (this->Ech_Maxi - this->Ech_Mini) / 4095.0;
	this->B = this->Ech_Maxi - (this->A * 4095.0);
	this->Ri = 0U;
	this->Ro = 0U;
}


/** Executer entree ana
 * sur ordre lorsque Ri change
 */
void yANALOG_Exec(yANALOG* this)
{

//TODO Ri/Ro schedule
//    //this->m_ValeurN = _ai.read();    // lire la valeur norm√©e de l'entree (la pin)
//    if (m_Ri == 1U) {
//    	CalulerMesure();
//    	m_Ri = 0U;		//raz reset input
//    	(m_Ro == 1U) ? (m_Ro = 0U) : (m_Ro = 1U);	//reset output
//    }
}

/* Calculer la mesure en fonction de l'echelle et du filtrage */
void yANALOG_CalulerPV(yANALOG* this)
{
	/* DÈtection_Variation Raw value */
	float tmp;
	uint32_t tmpR;
	tmpR = this->Raw - this->PrevRaw;
    (tmpR >= 0) ? (tmpR = tmpR) : (tmpR = -tmpR);	//abs!
	if (tmpR > this->DeadBand) {
		/* Mise a Echelle
		 * A = (MAXpercent - MINpercent) / (MAXma - MINma)
		 * A = (echMAX - echMIN) / 4095.0
		 * B = MAXpercent-(A*MAXma)
		 * B = echMAX - (A * 4095) ou echMiIN
		 * PV = A * Raw + B
		 */
		tmp = this->A * (float)this->Raw + this->B + this->Trim;
	    //ecretage Ech_Min / Ech_Max
	    if (tmp > this->Ech_Maxi) {tmp = this->Ech_Maxi;}
	    if (tmp < this->Ech_Mini) {tmp = this->Ech_Mini;}
	    this->PV = tmp;
		this->PrevRaw = this->Raw;	//memoriser
	} else {
		// nothing
	}

    //mesure filtree y(n) = Coef * x(n) + (1 - Coef) * y(n-1)
    this->PV = (this->Coef_Filtre * this->PV) + (this->UnMoinsCoef * this->PVmemo);
    this->PVmemo = this->PV; //memoriser valeur precedente

//    //check variation around hysteresis
//    tmp = m_PV - m_PVmemo;
//    (tmp >= 0) ? (tmp = tmp) : (tmp = -tmp);	//abs!
//    if (tmp >= m_Hysteresis) {
//    	(m_Ro == 1U) ? (m_Ro = 0U) : (m_Ro = 1U);	//output a variation
//    	m_PVmemo = m_PV;	//memo new stable value
//    }
}

/* Renvoyer la valeur en unite engenieur (mise a l'echelle) */
float yANALOG_GetPV(yANALOG* this)
{
    return this->PV;
}

///* Change l'echelle en cours de route */
//void yANALOG_FLT::majEchelle(float Ech_Min, float Ech_Max)
//{
//    this->m_Ech_Min = Ech_Min;
//    this->m_Ech_Max = Ech_Max;
//    CalulerMesure();
//}
//
///** Changer le coefficient de filtrage */
//void yANALOG_FLT::majFiltre(float Coef_Filtre)
//{
//    this->m_Coef_Filtre = Coef_Filtre;   // filtre
//    this->m_1moinsCoef = 1.0 - Coef_Filtre;
//    this->CalulerMesure();
//}


///* Stocker la mesure normalisÈee */
//void yANALOG_FLT::SetValeurN(float val)
//{
//    this->m_ValeurN = val;
//}

///** Ajuster le trim
//  * @param valeur du trim
//  */
// void yANALOG_FLT::AdjustTrim(float trim)
// {
//	 //TODO verifier limites!
//	 this->m_Trim = trim;
// }
//
///* Stocker la mesure 32bits */
//void yANALOG_FLT::SetRaw(uint32_t val)
//{
//    this->m_ValeurU32 = val;
//}
//
///* Renvoyer la Raw value */
//uint32_t yANALOG_FLT::GetRaw()
//{
//    return this->m_ValeurU32;
//}

///* Renvoyer l'hysteresis */
//float yANALOG_FLT::GetHysteresis()
//{
//    return this->m_Hysteresis;
//}
//


/*
 *   That's all folks!
 */
