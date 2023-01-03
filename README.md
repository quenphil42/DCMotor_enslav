# DCMotor_enslav - TP Systeme Acquisition et Commande ENSEA

L'objectif de ce TP est de réaliser l'asservissement d'une machine à courant continu (MCC) à l'aide de deux boucles imbriquées en vitesse et en courant.
Nous utiliserons tout au long du projet un shell permetant de réaliser une interface homme machine (IHM) afin de pouvoir tester nos programmes et récupérer des informations sans recompiler.

D'un point de vue hardware, nous sommes muni d'une NUCLEO-G474RE, d'un onduleur réalisant l'interface de puissance ainsi que d'une MCC munie d'un tachimètre. Aussi nous disposons d'un shield réalisant l'interface entre la NUCLEO, l'onduleur et le tachimètre.

Vous trouverez une documentation doxygène dans le dossier Src du projet STM32CubeIDE dans Git.



## 1. Config

Tout au long du projet nous avons travaillé sur le logiciel STM32CubeIDE version 1.10.1 

Pour débuter le projet nous avons configuré les broches d'entrées et de sorties de la NUCLEO. Cette configuration sera utile par la suite pour pouvoir réaliser les tâches suivantes :
  - Communication UART avec un ordinateur
  - Clignotement d'une LED
  - Utilisation du Bouton Utilisateur
  - Commande de l'onduleur
  - Lecture du courant
  - Lecture de la tension
 
Nous avons associés différents labels aux broches afin que le code soit le plus explicite possible

Ci-dessous vous trouverez la configuration que nous avons choisit pour ce TP.
![image](https://user-images.githubusercontent.com/113909680/210385445-a4003bec-f136-4493-abe0-0ddb433b127b.png)

Sur le connecteur 40 broches du shield on vient réaliser le branchement montré sur le schéma ci-dessous afin de connecter les broches physiques de l'onduleur aux broches configurées par la NUCLEO.
![image](https://user-images.githubusercontent.com/113909661/205322873-f4ddeada-e1db-4ac6-acd8-8282e608288a.png)



## 2. Configuration de l'UART et réalisation de l'IHM

Sous STM32CubeIDE nous avons configuré la communication UART2 avec interruption afin de permettre la connection par un port serie de la NUCLEO vers un ordinateur et inversement.

Dans un premier temps nous avons redéfinit la fonction "io_putchar" qui permet d'associer la fonction printf à la communication série.

Nous avons ensuite inclu dans le projet les fichiers shell2.c et shell2.h qui contiennent le code d'un shell réalisé par notre professeur. Ce shell permet de réaliser des fonctions associées à un nom d'appelle à ecrire par l'utilisateur. Il est facile de rajouter de nouvelles fonctions à ce shell mais aussi de travailler avec des interruptions puisque le fonctionnement du shell n'est pas bloquant. En effet l'appel aux fonctions du shell n'a lieu qu'à l'initialisation de la carte ainsi que lorsque le flag uartRxReceived est passé à True lors d'une interrupton.

Le Shell est muni d'une fonction help qui définit les differentes commandes qu'on peut effectuer ainsi qu'une description des fonctions.
Une fonction Pinout permet notemment de retrouver quels sont les branchements à réaliser pour pouvoir faire fonctionner l'onduleur.

!!!!!!!!! INCLURE PHOTO DU SHELL ICI !!!!!

!!!!! CHANGER IHM INIT PAR "start" et ajouter un "stop"



## 3. Commande MCC en boucle ouverte

FIN DU COMPTE RENDU
## 1. Config 
To start off, we configure the NUCLEO pins to the configuration givven right bellow:
![image](https://user-images.githubusercontent.com/113909661/195544779-ca40b81b-0b3d-45dd-8a04-904a3a217c6b.png)

On the custom made PCB, we need to connect the pins as following:
![image](https://user-images.githubusercontent.com/113909661/205322873-f4ddeada-e1db-4ac6-acd8-8282e608288a.png)

Remember to plug in the DC adapter for the custom board. 

## 2. UART setup for character echoing

We setup the UART2 to give the user an echo of what key is pressed.


