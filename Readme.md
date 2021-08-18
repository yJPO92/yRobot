<!---
\mainpage
-->

# Project Title : yRobot

Draft pour Meccano Robot    
Mixed STM32CubeMX, FreeRTOS, C

## STM32CubeMX

Cr&eacute;ation de l'appli.  

## Fonctionnalities
Manage 2 driving motors  
Use Jostick  
*future* Receive order from PocketBeagle in Node-Red  

##### Inputs
BP1 (interrupt) : ???  
Jostick VRx, VRy, SWxy
* VRx : direction
* VRy : vitesse 
* SWxy : Marche/Arret  

*future* 5 GPio order from PocketBeagle (F,R,D,G,S)

Menu VT100 : envoie de commandes  
* key A|B : Arrêt|Marche

##### Outputs
Onboard led LD2 : running  
Menu VT100, display status  
STM32CubeMonitor (Node-RED)

##### 
6 task (inclus default)  
6 queue

## Version
##### v3.x: update Node-red 
##### v2.x: ebauche tasks
##### v1.x: creation à partir de NewCpp + FreeRTOS

##Program design

Program organization is describe in drawing 'yRobotFreeRTOS.xml' done with draw.io  
 [https://app.diagrams.net/](https://app.diagrams.net/)

## Authors

**JPO** - *FreeRTOS, C in TrueStudio*

## License

This project is licensed under the [CC0 1.0 Universal](LICENSE.md)
Creative Commons License - see the [LICENSE.md](LICENSE.md) file for
details
