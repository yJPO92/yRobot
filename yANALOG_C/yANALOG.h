/**
 *******************************************************************************
 * @file    yANALOG.h
 * @brief   Entree analogique elaboree et filtree
 * @version 4.3
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
 *      on ne lit pas l'AI directement, on rÃ©cupÃ©re un item d'un tableau
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

#ifndef _yANA_H
#define _yANA_H

#include <sys/types.h>			//definir types mcu like uint32_t

// valeurs par defaut
#define DEFAULT_ECH_MIN -100.0    	// Mini scale
#define DEFAULT_ECH_MAX 100.0    	// Maxi scale
#define DEFAULT_COEF_FILTRAGE 0.9	// 1.0=no filtre, 0.0=retard d'un sample
#define DEFAULT_TRIM 0.0			// Correction PV
#define DEFAULT_TRIMRAW 0			// Correction Raw
#define DEFAULT_HYSTERESIS 1.0		// hysteresis
 
/*
 * Structure d'une entrée analogique
 */
typedef struct {
	//--- Inputs
	uint32_t	Raw;		// 0-4095 points
    uint8_t 	Ri;			//pulse in (raz inside)
    //--- Outputs
	float		PV;			// Process Value
	float		PVa;		// abs(PV)
	int8_t		sens;		// (-1) PV neg, (0) indeterminé, (+1) PV pos.
    uint8_t 	Ro;			//pulse output
    //--- Parameters
	float		Ech_Mini;	// for scaling
	float		Ech_Maxi;
	float		A;			// coefs de mise à l'échelle
	float		B;
	float		Coef_Filtre;	// Coefficient de filtrage [0.0,1.0]
    float		UnMoinsCoef;	// 1 - Coeff
	float		Trim;			//correction sur PV
	uint32_t	TrimRaw;		//sur Raw
    float 		Hysteresis; 	//hysteresis sur PV
    //--- Memories
	float		PVmemo;		// valeur precedente
    float		PVhyst;		//memo PV pour hysteresis
} yANALOG;

//------------------------
// Methodes
//------------------------

/*
 * @brief  Initialisation data pour entrée analogique
 * @param  pointeur sur structure de l'entree analogique
 * @retval none
 */
void yANALOG_Init(yANALOG* this);

/*
 * @brief  Execution du calcul d'une nouvelle valeur de l'entrée analogique
 * @param  pointeur sur structure de l'entree analogique
 * @retval none
 */
void yANALOG_Exec(yANALOG* this);

/*
 * @brief  Calculer la mesure en fonction de l'echelle et du filtrage
 * @param  pointeur sur structure de l'entree analogique
 * @retval none
 */
void yANALOG_CalulerPV(yANALOG* this);

/*
  * @brief  check variation around hysteresis
  * @param  pointeur sur structure de l'entree analogique
  * @retval Ro
*/
uint8_t yANALOG_Variation(yANALOG* this);

/*
  * @brief  Modifier la valeur du Trim sur PV
  * @param  pointeur sur structure de l'entree analogique
  * @param  valeur du Trim
  * @retval status
*/
void yANALOG_SetTrim(yANALOG* this, float trim);

/*
  * @brief  Modifier la valeur du Trim Raw
  * @param  pointeur sur structure de l'entree analogique
  * @param  valeur du Trim sur RAw
  * @retval status
*/
void yANALOG_SetTrimRaw(yANALOG* this, float trimr);

//    /** Changer l'echelle de l'objet entree analogique
//     * @param Ech_Min
//     * @param Ech_Max la nouvelle echelle
//     */
//    void majEchelle(float Ech_Min, float Ech_Max);
//
//    /** Changer le coefficient de filtrage
//     * @param Coef_Filtre
//     */
//    void majFiltre(float Coef_Filtre);
//
    
//};

#endif	//_yANA_H
/*
 * That's all folks!
 */
