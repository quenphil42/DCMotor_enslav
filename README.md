# DCMotor_enslav - TP Systeme Acquisition et Commande ENSEA

L'objectif de ce TP est de réaliser l'asservissement d'une machine à courant continu (MCC) à l'aide de deux boucles imbriquées en vitesse et en courant.
Nous utiliserons tout au long du projet un shell permetant de réaliser une interface homme machine (IHM) afin de pouvoir tester nos programmes et récupérer des informations sans recompiler.

D'un point de vue hardware, nous sommes muni d'une NUCLEO-G474RE, d'un onduleur réalisant l'interface de puissance ainsi que d'une MCC munie d'un tachimètre. Aussi nous disposons d'un shield réalisant l'interface entre la NUCLEO, l'onduleur et le tachimètre.

Vous trouverez une documentation doxygène dans le dossier Src du projet STM32CubeIDE dans Git.



## 1. Config 
To start off, we configure the NUCLEO pins to the configuration givven right bellow:
![image](https://user-images.githubusercontent.com/113909661/195544779-ca40b81b-0b3d-45dd-8a04-904a3a217c6b.png)

On the custom made PCB, we need to connect the pins as following:
![image](https://user-images.githubusercontent.com/113909661/205322873-f4ddeada-e1db-4ac6-acd8-8282e608288a.png)

Remember to plug in the DC adapter for the custom board. 

## 2. UART setup for character echoing

We setup the UART2 to give the user an echo of what key is pressed.


