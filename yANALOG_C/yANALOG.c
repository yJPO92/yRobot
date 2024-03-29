/**
 *******************************************************************************
 * @file    yANALOG.c
 * @brief   Entree analogique elaboree et filtree
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
 *      on ne lit pas l'AI directement, on récupére un item d'un tableau
 *      rempli par ADC/DMA du STM32L476
 * v3.0 add threshold & hysteresis to found a variation
 * 		and add output indicate modification
 * v4.0 migration to C
 * v4.1 some update (no deadband on raw)
 * v4.2 check hysteresis & .Ro
 * v4.3 abs de PV et sens
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


/* Initialisation data pour entr�e analogique */
void yANALOG_Init(yANALOG* this){
	//--- Inputs
	this->Raw = 0U;
	this->Ri = 0U;
    //--- Outputs
	this->PV = 0.0;
	this->PVa = 0.0;
	this->sens = 0;		//indetermin� (freewheeling)
	this->Ro = 0U;
    //--- Parameters
	this->Ech_Mini = DEFAULT_ECH_MIN;
	this->Ech_Maxi = DEFAULT_ECH_MAX;
	/* A = (MAXscale - MINscale) / (MAXpoint - MINpoint)
	 * B = MAXscale-(A * MAXpoint) */
	this->A = (this->Ech_Maxi - this->Ech_Mini) / 4095.0;
	this->B = this->Ech_Maxi - (this->A * 4095.0);
	this->Coef_Filtre = DEFAULT_COEF_FILTRAGE;
	this->UnMoinsCoef = 1.0 - DEFAULT_COEF_FILTRAGE;
	this->Trim = DEFAULT_TRIM;
	this->TrimRaw = DEFAULT_TRIMRAW;
	this->Hysteresis = DEFAULT_HYSTERESIS;
    //--- Memories
	this->PVmemo = 0.0;
	this->PVhyst = 0.0;
}


/** Executer entree ana
 * sur ordre ou lorsque Ri change
 */
void yANALOG_Exec(yANALOG* this)
{

// /** @todo Ri/Ro schedule */
//    //this->m_ValeurN = _ai.read();    // lire la valeur normée de l'entree (la pin)
//    if (m_Ri == 1U) {
//    	CalulerMesure();
//    	m_Ri = 0U;		//raz reset input
//    	(m_Ro == 1U) ? (m_Ro = 0U) : (m_Ro = 1U);	//reset output
//    }
}

/* Calculer la mesure en fonction de l'echelle et du filtrage */
void yANALOG_CalulerPV(yANALOG* this)
{
	/* D�tection_Variation Raw value */
	static float tmp;
	/* Mise a Echelle
	 * A = (MAXpercent - MINpercent) / (MAXma - MINma)
	 * A = (echMAX - echMIN) / 4095.0
	 * B = MAXpercent-(A*MAXma)
	 * B = echMAX - (A * 4095) ou echMiIN
	 * PV = A * (Raw + TrimRaw) + B + Trim
	 */
	//tmp = this->A * (float)this->Raw + this->B + this->Trim;
	tmp = this->A * (float)(this->Raw + this->TrimRaw) + this->B + this->Trim;
	//ecretage Ech_Min / Ech_Max
	if (tmp > this->Ech_Maxi) {tmp = this->Ech_Maxi;}
	if (tmp < this->Ech_Mini) {tmp = this->Ech_Mini;}
	this->PV = tmp;

	//mesure filtree y(n) = Coef * x(n) + (1 - Coef) * y(n-1)
	this->PV = (this->Coef_Filtre * this->PV) + (this->UnMoinsCoef * this->PVmemo);
	this->PVmemo = this->PV; //memoriser valeur precedente

	//extraire valeur absolue et sens
	if (this->PV >= 0.0) {
		this->PVa = this->PV;
		this->sens = 1;
	} else {
		this->PVa = -(this->PV);
		this->sens = -1;
	}
}

/* check variation around hysteresis */
uint8_t yANALOG_Variation(yANALOG* this)
{
	/** @todo verifier Ro */
	static float tmp;
	tmp = this->PV - this->PVhyst;
	(tmp >= 0) ? (tmp = tmp) : (tmp = -tmp);	//abs!
	if (tmp >= this->Hysteresis) {
		(this->Ro == 10U) ? (this->Ro = 0U) : (this->Ro = 10U);	//output a variation
		this->PVhyst = this->PV;	//memo new stable value
	} else {
		this->Ro = 0U;
	}
	return this->Ro;
}

/* Modifier la valeur du Trim sue PV */
void yANALOG_SetTrim(yANALOG* this, float trim)
{
	this->Trim = trim;
}

/* Modifier la valeur du Trim sur RAw*/
void yANALOG_SetTrimRaw(yANALOG* this, float trimr)
{
	this->TrimRaw = trimr;
}

///** Changer le coefficient de filtrage */
/** @todo maj filtrage */
//void yANALOG_FLT::majFiltre(float Coef_Filtre)
//{
//    this->m_Coef_Filtre = Coef_Filtre;   // filtre
//    this->m_1moinsCoef = 1.0 - Coef_Filtre;
//    this->CalulerMesure();
//}


/*
 *   That's all folks!
 */
