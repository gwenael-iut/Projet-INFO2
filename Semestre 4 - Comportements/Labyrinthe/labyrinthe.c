/* ----------------------------------------------- */
/* -------      PROGRAMME DE PARCOURS       ------ */
/* -------            LABYRINTHE            ------ */
/* -------        ROUES & INFRAROUGES       ------ */
/* ----------------------------------------------- */
/* ------- Fichier : labyrinthe.c           ------ */
/* -------                                  ------ */
/* ------- Auteur  : NAYET Morgan           ------ */
/* ------- Auteur  : BASCOUR Gwenaël        ------ */
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
        return -1;
    }

    /* Initialisation de la connexion au robot */
    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL );
    if ( dsPic == NULL) {
        printf("ERREUR: Echec de la communication avec le robot (Kh4 dsPic)\n\n");
        return -1;
    }

    kh4_set_speed(0, 0, dsPic); // Arrêt du robot

    /* Mise du mode des contrôleurs des moteurs des roues sur 3 pour les contrôler grâce au nombre de pulsation de l'odomètre */
    if (kh4_SetMode(kh4RegIdle, dsPic) < 0){
        printf("ERREUR: Echec de la position du controle des roues sur les pulsation de l'odometre\n");
        return -1;
    }

    return 0;
}

/**
 * Fait pivoter le robot vers sa droite d'un certain degré
 */
int pivoter(double degre)
{
    int roueDroite, roueGauche;
    int maxsp, accinc, accdiv, minspacc, minspdec; // SetSpeedProfile
    double pulsationCible;
    double espacement_roues_2; // Rayon d'un cercle ayant pour centre l'axe central du robot (soit la moitié de la distance entre les deux roues)

    if (raz() != 0)
    {
        perror("raz -> ");
        return -1;
    }

    accinc=3;
    accdiv=0;
    minspacc=20;
    minspdec=1;
    maxsp=400;

    if(kh4_SetSpeedProfile(accinc, accdiv, minspacc, minspdec, maxsp, dsPic ) < 0)
    {
       perror("(kh4_SetSpeedProfile -> ");
       return -1;
    }

    /* Affectation de la position des 2 roues */
    if (kh4_get_position(&roueGauche, &roueDroite, dsPic) < 0)
    {
        perror("kh4_get_position -> ");
        return -1;
    }

    /* Rotation des roues par une rotation du rapport entre le degré demandé
     * et le rayon du robot
     * (le périmètre du robot étant de 2.PI.r alors 360°=2.PI.r
     * donc si l'on désire pivoter de degré,
     * alors il faut faire le rapport sur 360°)
     */
    espacement_roues_2 = 52,7;
    pulsationCible = (degre/360.0)*(2.0*M_PI*espacement_roues_2)/KH4_PULSE_TO_MM;

    if( kh4_SetMode( kh4RegPosition,dsPic ) < 0)
    {
        perror("kh4_SetMode -> ");
        return -1;
    }

    if ( kh4_set_position(roueGauche+(long)pulsationCible,
			  roueDroite-(long)pulsationCible,
			  dsPic) < 0)
    {
        perror("kh4_set_position -> ");
	    return -1;
    }
    return 0;
}

/**
 * Renvoie le capteur us qui détecte en premier un obstacle
 * @param rayonIr valeur à partir de laquelle on détecte un obstacle
 * @return 0 si le capteur ne détecte aucun obstacle à droite
 * @return 1 si le capteur droit détecte un obstacle et que le capteur avant n'en détecte pas
 * @return 2 si le capteur droit et avant détectent un obstacle mais pas le capteur gauche
 * @return 3 si le capteur droit, avant et gauche détectent un obstacle
 */
int detect(int rayonIr)
{
    char Buffer[100];
    int right_ir;
    int front_ir;
    int left_ir;


    if(rayonIr > 1024 || rayonIr < 0)
    {
        printf("detect -> valeur de détection non valide (comprise entre 0 et 1024)");
        return -1;
    }

    /* Récupère les valeurs des capteurs et les stocke dans le tampon  */
    kh4_proximity_ir(Buffer, dsPic);

    right_ir = (Buffer[5*2] | Buffer[5*2+1]<<8);
    front_ir = (Buffer[3*2] | Buffer[3*2+1]<<8);
    left_ir = (Buffer[1*2] | Buffer[1*2+1]<<8);

    if(right_ir < rayonIr)
    {
    	return 0;
    }

    if(front_ir < rayonIr) // && right_ir >= rayonIr
    {
        return 1;
    }

    if(left_ir < rayonIr) // && right_ir >= rayonIr && front_ir >= rayonIr
    {
        return 2;
    }

    return 3; // Tous les capteurs détectent un obstacle

}

int parcours(void)
{
	int capteur_ir_comportement;
    int degre_a_pivoter;

    if (raz() != 0)
    {
    	perror("raz -> ");
        return -1;
   	}

    if(kh4_SetMode( kh4RegSpeed,dsPic ) < 0)
    {
        perror("kh4_SetMode -> ");
        return -1;
    }
    kh4_set_speed(200, 200, dsPic);

    while(1) {

	    while( (capteur_ir_comportement = detect(900) ) == 1); // Méthode bloquante, se débloque quand un obstacle est détecté

        if(capteur_ir_comportement == 0)
        {
            degre_a_pivoter = 90;
        }else if(capteur_ir_comportement == 2)
        {
            degre_a_pivoter = -90;
        }else if(capteur_ir_comportement == 3)
        {
           degre_a_pivoter = 180;
        }else
        {
            perror("parcour -> ");
            return -1;
        }

        kh4_set_speed(0, 0, dsPic);
	    // Le robot tourne d'un certain degré sur l'axe d'une roue
	    if(pivoter(degre_a_pivoter) != 0)
	    {
	        perror("\npivoter -> ");
	        return -1;
	    }

	    usleep(3000000);

	    if(kh4_SetMode( kh4RegSpeed,dsPic ) < 0)
	    {
	        perror("kh4_SetMode -> ");
	    	return -1;
	    }
	    kh4_set_speed(200, 200, dsPic);
	}
	return 0;
}


/**
 * Lance la méthode parcours afin de parcourir une pièce en autonomie
 */
int main(void)
{
    if( parcours() != 0 )
    {
    	perror("parcours -> ");
    	return -1;
    }

    return 0;
}
