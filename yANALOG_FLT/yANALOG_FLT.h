/**
 *******************************************************************************
 * @file    yANALOG_FLT.h
 * @brief   Classe 'yANALOG_FLT' pour entree analogique elaboree et filtree
 * @version 3.0
 * @author  Jean92
 * @note    Class pour formatter une entree analogique\n
 *          lecture DMA via ADC\n
 *          echelle et valeur engenieur calculée,\n
 *          echelle defaut = 0.0/1.0\n
 *          filter y(n) = Coef * x(n) + (1 - Coef) * y(n-1)
 *
 * v1.x Copyright (c) 2015 Jean92, https://developer.mbed.org/users/Jean92/
 * v2.0 migration ds TrueStudio et STM32L476RG
 *      on ne lit pas l'AI directement, on récupére un item d'un tableau
 *      rempli par ADC/DMA du STM32L476
 * v3.0 add threshold & hysteresis to found a variation
 * 		and add output indicate modification
 *******************************************************************************
 * @date    Fev-2017, Juil-2020, aout-2020
 *******************************************************************************
 * @note    
 *******************************************************************************
 */
#ifndef _yANA_H
#define _yANA_H

#include <sys/types.h>			//definir types mcu like uint32_t

// valeurs par defaut
#define DEFAULT_ECH_MIN 0.0     // Mini scale
#define DEFAULT_ECH_MAX 1.0     // Maxi scale
#define DEFAULT_COEF 0.2		// Coef de filtrage
#define DEFAULT_TRIM 0.0		// Correction
#define DEFAULT_HYSTERESIS 15.0	// hysteresis

class yANALOG_FLT
{
public:     // Methodes
    /** Constructeur par defaut/Creation de l'objet entree analogique\n
     * @param pointeur to item of array
     */
     yANALOG_FLT(uint32_t *ptab);
    
    /** Constructeur/Creation de l'objet entree analogique\n
     *  avec echelle specifique et coef de filtrage
     * @param pointeur to item of array
     * @param Ech_Min
     * @param Ech_Max la nouvelle echelle
     * @param Coef_Filtre
     */
    yANALOG_FLT(uint32_t *ptab, float Ech_Min, float Ech_Max, float Coef_Filtre);
    
    /// Destructeur
    ~yANALOG_FLT();
 
    /** Executer entree ana
     * sur ordre lorsque Ri change
     */
    void Exec();

    /** Changer l'echelle de l'objet entree analogique
     * @param Ech_Min
     * @param Ech_Max la nouvelle echelle
     */
    void majEchelle(float Ech_Min, float Ech_Max);

    /** Changer le coefficient de filtrage
     * @param Coef_Filtre
     */
    void majFiltre(float Coef_Filtre);

    /** Renvoi la mesure
     * @returns la valeur mise a l'echelle et filtree
     */
    float GetPV(void);

//    /** Stocker la mesure normee
//     * @param val la mesure au format (0.0-1.0)
//     */
//    void SetValeurN(float val);

    /** Ajuster le trim
      * @param valeur du trim
      */
     void AdjustTrim(float trim);

    /** Stocker la mesure 32bits
     * @param valeur du trim
     */
    void SetRaw(uint32_t val);

    /** Renvoi la mesure Raw
     * @param none
     * @returns raw value
     */
    uint32_t GetRaw(void);

    /** Renvoi l'hysteresisw
     * @param none
     * @returns value
     */
    float GetHysteresis(void);

    /** Calculer la mesure en fonction de l'echelle et du filtrage
     */
    void CalulerMesure(void);
    
/* Attributs (m_membres) */
    // Input / output of object
    /// process value (engenieur filtree)
    float m_PV;
    /// reset input et output
    uint8_t m_Ri;	//pulse in (raz inside)
    uint8_t m_Ro;	//toggle output

    float m_PVmemo;

private:

protected:  /// Attibuts (accessibles seulement via les methodes)
    /// raw value (in point if possible?)
    uint32_t m_ValeurU32;
    /// correction
    float m_Trim;
    /// valeur norm�e (reste du Seeeduino Arch Pro)
    float m_ValeurN;
    /// Echelle mini
    float m_Ech_Min;
    /// Echelle maxi
    float m_Ech_Max;
    /// Coefficient de filtrage
    float m_Coef_Filtre;
    /// 1 - Coeff
    float m_1moinsCoef;
    /// valeur precedente
    float m_PrevVal;
    /// hysteresis
    float m_Hysteresis;
    /// memorisation PV si depassement hysteresis
    //float m_PVmemo;
};

#endif
/*
 * That's all folks!
 */
