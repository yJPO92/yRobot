/**
 *******************************************************************************
 * @file    yANALOG.h
 * @brief   Entree analogique elaboree et filtree
 * @version 4.0
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
#define DEFAUlT_COEF_FILTRAGE 1.0	// 1.0=no filtre, 0.0=retard d'un sample
#define DEFAULT_TRIM 0.0			// Correction
#define DEFAULT_DEADBAND 5			// sur 0-4095
#define DEFAULT_HYSTERESIS 10.0		// hysteresis
 
/*
 * Structure d'une entr�e analogique
 */
typedef struct {
	uint32_t	Raw;		// 0-4095 points
	uint32_t	PrevRaw;	// Raw precedent
	uint32_t	DeadBand;	// bande morte
	float		PV;			// Process Value
	float		PVmemo;	// valeur precedente
	float		Ech_Mini;	// for scaling
	float		Ech_Maxi;
	float		Coef_Filtre;	// Coefficient de filtrage [0.0,1.0]
    float		UnMoinsCoef;	// 1 - Coeff
	float		A;			// coefs de mise � l'�chelle
	float		B;
	float		Trim;		//correction
    float 		Hysteresis; //hysteresis sur PV
    uint8_t 	Ri;			//pulse in (raz inside)
    uint8_t 	Ro;			//toggle output
} yANALOG;

//------------------------
// Methodes
//------------------------

/*
 * @brief  Initialisation data pour entr�e analogique
 * @param  pointeur sur structure de l'entree analogique
 * @retval none
 */
void yANALOG_Init(yANALOG* this);

/*
 * @brief  Execution du calcul d'une nouvelle valeur de l'entr�e analogique
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
 * @brief  Renvoi la PV de l'entr�e analogique
 * @param  pointeur sur structure de l'entree analogique
 * @retval PV en float
 */
float yANALOG_GetPV(yANALOG* this);




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
//    /** Renvoi la mesure
//     * @returns la valeur mise a l'echelle et filtree
//     */
//    float GetPV(void);
//
////    /** Stocker la mesure normee
////     * @param val la mesure au format (0.0-1.0)
////     */
////    void SetValeurN(float val);
//
//    /** Ajuster le trim
//      * @param valeur du trim
//      */
//     void AdjustTrim(float trim);
//
//    /** Stocker la mesure 32bits
//     * @param valeur du trim
//     */
//    void SetRaw(uint32_t val);
//
//    /** Renvoi la mesure Raw
//     * @param none
//     * @returns raw value
//     */
//    uint32_t GetRaw(void);
//
//    /** Renvoi l'hysteresisw
//     * @param none
//     * @returns value
//     */
//    float GetHysteresis(void);
//
    
//};

#endif	//_yANA_H
/*
 * That's all folks!
 */