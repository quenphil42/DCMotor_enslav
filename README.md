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

L'ensemble des fonctions en lien avec la commande de la MCC et l'initialisation de l'onduleur se trouvent dans les fichiers "commandeMCC.c" et "commandeMCC.h".

### 3.1. Generation de la PWM

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

### 3.2. Initialisation de l'onduleur

Afin d'enlever les securités de l'onduleur il suffit de fixer à l'etat haut la broche ISO_RESET pendant 2us comme indiqué dans la datasheet.
![image](https://user-images.githubusercontent.com/113909680/210418857-69e9e9cd-54bd-428a-bc00-c8a04b8304d1.png)

Nous avons donc utilisé la fonction HAL_Delay(1) qui nous permet de manière bloquante d'attendre environ 1ms ce qui est largement superieur aux 2us demandées. L'utilisation de HAL_Delay n'est pas dérangeante dans ce cas puisqu'elle n'intervient que dans la phase d'initialisation du module de puissance.

Suite à cela nous avons configuré une fonction init_Onduleur() (qui devrait s'appeler start) qu'on appelle depuis le shell et permet de de retirer les securités du module de puissance et fixe les valeurs de CCR à ARR_MAX_VALUE/2 afin d'avoir une tension moyenne à 0V et n'appeler aucun courant.
Celle-ci pouvait également être appelé en appuyant sur le bouton utilisateur, néanmoins nous avons préféré dédié ce bouton à l'activation (ou non) de l'ecriture des données dans le shell. Le code est néanmoins toujours présent en commentaire pour répondre aux attentes du TP.


### 3.3. Premiers tests

Afin de commander la MCC nous avons definis une fonction setAlpha qui permet depuis le shell de fixer la valeur de alpha entre 0 et 100 (commande en %). 0 correspond à la vitesse maximale de la MCC dans le sens "négatif", 50 correspond à la position neutre où la MCC ne tourne pas et 100 correspond à la vitesse maximale dans le sens "positif".

Le moteur tourne dans le sens "positif" lorsque la valeur des CCR est superieur à 512 (soit alpha > 50) et dans le sens "negatif" lorsque alpha est inferieur à ARR_MAX_VALUE/2 (soit alpha < 50).
Cependant si on demande à la MCC d'aller trop vite sans accelerer progressivement l'appel de courant est trop important et le module de puissance se met en securité.

En accelerant progressivement, le courant est plus modéré et on peut augmenter la vitesse de rotation. Nous avons ainsi réalisé une fonction Swing qui fait accelerer puis deceleré la MCC de facon progressive.

Le phenomene de mise en securité de l'onduleur est accentué puisque le gain en courant qui devrait être de l'ordre de 10 et en réalité de 3. Ainsi, au lieu de se mettre en sécurité à 2A l'onduleur coupe à 600mA.

Nous avons donc interet à réaliser un asservissement pour pouvoir commander en vitesse la MCC en prenant en compte le bridage en courant.



## 4. Asservissement

L'ensemble des fonctions en lien avec l'asservissement de la MCC se trouvent dans les fichiers "PI.c" et "PI.h".

Pour réaliser l'asservissement de la MCC nous avons réalisé deux boucles imbriquées et utilisé des correcteurs Proportionnels Integrals (PI) pour controler la commande en courant et en vitesse.

![image](https://user-images.githubusercontent.com/113909680/210424204-011c0492-4955-4fd3-9c45-afbd36972ec7.png)

Le réglage des correcteurs se fait étape par étape, nous avons du mettre en place et régler l'asservissement en courant dans un premier temps avant de réaliser celui en vitesse.

Afin que l'asservissement en vitesse se réalise correctement nous avons besoin que l'asservissement du courant soit stabilisé au moment de la mesure de vitesse. Cela implique donc que l'assercissement en courant doit être plus rapide que celui en vitesse. De l'ordre au moins de 10. On choisit de réaliser la lecture du courant avec le Timer1 (celui qui genere les PWM) Cela permet d'avoir une lecture de courant à une fréquence de 16kHz et de limiter les lectures de courant erronées dù à la transition des transistors.
La position angulaire est mesurée par le Timer4 à une fréquence de 16Hz

### 4.1. Asservissement en courant

Nous avons réalisé un asservissement en courant à la fréquence 16kHz selon le schéma donné ci-dessous

![image](https://user-images.githubusercontent.com/113909680/210427420-460d7f64-0b93-4ef2-9425-6d0ba6e48f71.png)


#### 4.1.1. Mesure du courant

La mesure du courant se réalise par un ADC couplé au DMA. Nous avons ainsi une mesure régulière du couran réalisé par l'ADC. celui ci envoi une interruption au DMA lors de la fin d'une conversion. Celui-ci va copier la valeure mesurée dans l'espace en mémoire associé. Ces opérations sont donc transparentes pour le processeur.

Pour avoir une mesure de courant correcte il faut ensuite traiter la donnée brut mesurée afin d'avoir une valeur de 0A lorsque alpha est à 50%. La relation est donnée dans la datasheet du module de puissance néanmoins nous l'avons modifié à cause de l'erreur de gain que nous avons remarqué.

#### 4.1.2. Anti-Wind-Up

#### 4.1.3. Mise en place de l'asservissement


### 4.2. Asservissement en vitesse

#### 4.1.1. Mesure de la position et vitesse

#### 4.1.2. Anti-Wind-Up

#### 4.1.3. Mise en place de l'asservissement




## FIN DU COMPTE RENDU
