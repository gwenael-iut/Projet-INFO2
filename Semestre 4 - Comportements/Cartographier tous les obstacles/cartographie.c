/* ----------------------------------- */
/* --- CARTOGRAPHIER LES OBSTACLES --- */
/* ----------------------------------- */
/* ----- Fichier : cartographie.c ---- */
/* -----						------ */
/* ----- Auteur : Lucien PARES ------- */
/* ----------------------------------- */

#include <khepera/khepera.h>
#include <math.h>

#define TOUR_DEGRE 45.0
#define TOUR_COMPLET 360.0
#define TOUR_INIT 0.0
#define ROUES_ESPACEMENT 52.7

static knet_dev_t *dsPic; // micro-contrôleur du robot
int roueGauche, roueDroite;

int raz() // Remise à zéro du robot
{
	/* Initialisation de la librairie Khepera */
	if (kb_init(0, NULL) < 0)
	{
		printf("ERREUR: Echec de l'initialisation de la librairie !\n\n");
		return 1;
	}

	/* Initialisation de la connexion au robot */
	dsPic = knet_open("Khepera:dsPic", KNET_BUS_I2C, 0, NULL);
	if (dsPic == NULL)
	{
		printf("ERREUR: Echec de la communication avec le robot (Kh4 dsPic)\n\n");
		return 1;
	}

	/* Arrêt du robot */
	kh4_set_speed(0, 0, dsPic);

	return 0;
}

int init() // Initialisation du robot
{
	/* Connexion aux roues du robot */
	if (kh4_SetMode(kh4RegIdle, dsPic) < 0)
	{
		printf("ERREUR: Echec de la position du controle des roues sur les pulsations de l'odometre\n");
		return 1;
	}

	/* Affectation de la position des 2 roues */
	if (kh4_get_position(&roueGauche, &roueDroite, dsPic) < 0)
	{
		perror("kh4_get_position -> ");
		return 1;
	}

	return 0;
}

int rotation() // Rotation du robot sur son axe de TOUR_DEGRE °
{
	double pulsation;

	pulsation = (TOUR_DEGRE / TOUR_COMPLET) * (2.0 * M_PI * ROUES_ESPACEMENT) / KH4_PULSE_TO_MM;

	/* Rotation envoyée aux roues */
	if (kh4_set_position(roueGauche + (long)pulsation,
			     roueDroite - (long)pulsation,
			     dsPic) < 0)
	{
		perror("kh4_set_position -> ");
		return 1;
	}

	return 0;
}

int cartographie() // Cartographie l'environnement à l'aide de la rotation
{
	char buffer[100];
	int distance; // distance en mm entre le robot et l'obstacle

	roueGauche = roueDroite = TOUR_INIT;

	/* Mesure de la distance entre le robot et un obstacle détecté  */
	while (roueGauche < TOUR_INIT && roueDroite < TOUR_INIT)
	{
		if (rotation() > 0)
		{
			perror("rotation -> ");
			return 1;
		}

		kh4_mesure_us(buffer, dsPic);
		distance = (buffer[4] | buffer[5]<<8);
		printf("Obstacle à %d mm\n", &distance);
	}

	return 0;
}

int main(void)
{
	if (raz() > 0)
	{
		perror("raz -> ");
		return 1;
	}

	if (init() > 0)
	{
		perror("init -> ");
		return 1;
	}

	if (cartographie() > 0)
	{
		perror("cartographie -> ");
		return 1;
	}

	return 0;
}
