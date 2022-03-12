/* ----------------------------------- */
/* --- CARTOGRAPHIER LES OBSTACLES --- */
/* ----------------------------------- */
/* ----- Fichier : cartographie.c ---- */
/* -----                        ------ */
/* ----- Auteur : Lucien PARES ------- */
/* ----------------------------------- */

#include <khepera/khepera.h>
#include <math.h>

#define TOUR_DEGRE 180.0
#define TOUR_COMPLET 360.0
#define TOUR_INIT 0.0
#define ROUES_ESPACEMENT 52.7

static knet_dev_t *dsPic; // micro-contrôleur du robot
int roueGauche, roueDroite;

double rotation = 0;


/* Remise à zéro du robot */
int raz()
{
    /* Initialisation de la librairie Khepera */
    if (kb_init(0, NULL) < 0)
    {
        printf("ERREUR: Echec de l'initialisation de la librairie !\n\n");
        return 1;
    }

    /* Initialisation de la connexion au robot */
    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
    if (dsPic == NULL)
    {
        printf("ERREUR: Echec de la communication avec le robot (Kh4 dsPic)\n\n");
        return 1;
    }

    /* Arrêt du robot */
    kh4_set_speed(0, 0, dsPic);

    /* Connexion aux roues du robot */
    if (kh4_SetMode(kh4RegIdle, dsPic) < 0)
    {
        printf("ERREUR: Echec de la position du controle des roues sur les pulsations de l'odometre\n");
        return 1;
    }

    return 0;
}

/* Rotation du robot sur son axe de TOUR_DEGRE ° */
int pivoter()
{
    double pulsation;

    // SetSpeedProfile
    int maxsp = 400, 
        accinc = 3, 
        accdiv = 0, 
        minspacc = 20, 
        minspdec = 1; 

    if (raz() != 0)
    {
        perror("raz -> ");
        return -1;
    }

    if (kh4_SetSpeedProfile(accinc, accdiv, minspacc, minspdec, maxsp, dsPic ) < 0)
    {
        perror("(kh4_SetSpeedProfile -> ");
        return -1;
    }

    /* Affectation de la position des 2 roues */
    if (kh4_get_position(&roueGauche, &roueDroite, dsPic) < 0)
    {
        perror("kh4_get_position -> ");
        return 1;
    }

    pulsation = (TOUR_DEGRE / TOUR_COMPLET) * (2.0 * M_PI * ROUES_ESPACEMENT) / KH4_PULSE_TO_MM;

    if (kh4_SetMode(kh4RegPosition, dsPic) < 0)
    {
        perror("kh4_SetMode -> ");
        return -1;
    }

    if (kh4_set_position(roueGauche + (long)pulsation,
                         roueDroite - (long)pulsation,
                         dsPic) < 0)
    {
        perror("kh4_set_position -> ");
        return -1;
    }

    return 0;
}



/* Cartographie l'environnement à l'aide de la rotation */
int cartographie()
{
    char buffer[100], buffer2[100];
    int i;
    short distance_values[10];

    roueGauche = roueDroite = TOUR_INIT;
    kh4_activate_us(31, dsPic);

    /* Mesure de la distance entre le robot et un obstacle détecté  */
    while (rotation != TOUR_COMPLET)
    {
        kh4_measure_us(buffer, dsPic);
        for (i = 0; i < 5; i++) {
            distance_values[i] = (short)(buffer[i*2] | buffer[i*2+1]<<8);                       
        }
        usleep(3000000);

        if (pivoter() > 0)
        {
            perror("pivoter -> ");
            return 1;
        }
        rotation += TOUR_DEGRE;
        usleep(5000000);

        kh4_measure_us(buffer2, dsPic);
        for (i = 0; i < 5; i++) {
            distance_values[4+i] = (short)(buffer2[i*2] | buffer2[i*2+1]<<8);
        }
        usleep(3000000);

        if (pivoter() > 0)
        {
            perror("pivoter -> ");
            return 1;
        }
        rotation += TOUR_DEGRE;
        usleep(2000000);
    }

    /* Affichage des distances retournées par les capteurs US */
    printf("\n- Distance >= 1000 si objet a moins de 25cm, a plus de 250cm ou aucune detection\
            \n- Marge d'erreur : +/- 2cm\n\n");
    printf("\nAffichage des distances (cm)\
           \n\nGauche        : %4d cm\
           \nAvant-Gauche  : %4d cm\
           \nAvant         : %4d cm\
           \nAvant-Droit   : %4d cm\
           \nDroit         : %4d cm\
           \nArriere-Droit : %4d cm\
           \nArriere       : %4d cm\
           \nArrire-Gauche : %4d cm\n",
           distance_values[0], distance_values[1], distance_values[2], distance_values[3],
           distance_values[4], distance_values[6], distance_values[7], distance_values[8]);

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
