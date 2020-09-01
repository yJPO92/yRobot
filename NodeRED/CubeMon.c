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
//#include "yANALOG_FLT.h"
#undef NR_VAR_GLO_
#include "CubeMon.h"


/*
  * @brief  Prepare or collect global data for/from STM32CubeMonitoer
  * @param  read(0) from STM32CubeMon /write(1) to STM32CubeMon
  * @retval none
*/
void yCopy2CubeMonitor(uint8_t rw){
	if (rw == 1) {		//write data to Node-RED
		ymx_VRx_Raw = (uint32_t) adcbuf[0];
		ymx_VRy_Raw = (uint32_t) adcbuf[1];

//		ymx_VRx_Raw = VRx.GetRaw()
//		ymx_VRx_PV = VRx.m_PV;

//		ymx_VRy_Raw = VRy.GetRaw();
//		ymx_VRy_PV = VRy.m_PV;
//		ymx_VRy_HystP = VRy.m_PVmemo + VRy.GetHysteresis();
//		ymx_VRy_HystM = VRy.m_PVmemo - VRy.GetHysteresis();
//		ymx_VRy_Ro = (uint8_t) VRy.m_Ro * 10;
	} else {	//read data from Node-red
		//TODO
//		VRy.m_Ri = ymx_VRy_Ri;
	}
}



/* That's all folks */
