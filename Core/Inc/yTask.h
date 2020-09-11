 /**
 *******************************************************************************
 * @file    yTask.h
 * @author  Jean
 * @brief   Outils pour mes taches
 * @version 3.0
 *******************************************************************************
 * Modified : aout 2020 (pour NewCpp)
 * Modified : 16 avr. 2020
 * Created  : janv. 2020
 *******************************************************************************
 * @note
 *******************************************************************************
 */

#ifndef INC_YTASK_H_
#define INC_YTASK_H_

/* Define the structure type of events will be passed on the queue. */
typedef struct {
	uint8_t Src;
	uint8_t Topic;
	float	PayLoadF;
	uint8_t PayloadI;
} yEvent_t;

/* Define structure to pass texte to display via VTdisplay */
//yDOC: objet d'�change vers la task d'affichage
typedef struct {
	int src;			// identifier l'emetteur
	char VTbuff[200];	// buffer a afficher
} yVTbuff_t;

/* Nommer les sources des messages ds les queues */
typedef enum {
  SrcNone = 0, SrcBP1 = 1, SrcSWxy = 2, SrcVRx = 5, SrcVRy = 6, SrcVRxy= 7, SrcKbd = 11, SrcTk = 20,
} ySource;

/* Nommer les destinations des messages ds les queues */
typedef enum {
  DestNone  = 50, Process = 51, VTaffiche = 52,
} yDestination;

/* Nommer les Topics des messages ds les queues */
typedef enum {
  TopicNone = 0, VR_PV = 1, VR_Raw = 2, VR_xxx = 3, BP1 = 4, SWxy = 5, MotD = 11, MotG = 12,
} yTopic;

/* Flag de d�marrage des taches */
typedef enum  {
	TkNone = 0, TkInit, TkVTaffiche, TkCheckVR, TkProcess, TkDefault, TkAll,
} yTkOrder;

/* differents wait time for tasks */
#define Wait500ms	((uint32_t)500)		//0.5sec
#define Wait1s		((uint32_t)1000)	//1sec
#define Wait5s		((uint32_t)5000)	//5sec


#endif /* INC_YTASK_H_ */
//That's all folks!!
