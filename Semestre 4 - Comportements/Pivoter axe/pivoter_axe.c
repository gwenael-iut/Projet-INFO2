/* ----------------------------------------------- */
/* -------      PROGRAMME DE ROTATION      ------- */
/* -------            AXE ROUE             ------- */
/* -------        ROUES & ODOMETRE         ------- */
/* ----------------------------------------------- */
/* ------- Fichier : pivoter_roue.c          ------ */
/* -------                                  ------ */
/* ------- Auteur  : NAYET Morgan           ------ */
/* ----------------------------------------------- */

#include <khepera/khepera.h>
#include <math.h>

static knet_dev_t *dsPic; // Accès au microcontrôleur Pic du robot

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
    if (kh4_SetMode(kh4RegIdle, dsPic) < 0){
        printf("ERREUR: Echec de la position du controle des roues sur les pulsation de l'odometre\n");
        return 1;
    }

    return 0;
}

/**
 * Fais pivoter le robot vers sa droite d'un certain degré
 */
int pivoter(double degre)
{
    int roueDroite, roueGauche;
    int maxsp,accinc,accdiv,minspacc, minspdec; // SetSpeedProfile
    double pulsationCible;
    double espacement_roues_2; //rayon d'un cercle ayant pour centre l'axe central du robot (soit la moitié de la distance entre les deux roues

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

    if(kh4_SetSpeedProfile(accinc,accdiv,minspacc, minspdec,maxsp,dsPic ) < 0)
    {
       perror("(kh4_SetSpeedProfile -> ");
       return 1;
    }

    /* Affectation de la position des 2 roues */
    if (kh4_get_position(&roueGauche, &roueDroite, dsPic) < 0)
    {
        perror("kh4_get_position -> ");
        return 1;
    }

    /* Rotation des roues par une rotation du rapport entre le degre demandé
     * et le rayon du robot
     * (le périmètre du robot étant de 2.PI.r alors 360°=2.PI.r
     * donc si l'on désire pivoter de degre,
     * alors il faut faire le rapport sur 360°)
     */
    espacement_roues_2 = 52,7;
    pulsationCible = (degre/360.0)*(2.0*M_PI*espacement_roues_2)/KH4_PULSE_TO_MM; // vérifier que quand on fait int*double -> renvoie double
    
    if( kh4_SetMode( kh4RegPosition,dsPic ) < 0)
    {
        perror("kh4_SetMode -> ");
        return 1;
    }

    if ( kh4_set_position(roueGauche+(long)pulsationCible,
						  roueDroite-(long)pulsationCible,
					      dsPic) < 0)
    {
	    perror("kh4_set_position -> ");
		return 1;
	}


    return 0;
}

/**
 * L'utilisateur entre le degrer de rotation que le robot effectue
 */
int main(void)
{
    double degre;

    printf("\nEntrez un degre: "); scanf("%lf", &degre);

    /* le robot tourne d'un certain degrer sur l'axe d'une roue */
    if(pivoter(degre) != 0)
    {
        perror("\npivoter -> ");
        return 1;
    }

    return 0;
}
