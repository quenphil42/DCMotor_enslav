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

# 3.1. Generation de la PWM

Pour commander une MCC en boucle ouverte il suffit de commander 4 transistors de puissance présent dans l'onduleur. Au niveau de leur commande nous avons utilisé les voies les broches TIM1_CH1, TIM1_CH1N, TIM1_CH2 et TIM1_CH2N toutes associées au Timer1.

La NUCLEO fonctionne à une frequence de 160MHz comme montré dans la capture ci-dessous.
![image](https://user-images.githubusercontent.com/113909680/210412028-f46c2cc4-5f05-4c88-9eb3-dfda564588ef.png)

Ainsi nous avons paramétré le Timer1 avec un prescaler de 10 et un compteur à 1024 ce qui donne une fréquence de fonctionnement f_tim1 = 160.10^6 / 10 / 1024 = 15625 Hz et environ égale à 16kHz. 16kHz est la valeur preconnisée parle constructeur du module de puissance comme l'indiquera la capture ci-dessous.

On dispose ainsi d'une plage de 1024 valeur de alpha differente afin de piloter en vitesse notre MCC, soit une commande sur 10 bits comme le demande le cahier des charges.
Le Timer est paramétré pour generé sur 2 channels des signaux PWM complémentaires décalés.

Selon la documentation de l'onduleur dont un extrait est donné ci-dessous, des temps morts de 2us ou plus sont conseillés pour les six tranistor du module de puissance.
![image](https://user-images.githubusercontent.com/113909680/210416528-0eb02254-e6ed-4c15-aeac-36896295c530.png)

Nous avons fixés les temps morts à la valeur 208. Ceux-ci permettent d'eviter les courts circuits pendant la periode de commutation des transitors (qui est non nulle). Sur la NUCLEO cela correspond à une valeur de 
dt = [127 + 2 * (208-128)] * tds = !!!!!!! INSERER VALEUR !!!!!!!  > 2 us recommandés.

![image](https://user-images.githubusercontent.com/113909680/210417477-edee0be2-4402-41d3-84e3-8fa7ea6dfa91.png)

Aussi nous avons fixé la valeur du alpha à 512 = ARR_MAX_VALUE / 2 afin de fixer la valeur moyenne vu par la MCC à 0V afin quelle reste statique et n'appelle pas un courant trop élevé à l'initialisation.

La commande des deux channels est complémentaire. Cela signifie que si le CaptureCompareRegister du Channel1 est à la valeur x, alors le CaptureCompareRegister du Channel2 est à la valeur ARR_MAX_VALUE - x.

# 3.2. Initialisation de l'onduleur

Afin d'enlever les securités de l'onduleur il suffit de fixer à l'etat haut la broche ISO_RESET pendant 2us comme indiqué dans la datasheet.
![image](https://user-images.githubusercontent.com/113909680/210418857-69e9e9cd-54bd-428a-bc00-c8a04b8304d1.png)

Nous avons donc utilisé la fonction HAL_Delay(1) qui nous permet de manière bloquante d'attendre environ 1ms ce qui est largement superieur aux 2us demandées. L'utilisation de HAL_Delay n'est pas dérangeante dans ce cas puisqu'elle n'intervient que dans la phase d'initialisation du module de puissance.

Suite à cela nous avons configuré une fonction init_Onduleur() (qui devrait s'appeler start) qu'on appelle depuis le shell et permet de de retirer les securités du module de puissance et fixe les valeurs de CCR à ARR_MAX_VALUE/2 afin d'avoir une tension moyenne à 0V et n'appeler aucun courant.
Celle-ci pouvait également être appelé en appuyant sur le bouton utilisateur, néanmoins nous avons préféré dédié ce bouton à l'activation (ou non) de l'ecriture des données dans le shell. Le code est néanmoins toujours présent en commentaire pour répondre aux attentes du TP.




## FIN DU COMPTE RENDU
## 1. Config 
To start off, we configure the NUCLEO pins to the configuration givven right bellow:
![image](https://user-images.githubusercontent.com/113909661/195544779-ca40b81b-0b3d-45dd-8a04-904a3a217c6b.png)

On the custom made PCB, we need to connect the pins as following:
![image](https://user-images.githubusercontent.com/113909661/205322873-f4ddeada-e1db-4ac6-acd8-8282e608288a.png)

Remember to plug in the DC adapter for the custom board. 

## 2. UART setup for character echoing

We setup the UART2 to give the user an echo of what key is pressed.


