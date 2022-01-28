/* ----------------------------------------------- */
/* -------      PROGRAMME DE PARCOURS      ------- */
/* -------      ROUES & ACCELEROMETRE      ------- */
/* ----------------------------------------------- */
/* ------- Fichier : parcours_dist_mm.c     ------ */
/* -------                                  ------ */
/* ------- Auteur  : NAYET Morgan           ------ */
/* ----------------------------------------------- */

#include <khepera/khepera.h>

static knet_dev_t *dsPic; // Accès au microcontrôleur Pic du robot

#define PI 3


/**
 * Méthode permettant l'initialisation du robot et/ou sa remise à zéro
 * Librairie Khepera & connexion au robot
 */
int raz()
{
    /* Initialisation de la librairie khepera */
    if ((kb_init(0 , NULL)) < 0 ) {
        printf("ERREUR: Echec de l'initialisation de la librairie !\n\n");
        return 1;
    }

    /* Initialisation de la connexion au robot */
    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL );
    if ( dsPic == NULL) {
        printf("ERREUR: Echec de la communication avec le robot (Kh4 dsPic)\n\n");
        return 1;
    }

    kh4_set_speed(0, 0, dsPic); // Arret du robot

    /* Mise du mode des contrôleurs des moteurs des roues sur 3 pour les contrôler grâce au nombre de pulsation de l'odomètre */
    if (kh4_SetMode(kh4RegPosition, dsPic) < 0){
        printf("ERREUR: Echec de l'arret des controleurs des moteurs");
        return 1;
    }

    return 0;
}

/**
 * Déplace le robot vers l'avant d'une certaine distance en millimètres
 */
int parcourir(int distance)
{
    int roueDroite,
        roueGauche;
    int pmarg,maxsp,accinc,accdiv,minspacc, minspdec; // SetSpeedProfile
    double pulsationCible;

    if (raz() != 0)
    {
        perror("raz -> ");
        return 1;
    }
    
    accinc=3;
    accdiv=0;
    minspacc=20;
    minspdec=1;
    maxsp=400;
    kh4_SetSpeedProfile(accinc,accdiv,minspacc, minspdec,maxsp,dsPic );

    /* Affectation de la position des 2 roues */
    if (kh4_get_position(&roueGauche, &roueDroite, dsPic) < 0)
    {
        perror("kh4_get_position -> ");
        return 1;
    }
    
    pulsationCible = distance/KH4_PULSE_TO_MM;
    
    kh4_SetMode( kh4RegPosition,dsPic );
    if ( kh4_set_position(roueGauche+(long)pulsationCible,
                          roueDroite+(long)pulsationCible,
    		              dsPic) < 0){
    	perror("kh4_set_position -> ");
    	return 1;
    }
    
    return 0;
}

/**
 * L'utilisateur peut entrer la distance qu'il souhaite parcourir pour le robot
 * une fois le programme appelé.
 */
int main(void)
{
    int distance;

    printf("\nEntrez une distance en millimetres: "); scanf("%d", &distance);

    /* le robot parcours une certaine distance en mm en avançant droit devant lui */
    if(parcourir(distance) != 0)
    {
        perror("\nparcourir -> ");
        return 1;
    }

    return 0;
}
