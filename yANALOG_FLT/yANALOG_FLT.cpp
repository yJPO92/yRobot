/**
 *******************************************************************************
 * @file    yANALOG_FLT.cpp
 * @brief   Classe 'yANALOG_FLT' pour entree analogique elaboree et filtree
 * @version 3.0
 * @author  Jean92
 * @note    Class pour formatter une entree analogique\n
 *          lecture DMA via ADC\n
 *          echelle et valeur engenieur calculée,\n
 *          echelle defaut = 0.0/1.0\n
 *          filter y(n) = Coef * x(n) + (1 - Coef) * y(n-1)
 *          avec trim correction
 *
 * v1.x Copyright (c) 2015 Jean92, https://developer.mbed.org/users/Jean92/
 * v2.0 migration ds TrueStudio et STM32L476RG
 * v2.1 add trim
  * v3.0 add threshold & hysteresis to found a variation
 * 		and add output indicate modification
 *******************************************************************************
 * @date    Fev-2017, Juil-2020, Aout-2020
 *******************************************************************************
 * @note    
 *******************************************************************************
 */
#include "yANALOG_FLT.h"
//#include <stdlib.h>     /* abs */
//#include <cmath>

/** Constructeurs */
yANALOG_FLT::yANALOG_FLT(uint32_t *ptab)
{
    this->m_Ech_Min = DEFAULT_ECH_MIN;      // echelle par defaut
    this->m_Ech_Max = DEFAULT_ECH_MAX;
    this->m_Coef_Filtre = DEFAULT_COEF;
    this->m_1moinsCoef = 1.0 - DEFAULT_COEF;
    this->m_1moinsCoef = 0.0;
    this->m_PrevVal = (m_Ech_Max - m_Ech_Min) / 2.0;
    this->m_Trim = DEFAULT_TRIM;
    this->m_Hysteresis = DEFAULT_HYSTERESIS;
}

yANALOG_FLT::yANALOG_FLT(uint32_t *ptab, float Ech_Min, float Ech_Max, float Coef_Filtre)
{
    this->m_Ech_Min = Ech_Min;           // echelle speciale
    this->m_Ech_Max = Ech_Max;
    this->m_Coef_Filtre = Coef_Filtre;
    this->m_1moinsCoef = 1.0 - Coef_Filtre;
    this->m_PrevVal = (m_Ech_Max - m_Ech_Min) / 2.0;
    this->m_Trim = DEFAULT_TRIM;
    this->m_Hysteresis = DEFAULT_HYSTERESIS;
}

/* Destructeur */
yANALOG_FLT::~yANALOG_FLT()
{    /*
     * Rien a  mettre ici car on ne fait pas d'allocation dynamique
     * En temps normal, un destructeur fait souvent des delete et quelques
     * autres vÃ©rifications si nÃ©cessaire avant la destruction de l'objet.
     */
}

/** Executer entree ana
 * sur ordre lorsque Ri change
 */
void yANALOG_FLT::Exec(void)
{
    //this->m_ValeurN = _ai.read();    // lire la valeur normÃ©e de l'entree (la pin)
    if (m_Ri == 1U) {
    	CalulerMesure();
    	m_Ri = 0U;		//raz reset input
    	(m_Ro == 1U) ? (m_Ro = 0U) : (m_Ro = 1U);	//reset output
    }
}

/* Change l'echelle en cours de route */
void yANALOG_FLT::majEchelle(float Ech_Min, float Ech_Max)
{
    this->m_Ech_Min = Ech_Min;
    this->m_Ech_Max = Ech_Max;
    CalulerMesure();
}

/** Changer le coefficient de filtrage */
void yANALOG_FLT::majFiltre(float Coef_Filtre)
{
    this->m_Coef_Filtre = Coef_Filtre;   // filtre
    this->m_1moinsCoef = 1.0 - Coef_Filtre;
    this->CalulerMesure();
}

/* Renvoyer la valeur en unite engenieur (mise a l'echelle) */
float yANALOG_FLT::GetPV()
{
    return this->m_PV;
}

///* Stocker la mesure normaliséee */
//void yANALOG_FLT::SetValeurN(float val)
//{
//    this->m_ValeurN = val;
//}

/** Ajuster le trim
  * @param valeur du trim
  */
 void yANALOG_FLT::AdjustTrim(float trim)
 {
	 this->m_Trim = trim;
 }

/* Stocker la mesure 32bits */
void yANALOG_FLT::SetRaw(uint32_t val)
{
    this->m_ValeurU32 = val;
}

/* Renvoyer la Raw value */
uint32_t yANALOG_FLT::GetRaw()
{
    return this->m_ValeurU32;
}

/* Renvoyer l'hysteresis */
float yANALOG_FLT::GetHysteresis()
{
    return this->m_Hysteresis;
}

/* Calculer la mesure en fonction de l'echelle et du filtrage */
void yANALOG_FLT::CalulerMesure()
{
    //mise a echelle & add trim
	float tmp =  (this->m_Ech_Min
					+ (((this->m_Ech_Max -this-> m_Ech_Min) * (float)this->m_ValeurU32) / 4096.0))
					+ this->m_Trim;

    //ecretage Ech_Min / Ech_Max
    if (tmp > this->m_Ech_Max) {tmp = this->m_Ech_Max;}
    if (tmp < this->m_Ech_Min) {tmp = this->m_Ech_Min;}

    //mesure filtree y(n) = Coef * x(n) + (1 - Coef) * y(n-1)
    this->m_PV = (this->m_Coef_Filtre * tmp) + (this->m_1moinsCoef * this->m_PrevVal);
    this->m_PrevVal = this->m_PV; //memoriser valeur precedente

    //check variation around hysteresis
    tmp = m_PV - m_PVmemo;
    (tmp >= 0) ? (tmp = tmp) : (tmp = -tmp);	//abs!
    if (tmp >= m_Hysteresis) {
    	(m_Ro == 1U) ? (m_Ro = 0U) : (m_Ro = 1U);	//output a variation
    	m_PVmemo = m_PV;	//memo new stable value
    }
}


/*
 *   That's all folks!
 */
