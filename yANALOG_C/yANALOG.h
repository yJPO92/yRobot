/**
 *******************************************************************************
 * @file    yANALOG.h
 * @brief   Entree analogique elaboree et filtree
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
 *      on ne lit pas l'AI directement, on rÃ©cupÃ©re un item d'un tableau
 *      rempli par ADC/DMA du STM32L476
 * v3.0 add threshold & hysteresis to found a variation
 * 		and add output indicate modification
 * v4.0 migration to C
 * v4.1 some update (no deadband on raw)
 * v4.2 check hysteresis & .Ro
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
#define DEFAULT_COEF_FILTRAGE 0.8	// 1.0=no filtre, 0.0=retard d'un sample
#define DEFAULT_TRIM 0.0			// Correction
#define DEFAULT_HYSTERESIS 15.0		// hysteresis
 
/*
 * Structure d'une entrée analogique
 */
typedef struct {
	uint32_t	Raw;		// 0-4095 points
	float		PV;			// Process Value
	float		PVmemo;		// valeur precedente
	float		Ech_Mini;	// for scaling
	float		Ech_Maxi;
	float		Coef_Filtre;	// Coefficient de filtrage [0.0,1.0]
    float		UnMoinsCoef;	// 1 - Coeff
	float		A;			// coefs de mise à l'échelle
	float		B;
	float		Trim;		//correction
    float 		Hysteresis; //hysteresis sur PV
    float		PVhyst;		//memo PV pour hysteresis
    uint8_t 	Ri;			//pulse in (raz inside)
    uint8_t 	Ro;			//pulse output
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


//
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
