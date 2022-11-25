#include "shell_fonctions.h"
#include "commandeMCC.h"


int shell_init_fonctions(void)
/* fonction qui add les differentes fonctions qu'on souhaite dans le shell*/
{
	shell_add('f', fonction_bidon, "Une fonction inutile");
	shell_add('a', addition, "Une fonction qui additionne");
	shell_add('p', pinout, "Une fonction qui renvoi l'association des pins");
	shell_add('s', Init_Onduleur, "Une fonction qui envoi la commande d'init de l'onduleur");
	shell_add('o', Swing,"Une fonction qui fait osciller le moteur 3 fois");
	shell_add('m', SetAlpha, "Commande moteur avec alpha");
	shell_add('p', pinout, "Donne le pinout du syst");
	return 0;
}

int fonction_bidon(int argc, char ** argv)
{
	printf("Je suis une fonction bidon\r\n");

	printf("argc = %d\r\n", argc);

	for(int i=0; i<argc; i++)
	{
		printf("arg numero %d = %s \r\n", i, argv[i]);
	}
	return 0;
}

int addition(int argc, char ** argv)
{
	if(argc !=3)
	{
		printf("Error y a pas le bon nombre d'argument");
		return -1;
	}
	else
	{
		int a = atoi(argv[1]);
		int b = atoi(argv[2]);
		printf("%d + %d = %d\r\n", a, b, a+b);

	}
	return 0;
}

int pinout(int arc, char ** argv)
{
	printf("Les cables a installer pour faire fonctionner la MCC sont :\r\n\n");
	printf("Alim externe de puissance ( avec terre ) 48V\r\n");
	printf("Alim onduleur <-> moteur\r\n\n");
	printf("Les pins indiquees sont les pins reelles et non celles indiquees sur la carte\r\n\n");
	printf("GND  : 20\r\n");
	printf("PC3  : 34\r\n"); //Enable
	printf("D8   : 12\r\n");
	printf("D7   : 11\r\n");
	printf("PA11 : 30\r\n");
	printf("PA12 : 31\r\n");

}
