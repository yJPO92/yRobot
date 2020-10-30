/**
 *******************************************************************************
 * @file    CubeMon.cpp
 * @author  Jean
 * @brief   prepare data for STM32CubeMonitor
 * @version
 *******************************************************************************
 * Modified :
 * Created  : 9 juil. 2020
 *******************************************************************************
 * @note
 *******************************************************************************
 */
#include "main.h"
#undef NR_VAR_GLO_
#include "CubeMon.h"


/*
  * @brief  Prepare or collect global data for/from STM32CubeMonitoer
  * @param  read(0) from STM32CubeMon /write(1) to STM32CubeMon
  * @retval none
*/
void yCopy2CubeMonitor(uint8_t rw){
	if (rw == 1) {		//write data to Node-RED
//		ymx_VRx_Raw = (uint32_t) adcbuf[0];
//		ymx_VRy_Raw = (uint32_t) adcbuf[1];

		ymx_VRx_PV = VRx.PV;
		ymx_VRx_Raw = VRx.Raw;
		ymx_VRx_Ri = VRx.Ri;
		ymx_VRx_Ro = VRx.Ro;
		ymx_VRx_trim = VRx.Trim;


		ymx_VRy_PV = VRy.PV;
		ymx_VRy_Raw = VRy.Raw;
		ymx_VRy_Ri = VRy.Ri;
		ymx_VRy_Ro = VRy.Ro;
		ymx_VRy_trim = VRy.Trim;

		//ymx_VRy_HystP = VRy.PVmemo + VRy.Hysteresis;
		//ymx_VRy_HystM = VRy.PVmemo - VRy.Hysteresis;

		//ymx_MotD = Moteur_D;	//!!! ne fonctionne qu'avec des var glo

	} else {	//read data from Node-red
		//VRy.m_Ri = ymx_VRy_Ri;
		//VRx.Coef_Filtre = ymx_Coef;
		//VRx.UnMoinsCoef = 1.0 - ymx_Coef;
		//VRy.Coef_Filtre = ymx_Coef;
		//VRy.UnMoinsCoef = 1.0 - ymx_Coef;
	}
}



/* That's all folks */
