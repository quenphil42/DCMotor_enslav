# DCMotor_enslav - TP Système Acquisition et Commande ENSEA


L'objectif de ce TP est de réaliser l'asservissement d'une machine à courant continu (MCC) à l'aide de deux boucles imbriquées en vitesse et en courant.
Nous utiliserons tout au long du projet un shell permettant de réaliser une interface homme machine (IHM) afin de pouvoir tester nos programmes et récupérer des informations sans recompiler.

D'un point de vue hardware, nous sommes muni d'une NUCLEO-G474RE, d'un onduleur réalisant l'interface de puissance ainsi que d'une MCC munie d'un tachymètre. Aussi nous disposons d'un shield réalisant l'interface entre la NUCLEO, l'onduleur et le tachymètre.

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
 
Nous avons associé différents labels aux broches afin que le code soit le plus explicite possible

Ci-dessous vous trouverez la configuration que nous avons choisi pour ce TP.

![image](https://user-images.githubusercontent.com/113909680/210385445-a4003bec-f136-4493-abe0-0ddb433b127b.png)

Sur le connecteur 40 broches du shield on vient réaliser le branchement montré sur le schéma ci-dessous afin de connecter les broches physiques de l'onduleur aux broches configurées par la NUCLEO.

![image](https://user-images.githubusercontent.com/113909661/205322873-f4ddeada-e1db-4ac6-acd8-8282e608288a.png)



## 2. Configuration de l'UART et réalisation de l'IHM


Sous STM32CubeIDE nous avons configuré la communication UART2 avec interruption afin de permettre la connexion par un port série de la NUCLEO vers un ordinateur et inversement.

Dans un premier temps nous avons redéfinit la fonction "io_putchar" qui permet d'associer la fonction printf à la communication série.

Nous avons ensuite inclus dans le projet les fichiers « shell2.c » et « shell2.h » qui contiennent le code d'un shell réalisé par notre professeur. Ce shell permet de réaliser des fonctions associées à un nom d'appelle à écrire par l'utilisateur. Il est facile de rajouter de nouvelles fonctions à ce shell mais aussi de travailler avec des interruptions puisque le fonctionnement du shell n'est pas bloquant. En effet l'appel aux fonctions du shell n'a lieu qu'à l'initialisation de la carte ainsi que lorsque le flag uartRxReceived est passé à True lors d'une interruption.

Le Shell est muni d'une fonction help qui définit les différentes commandes qu'on peut effectuer ainsi qu'une description des fonctions.
Une fonction Pinout permet notamment de retrouver quels sont les branchements à réaliser pour pouvoir faire fonctionner l'onduleur.

La fonction "start" est appelée initOnduleur dans notre projet.



## 3. Commande MCC en boucle ouverte


L'ensemble des fonctions en lien avec la commande de la MCC et l'initialisation de l'onduleur se trouvent dans les fichiers "commandeMCC.c" et "commandeMCC.h".


### 3.1. Génération de la PWM


Pour commander une MCC en boucle ouverte il suffit de commander 4 transistors de puissance présent dans l'onduleur. Au niveau de leur commande nous avons utilisé les voies TIM1_CH1, TIM1_CH1N, TIM1_CH2 et TIM1_CH2N toutes associées au Timer1.

La NUCLEO fonctionne à une fréquence de 160MHz comme montré dans la capture ci-dessous.

![image](https://user-images.githubusercontent.com/113909680/210412028-f46c2cc4-5f05-4c88-9eb3-dfda564588ef.png)

Ainsi nous avons paramétré le Timer1 avec un prescaler de 10 et un compteur à 1024 ce qui donne une fréquence de fonctionnement f_tim1 = 160.10^6 / 10 / 1024 = 15625 Hz et environ égale à 16kHz. 16kHz est la valeur préconisée par le constructeur du module de puissance comme l'indiquera la capture ci-dessous.

On dispose ainsi d'une plage de 1024 valeurs de alpha différente afin de piloter en vitesse notre MCC, soit une commande sur 10 bits comme le demande le cahier des charges.
Le Timer est paramétré pour générer sur 2 channels des signaux PWM complémentaires décalés.

Selon la documentation de l'onduleur dont un extrait est donné ci-dessous, des temps morts de 2us ou plus sont conseillés pour les six transistors du module de puissance.

![image](https://user-images.githubusercontent.com/113909680/210416528-0eb02254-e6ed-4c15-aeac-36896295c530.png)

Nous avons fixé les temps morts à la valeur 208. Ceux-ci permettent d'éviter les courts circuits pendant la période de commutation des transistors (qui est non nulle). Sur la NUCLEO on a : 
t_dt / t_dts = 2.10^-6 / (1/160 . 10^-6) = 320

Rappel : 
- pour x allant de   0 à 127, DTG[x] = x
- pour x allant de 128 à 254, DTG[x] = 127 + (x-127).2
- pour x allant de 255 à 508, DTG[x] = 254 + 4.x

Ainsi, La valeur du deadtime pour un ratio de 320 correspond à une valeur de DTG(7:0) : 208

![image](https://user-images.githubusercontent.com/113909680/210417477-edee0be2-4402-41d3-84e3-8fa7ea6dfa91.png)

![image](https://user-images.githubusercontent.com/113909680/210463052-790302da-0a43-412f-b76d-c4c1dd5ef6e6.png)


Aussi nous avons fixé la valeur du alpha à 512 = ARR_MAX_VALUE / 2 afin d'avoir une valeur moyenne vu par la MCC à 0V. Cela permet au moteur de rester statique et de n'appeller aucun courant à l'initialisation.

La commande des deux channels est complémentaire. Cela signifie que si le CaptureCompareRegister du Channel1 est à la valeur x, alors le CaptureCompareRegister du Channel2 est à la valeur ARR_MAX_VALUE - x.


### 3.2. Initialisation de l'onduleur


Afin d'enlever les sécurités de l'onduleur il suffit de fixer à l'état haut la broche ISO_RESET pendant 2us comme indiqué dans la datasheet.

![image](https://user-images.githubusercontent.com/113909680/210418857-69e9e9cd-54bd-428a-bc00-c8a04b8304d1.png)

Nous avons donc utilisé la fonction HAL_Delay(1) qui nous permet de manière bloquante d'attendre environ 1ms ce qui est largement supérieur aux 2us demandées. L'utilisation de HAL_Delay n'est pas dérangeante dans ce cas puisqu'elle n'intervient que dans la phase d'initialisation du module de puissance.

Suite à cela nous avons configuré une fonction init_Onduleur() (qui devrait s'appeler start) qu'on appelle depuis le shell et permet de de retirer les sécurités du module de puissance et fixe les valeurs de CCR à ARR_MAX_VALUE/2 afin d'avoir une tension moyenne à 0V et n'appeler aucun courant.
Celle-ci pouvait également être appelé en appuyant sur le bouton utilisateur, néanmoins nous avons préféré dédier ce bouton à l'activation (ou non) de l'écriture des données dans le shell. Le code est néanmoins toujours présent en commentaire pour répondre aux attentes du TP.


### 3.3. Premiers tests


Afin de commander la MCC nous avons définis une fonction setAlpha qui permet depuis le shell de fixer la valeur de alpha entre 0 et 100 (commande en %). 0 correspond à la vitesse maximale de la MCC dans le sens "négatif", 50 correspond à la position neutre où la MCC ne tourne pas et 100 correspond à la vitesse maximale dans le sens "positif".

Le moteur tourne dans le sens "positif" lorsque la valeur du CCR de la channel1 est supérieure à 512 (soit alpha > 50) et dans le sens "négatif" lorsque la valeur la valeur du CCR de la channel1 est inférieur à ARR_MAX_VALUE/2 (soit alpha < 50).
Cependant si on demande à la MCC d'aller trop vite sans accélérer progressivement l'appel de courant est trop important et le module de puissance se met en sécurité.

En accélérant progressivement, le courant est plus modéré et on peut augmenter la vitesse de rotation. Nous avons ainsi réalisé une fonction Swing qui fait accélérer puis décélérer la MCC de façon progressive.

Le phénomène de mise en sécurité de l'onduleur est accentué puisque le gain en courant qui devrait être de l'ordre de 12 et en réalité de 2.4. Ainsi, l'onduleur se met en sécuité si le courant d'appel est supérieur à 2.0A.

Nous avons donc intérêt à réaliser un asservissement pour pouvoir commander en vitesse la MCC en prenant en compte le bridage en courant.


## 4. Asservissement


Pour réaliser l'asservissement de la MCC nous avons réalisé deux boucles imbriquées et utilisé des correcteurs Proportionnels Intégral (PI) pour contrôler la commande en courant et en vitesse.

![image](https://user-images.githubusercontent.com/113909680/210424204-011c0492-4955-4fd3-9c45-afbd36972ec7.png)

Le réglage des correcteurs se fait étape par étape, nous avons dû mettre en place et régler l'asservissement en courant dans un premier temps avant de réaliser celui en vitesse.

Afin que l'asservissement en vitesse se réalise correctement nous avons besoin que l'asservissement du courant soit stabilisé au moment de la mesure de vitesse. Cela implique donc que l'asservissement en courant doit être plus rapide que celui en vitesse. De l'ordre au moins de 10. Nous avons choisit de réaliser la lecture du courant avec le Timer1 (celui qui génère les PWM). Cela permet d'avoir une lecture de courant à une fréquence de 16kHz et de limiter les lectures de courant erronées dû à la transition des transistors.
La position angulaire est mesurée par le Timer4 à une fréquence de 16Hz.


### 4.1. Architecture du correcteur PI


L'ensemble des fonctions en lien avec l'asservissement de la MCC se trouvent dans les fichiers "PI.c" et "PI.h".
Nous avons réalisé un asservissement PI et non PID car le système de base est initialement un système stable. Le correcteur proportionnel permet d'augmenter la vitesse du système et l'intégrateur le rend stable. Le dérivateur n'est donc pas nécessaire dans ce cas.

Nous avons créé une structure PIController contenant toutes les informations d'un correcteur PI.

![image](https://user-images.githubusercontent.com/113909680/210430937-806d9c96-ad23-4eaf-a659-4da9c2473d7a.png)

Ainsi puisque nous avons réalisés deux asservissements, les fonctions sont factorisées et les instances sont réalisées proprement et clairement.

Nous avons aussi réalisé des fonctions réalisant l'initialisation du correcteur (notamment lors de l'initialisation de l'onduleur) avec la fonction PIController_Init() ainsi que la mise à jour des valeurs avec la fonction PIController_Update().

Sont pris en compte dans la fonction PIController_Update() les limites par écrêtage ainsi que l'anti windup. L'anti windup permet de limiter l'emballement de l'intégrateur par saturation de la valeur calculée par le correcteur.


### 4.2. Asservissement en courant

Nous avons réalisé un asservissement en courant à la fréquence 16kHz selon le schéma donné ci-dessous

![image](https://user-images.githubusercontent.com/113909680/210427420-460d7f64-0b93-4ef2-9425-6d0ba6e48f71.png)


#### 4.2.1. Mesure du courant


La mesure du courant se réalise par un ADC couplé au Direct Memory Access (DMA). Nous avons ainsi une mesure régulière du courant réalisé par l'ADC. Celui-ci envoi une interruption au DMA lors de la fin d'une conversion. Celui-ci va copier la valeur mesurée dans l'espace en mémoire associé. Ces opérations sont donc transparentes pour le processeur.

Pour avoir une mesure de courant correcte il faut ensuite traiter la donnée brute mesurée afin d'avoir une valeur de 0A lorsque alpha est à 50%. La relation est donnée dans la datasheet du module de puissance. Néanmoins nous l'avons modifié à cause de l'erreur de gain que nous avons remarqué.
Nous avons donc mesuré la valeur du courant pour un alpha fixé à 50% puis mesuré plusieurs valeurs pour déterminer le gain réel par une régression linéaire.

On trouve ainsi la relation donnée ci-dessous

![image](https://user-images.githubusercontent.com/113909680/210429334-c086b4b3-aaff-470e-b4f5-9e0f42848184.png)

Où le gain réel est 2.4 (contrairement à la valeur constructeur donné à 12).


#### 4.2.2. Mise en place de l'asservissement

Pour mettre en place l'asservissement nous avons défini une instance de la structure PIController défini dans le fichier "PI.h". Celle-ci contient les valeurs de Kp, Ki ainsi que les limites, les deux mémoires et la sortie.

L'entrée du correcteur est la consigne de courant, sa sortie est la valeur de alpha le rapport cyclique des PWM.

Lors de l'initialisation nous fixons les valeurs des mémoires à 0 pour le correcteur proportionnel et 50% pour l'intégrale puisque nous souhaitons un courant stable à 0A et donc un alpha à 50%. Lors de l'initialisation de l'onduleur on réinitialise également les mémoires du PI pour supprimer la dérive de l'intégrateur dû aux imprécisions de lecture.

La fonction PIController_Update() intervient lors de l'interruption du Timer1 à 16kHz pour avoir l'asservissement le plus rapide possible

#### 4.2.3. Test

Une fois l'asservissement mis en place nous avons ajouté au shell une fonction permettant de rentrer la consigne en courant. Les essais sont concluants pour les coefficients suivants :
- KP_ALPHA = 0.10
- KI_ALPHA = 0.80

En rentrant une consigne de 2.0A et en mesurant le courant effectif via une sonde on peut valider que l'asservissement fonctionne correctement.

Théoriquement nous avions déterminé grâce à Matlab des valeurs du même ordre de grandeur mais moins robustes en réalité dû à la réalité du matériel.
Nous trouvons que le système reste long à atteindre le régime permanent et cela va certainement poser problème pour l'asservissement en vitesse.

En effet si l'asservissement en vitesse n'est pas assez rapide, alors l'asservissement en vitesse aura lieu sur un régime transitoire et le correcteur produira des valeurs erronées.

Remarque : Pour supprimer l'asservissement en courant il suffit de commenter le flag it_tim1 dans la fonction HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) 


### 4.3. Asservissement en vitesse


Nous avons réalisé un asservissement en vitesse à la fréquence 16Hz selon le schéma donné ci-dessous

![image](https://user-images.githubusercontent.com/113909680/210455661-f11ff4ee-be0b-435e-9206-ea522f6d47a8.png)


#### 4.3.1. Mesure de la position et vitesse

Afin de connaitre la vitesse de rotation du moteur, on doit réaliser la dérivée temporelle de la position qu'on peut obtenir grâce à un codeur incrémentale à 1024 positions.

Nous avons configuré le Timer2 en EncoderMode ce qui permet de récupérer la variation angulaire entre deux instants grâce au registre CNT du Timer.
A chaque interruption du Timer4 qui fonctionne à une fréquence de 16Hz on vient lire la valeur du registre CNT et stocker sa différence avec 2^31 (qui correspond à la moitié du TIM2_ARR_MAX_VALUE fixé à 2^32). 

![image](https://user-images.githubusercontent.com/113909680/210456595-a1aa6309-6671-483a-a070-a4c1eb4310b1.png)

On fixe ensuite la valeur du registre CNT à 2^31. Ainsi en considérant la vitesse maximale de la MCC, nous avons la certitude qu'en 62ms l'encodeur n'atteindra pas les 2^31 incréments qui ferait repasser le compteur à 0 ou 2^32 selon le sens de rotation de la MCC.

![image](https://user-images.githubusercontent.com/113909680/210457338-8b678ae5-d790-4b6b-bbad-10706f6453e3.png)

Pour obtenir la vitesse en tr/min on mesure entre deux instant le nombre d'incrément parcouru et on applique la relation suivante
Omega = (incrément / 1024) / (1 / f_echantillonage / 60) en tr/min


#### 4.3.3. Mise en place de l'asservissement

La mise en place de l'asservissement est très similaire à la mise en place de l'asservissement en courant. 
L'entrée du correcteur est la consigne de vitesse en tr/min, sa sortie est la valeur de consigne de courant en A.
Les mémoires sont initialisées toutes les deux à 0.0 puisque la sortie du PI est une consigne de courant qu'on désire nulle au moment de l'initialisation.
On définit une instance PI_vitesse qu'on initialise. A chaque interruption du Timer4 (à 16Hz) on mesure la vitesse et on execute la fonction PIController_Update().


#### 4.3.3. Test


Une fois l'asservissement mis en place nous avons ajouté au shell une fonction permettant de rentrer la consigne en vitesse. Les essais sont concluants pour les coefficients suivants :
- KP_ALPHA = 0.10  //contre  0.72 sur Matlab
- KI_ALPHA = 2.00  //contre 15.94 sur Matlab 

Nous avons rencontré quelques problèmes lors de la mise en place de l'asservissement en vitesse. Dans un premier temps car notre mesure de vitesse était erronée à 16Hz (elle était calculée sur la base d'une mesure à 1Hz ce qui rendait les valeurs incohérentes) ce qui avait pour conséquence de faire monter la MCC à vitesse maximale. En effet le correcteur souhaitait compenser l'erreur de lecture qui indiquait une vitesse 16 fois trop faible.
Puis nous nous sommes heurtés à un autre problème, la lenteur de l'asservissement en courant. Nous n'étions pas en régime établi en courant lors des mesures de vitesse ce qui rendait le système peu efficace.

Remarque : Pour supprimer l'asservissement en vitesse il suffit de commenter le flag it_tim4 dans la fonction HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) 


## 5. Conclusion

Durant ce TP nous avons pu mettre en application les différents outils vus durant les cours et TD : 

- Utilisation d'un shell
- Lecture d'un encodeur via un Timer
- Réaliser une lecture analogique et l'enregistrer grâce au Direct Memory Access (DMA)
- Générer des PWM alignés centrées avec gestion des temps morts
- Commander en boucle ouverte une MCC via un module de puissance
- Mise en place d'un PI
- Asservissement en courant
- Asservissement en vitesse
- Utilisation de doxygene et Github

Ces notions sont les bases de la programmation en système embarqué. Nous avons trouvé ce sujet très enrichissant et avons pris du plaisir à le travailler.
Pour aller plus loin, nous aurions pu mettre en place un OS (FreeRTOS par exemple) pour gérer les différentes tâches et interruptions pour intégrer ce TP dans un projet plus grand. Ce TP sera notamment utile pour la réalisation de notre projet RobotBeer où interviennent deux MCC avec codeur incrémental que nous désirons asservir en vitesse.

## FIN DU COMPTE RENDU
