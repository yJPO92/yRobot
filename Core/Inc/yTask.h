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
	uint8_t Source;
	uint8_t Destination;
	uint8_t Topic;
	float	PayLoad;
} yEvent_t;

/* Define structure to pass texte to display via VTdisplay */
//yDOC: objet d'échange vers la task d'affichage
typedef struct {
	int src;			// identifier l'emetteur
	char VTbuff[200];	// buffer a afficher
} VTbuff_t;

/* Nommer les sources des messages ds les queues */
typedef enum {
  SrcNone = 0, SrcBP1 = 1, SrcVRx = 5, SrcVRy = 6, SrcKbd = 11,
} ySource;

/* Nommer les destinations des messages ds les queues */
typedef enum {
  DestNone  = 50, Process = 51, VTaffiche = 52,
} yDestination;

/* Nommer les Topics des messages ds les queues */
typedef enum {
  TopicNone = 0, VR_PV = 1, VR_Raw = 2, VR_xxx = 3,
} yTopic;

/* Flag de démarrage des taches */
enum yTkOrder {
	TkNone = 0, TkInit, TkVTaffiche, TkCheckVR, TkProcess, TkDefault, TkAll,
};

/* */
const uint16_t Wait1s = 1000U;		//1sec
const uint16_t Wait5s = 5000U;		//5sec



// not used -------------------------------------------------------
// not used -------------------------------------------------------
// not used -------------------------------------------------------
/* Define structure des messages console pour manager les tasks */
typedef struct {
	uint8_t Source;
	uint8_t Destination;
	uint8_t PayLoad;
} yMngTk;

/* Liste de commandes transmises par message ds queue */
typedef enum {
	tkSuspend = 0, tkResume = 1,
} CmdMsg;

#endif /* INC_YTASK_H_ */
//That's all folks!!
