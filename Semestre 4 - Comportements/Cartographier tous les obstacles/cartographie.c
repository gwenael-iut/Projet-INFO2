/* ----------------------------------- */
/* --- CARTOGRAPHIER LES OBSTACLES --- */
/* ----------------------------------- */
/* ----- Fichier : cartographie.c ---- */
/* -----						------ */
/* ----- Auteur : Lucien PARES ------- */
/* ----------------------------------- */

#include <khepera/khepera.h>
#include <math.h>

#define TOUR_COMPLET 360.0
#define TOUR_DEGRE 45.0
#define TOUR_INITIAL 0
#define ROUES_ESPACEMENT 52.7;

/* Accès au micro-contrôleur du robot */
static knet_dev_t *dsPic;

int roueGauche, roueDroite,
	distance_mm, distance_cm;
char buffer[100];
double pulsationCible;

int raz()
{
    /* Initialisation de la librairie Khepera */
    if (kb_init(0 , NULL) < 0) 
    {
        printf("ERREUR: Echec de l'initialisation de la librairie !\n\n");
        return 1;
    }
    
    /* Initialisation de la connexion au robot */
    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL );
    if (dsPic == NULL) 
    {
        printf("ERREUR: Echec de la communication avec le robot (Kh4 dsPic)\n\n");
        return 1;
    }
    
    /* Arrêt du robot */
    kh4_set_speed(0, 0, dsPic); 
    
    return 0;
}

int rotation()
{	
	/* Connexion aux roues du robot */
    if (kh4_SetMode(kh4RegIdle, dsPic) < 0)
    {
        printf("ERREUR: Echec de la position du controle des roues sur les pulsation de l'odometre\n");
        return 1;
    }
    
    /* Affectation de la position des 2 roues */
    if (kh4_get_position(&roueGauche, &roueDroite, dsPic) < 0)
    {
        perror("kh4_get_position -> ");
        return 1;
    }
    
    /* Rotation sur l'axe via un degré défini */
	pulsationCible = (TOUR_DEGRE / TOUR_COMPLET) * (2.0 * M_PI * ROUES_ESPACEMENT) / KH4_PULSE_TO_MM;
	
	if (kh4_set_position(roueGauche + (long)pulsationCible,
						 roueDroite - (long)pulsationCible,
						 dsPic) < 0)
	{
		perror("kh4_set_position -> ");
		return 1;
	}

	/* Mesure de la distance de l'obstacle détecté */
	kh4_measure_us(buffer, dsPic);
	distance_mm = (buffer[4] | buffer[5]<<8); /* distance en mm */
	distance_cm = distance_mm * 10; /* distance en cm */
	
	return 0;
}

int cartographie()
{
	if (rotation() > 0)
	{
		perror("rotation -> ");
		return 1;
	}
	
	while (roueGauche == roueDroite == TOUR_INITIAL)
	{
		if (rotation() > 0)
		{
			perror("rotation -> ");
			return 1;
		}
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

	if (cartographie() > 0)
	{
		perror("cartographie -> ");
		return 1;
	}

	return 0;
}
