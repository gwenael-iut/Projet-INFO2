/* @(#) test_odometer.c */
/* Test sur l'odomètre */

#include <khepera/khepera.h>
#include <khepera/kb_khepera4.h>

static knet_dev_t *dsPic; 

int init(void)
{
	printf("-------------------\n");
	printf("TEST SUR L'ODOMÈTRE\n");
	printf("-------------------\n");

	if (kh4_init(0, NULL) != 0)
	{
		printf("\nERROR: could not initiate the libkhepera!\n\n");
		return(-1);
	}
	
	dsPic = knet_open("Khepera4:dsPic", KNET_BUS_ISC, 0, NULL);
	
	if (dsPic == NULL)
	{
		printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
		return(-2);
	}

	return(0);
}

int main(int argc, char *argv[])
{	
	int sortieBoucle = 0,
		vitessePulse = 200,
		vitesseMMS = vitessePulse * RATIO,
		roueGauche, 
		roueDroite,
		distanceRoueG = 0,
		distanceRoueD = 0;
	
	char toucheEnfoncee,
	     prec = 'a';
	
	if (init() != 0)
		return(1);

	while (1)
	{ 
		/* distance += kh4_get_position(*roueGauche, *roueDroite, *dsPic); */
		
		usleep(200000);
		kb_clrscr();
		
		printf("Test de l'odomètre :\n");
		printf("Bougez manuellement le robot.\n");
		printf("\n\nDistance parcourue :\n");
		printf("Roue gauche : %d\n", distanceRoueG);
		printf("Roue droite : %d\n", distanceRoueD);
		printf("\n\nAppuyez sur X pour quitter le programme.\n");
		
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
					kh4_get_position(*roueGauche, *roueDroite, *dsPic);
					distanceRoueG += roueGauche;
					distanceRoueD += roueDroite;
					prec = 'z';
					break;

				case 's':
					kh4_set_speed(-vitessePulse, -vitessePulse, dsPic);
					kh4_get_position(*roueGauche, *roueDroite, *dsPic);
					distanceRoueG += roueGauche;
					distanceRoueD += roueDroite;
					prec = 's';
					break;

				case 'd':
					kh4_set_speed(vitessePulse, -vitessePulse, dsPic);
					kh4_get_position(*roueGauche, *roueDroite, *dsPic);
					distanceRoueG += roueGauche;
					distanceRoueD += roueDroite;
					prec = 'd';
					break;

				case 'q':
					kh4_set_speed(-vitessePulse, vitessePulse, dsPic);
					kh4_get_position(*roueGauche, *roueDroite, *dsPic);
					distanceRoueG += roueGauche;
					distanceRoueD += roueDroite;
					prec = 'q';
					break;

				case 'a':
					kh4_set_speed(0, 0, dsPic);
					prec = 'a';
					break;

				case 'x':
					sortieBoucle = 1;
					break;

				default:
					break;
				}
			}
		}
	}
	
	return(0);
}
