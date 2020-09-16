 /**
 *******************************************************************************
 * @file    yTask.h
 * @author  Jean
 * @brief   Outils pour mes taches
 * @version 3.1
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
	uint8_t Topic;
	float	PayLoadF;
	uint8_t PayloadI;
} yEvent_t;

/* Define structure to pass texte to display via VTdisplay */
//yDOC: objet d'échange vers la task d'affichage
typedef struct {
	int src;			// identifier l'emetteur
	char VTbuff[200];	// buffer a afficher
} yVTbuff_t;

/* Nommer les sources des messages ds les queues */
typedef enum {
  SrcNone = 0,
  SrcTk = 1,
} ySource;

/* Nommer les Topics des messages ds les queues */
typedef enum {
  TopicNone = 0,  BP1 = 1, SWXY = 2, Kbd =3,
  	  	  	  	  VRX = 11, VRY = 12,
				  MotD = 21, MotG = 22,
} yTopic;

/* Flag de démarrage des taches */
typedef enum  {
	TkNone = 0, TkInit, TkVTaffiche, TkCheckVR, TkProcess, TkDefault, TkAll,
} yTkOrder;

/* differents wait time for tasks */
#define Wait500ms	((uint32_t)500)		//0.5sec
#define Wait1s		((uint32_t)1000)	//1sec
#define Wait5s		((uint32_t)5000)	//5sec


#endif /* INC_YTASK_H_ */
//That's all folks!!
