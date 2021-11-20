#include <khepera/khepera.h>

#define RATIO 0.678181
#define INCREMENT_P 25
#define INCREMENT_G 100

static knet_dev_t *dsPic; // robot pic microcontroller access

int init()
{
	printf("\nKhepera 4 - testkh4\n");

	// initiate libkhepera and robot access
	if (kh4_init(0, NULL) != 0)
	{
		printf("\nERROR: could not initiate the libkhepera!\n\n");
		return -1;
	}

	/* open robot socket and store the handle in its pointer */
	dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);

	if (dsPic == NULL)
	{
		printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
		return -2;
	}

	return 0;
}

void end()
{
	kb_change_term_mode(0);		// Terminal en mode initial
	kh4_set_speed(0, 0, dsPic); // Arret du robot
	kh4_SetMode(kh4RegIdle, dsPic);
}

int main()
{
	int sortieBoucle = 0,
		vitessePulse = 200,
		vitesseMMS = vitessePulse * RATIO;
	char toucheEnfoncee,
	     prec = 'a';

	int errorCode = init(); // Initialisation proposée par le template

	if (errorCode != 0)
	{
		return errorCode;
	}
	printf("Test des moteurs :\n");
	printf("Direction avec les ZQSD, arret avec A, sortie avec X.\n");
	printf("Augmentation de la vitesse (+ 25) avec B, Reduction avec N (-25)\n");
	printf("Augmentation de la vitesse (+ 100) avec H, Reduction avec J (- 100)\n");
	printf("(Vitesse comprises entre 5 et 1200)\n");

	kb_change_term_mode(1); // Changement du mode du terminal afin de pouvoir récuperer les entrées
	kh4_SetMode(kh4RegSpeed, dsPic);

	while (!sortieBoucle)
	{
		if (kb_kbhit()) // Fonction permettant de savoir si une touche est actuellement enfoncée
		{
			toucheEnfoncee = getchar();
			aSpeed:
			switch (toucheEnfoncee)
			{
			case 'z':
				kh4_set_speed(vitessePulse, vitessePulse, dsPic);
				prec = 'z';
				break;

			case 's':
				kh4_set_speed(-vitessePulse, -vitessePulse, dsPic);
				prec = 's';
				break;

			case 'd':
				kh4_set_speed(vitessePulse, -vitessePulse, dsPic);
				prec = 'd';
				break;

			case 'q':
				kh4_set_speed(-vitessePulse, vitessePulse, dsPic);
				prec = 'q';
				break;

			case 'a':
				kh4_set_speed(0, 0, dsPic);
				prec = 'a';
				break;

			case 'x':
				sortieBoucle = 1;
				break;

			case 'b' :
				vitessePulse += INCREMENT_P;
				if (vitessePulse > 1200)
					vitessePulse = 1200;
				vitesseMMS = vitessePulse * RATIO;
				printf("Vitesse actuelle : %d (%d mm/s)\n", vitessePulse, vitesseMMS);
				toucheEnfoncee = prec;
				goto aSpeed;
				break;
			
			case 'n' :
				vitessePulse -= INCREMENT_P;
				if (vitessePulse < 5)
					vitessePulse = 5;
				vitesseMMS = vitessePulse * RATIO;
				printf("Vitesse actuelle : %d (%d mm/s)\n", vitessePulse, vitesseMMS);
				toucheEnfoncee = prec;
				goto aSpeed;
				break;

			case 'h' :
				vitessePulse += INCREMENT_G;
				if (vitessePulse > 1200)
					vitessePulse = 1200;
				vitesseMMS = vitessePulse * RATIO;
				printf("Vitesse actuelle : %d (%d mm/s)\n", vitessePulse, vitesseMMS);
				toucheEnfoncee = prec;
				goto aSpeed;
				break;
			
			case 'j' :
				vitessePulse -= INCREMENT_G;
				if (vitessePulse < 5)
					vitessePulse = 5;
				vitesseMMS = vitessePulse * RATIO;
				printf("Vitesse actuelle : %d (%d mm/s)\n", vitessePulse, vitesseMMS);
				toucheEnfoncee = prec;
				goto aSpeed;
				break;

			default:
				break;
			}
		}
	}

	end();
	return 0;
}
