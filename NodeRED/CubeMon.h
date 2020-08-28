/**
 *******************************************************************************
 * @file    CubeMon.h
 * @author  Jean
 * @brief   prepare data for STM32CubeMonitor
 * @version
 *******************************************************************************
 * Modified :
 * Created  : 10 juil. 2020
 *******************************************************************************
 * @note	#undef NR_VAR_GLO_ before including this file in correspond .c file
 * 				to DECLARE global variables and necessaty extern variables.
 * 			Include this file with defining NR_VAR_GLO_ in other .c file
 * 				which does known functions.
 * 			---------------------
 * 			fixed location for Node-RED variables
 * 			__attribute__((section(".myvars.CRC"))) uint32_t CRC;
 *******************************************************************************
 */

#ifndef CUBEMON_H_
#define CUBEMON_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Variables pour Mode-RED/STM32CubeMonitor */
#ifndef NR_VAR_GLO_		//for CubeMon.cpp
 #define NR_VAR_GLO_
 //declarer variables globales pour Node-RED
 uint32_t ymx_VRx_Raw = 0;
 uint32_t ymx_VRy_Raw = 0;
 // at specific range address
 // order given by following declaration (order in .ld nor working)
 float ymx_VRx_PV __attribute__((section(".myvars"))) = 0.0;
 float ymx_VRx_trim  __attribute__((section(".myvars"))) = 0.0;

 float ymx_VRy_PV __attribute__((section(".myvars"))) = 0.0;
 float ymx_VRy_trim  __attribute__((section(".myvars"))) = 0.0;
 float ymx_VRy_HystP __attribute__((section(".myvars"))) = 0.0;
 float ymx_VRy_HystM __attribute__((section(".myvars"))) = 0.0;
 uint8_t ymx_VRy_Ri __attribute__((section(".myvars"))) = 0;
 uint8_t ymx_VRy_Ro __attribute__((section(".myvars"))) = 0;

 float ymx_Coef  __attribute__((section(".myvars"))) = 1.0;

 //et en 'extern" necessary variable
// extern yANALOG_FLT VRx;
// extern yANALOG_FLT VRy;

 #else		//for others .c/.cpp
 extern float ymx_VRx_trim;
 extern float ymx_VRy_trim;
 extern float ymx_Coef;

 //extern yANALOG_FLT VRx;
 //extern yANALOG_FLT VRy;
#endif

/* define functions for corresponding and others .c files */

/*
  * @brief  Prepare or collect data for/from STM32CubeMonitoer
  * @param  read(0) from STM32CubeMon /write(1) to STM32CubeMon
  * @retval none
*/
 void yCopy2CubeMonitor(uint8_t rw);

#ifdef __cplusplus
}
#endif

#endif /* CUBEMON_H_ */
//That's all folks!!
