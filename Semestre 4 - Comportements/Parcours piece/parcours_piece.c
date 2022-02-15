/* ----------------------------------------------- */
/* -------      PROGRAMME DE PARCOURS       ------ */
/* -------            D'UNE PIECE           ------ */
/* -------        ROUES & ODOMETRE          ------ */
/* ----------------------------------------------- */
/* ------- Fichier : parcours_piece.c       ------ */
/* -------                                  ------ */
/* ------- Auteur  : NAYET Morgan           ------ */
/* ------- Auteur  : CAUSSE Jade            ------ */
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

    kh4_set_speed(0, 0, dsPic); // Arret du robot

    /* Mise du mode des contrôleurs des moteurs des roues sur 3 pour les contrôler grâce au nombre de pulsation de l'odomètre */
    if (kh4_SetMode(kh4RegIdle, dsPic) < 0){
        printf("ERREUR: Echec de la position du controle des roues sur les pulsation de l'odometre\n");
        return -1;
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
        return -1;
    }

    accinc=3;
    accdiv=0;
    minspacc=20;
    minspdec=1;
    maxsp=400;

    if(kh4_SetSpeedProfile(accinc,accdiv,minspacc, minspdec,maxsp,dsPic ) < 0)
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

    /* Rotation des roues par une rotation du rapport entre le degre demandé
     * et le rayon du robot
     * (le périmètre du robot étant de 2.PI.r alors 360°=2.PI.r
     * donc si l'on désire pivoter de degre,
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
 * renvoie le capteur us qui détecte en premier un obstacle
 * @param distanceDetect la distance minimale en mm à laquelle le robot détecte un obstacle
 * @return 0 si le capteur ne détecte aucun obstacle
 * @return 1 si le capteur avant-gauche détecte un obstacle
 * @return 2 si le capteur avant détecte un obstacle
 * @return 3 si le capteur avant-droit détecte un obstacle
 */
int detect(int distanceDetect)
{
    char Buffer[100];
    int front_left_us;
    int front_us;
    int front_right_us;

    if(distanceDetect <= 250)
    {
        printf("detect -> La distance minimale de detection des ultrasons commence a partir de 25cm");
        return -1;
    }


    /* Récupère les valeurs des capteurs et les stocke dans le tampon  */
    kh4_measure_us(Buffer, dsPic);

    front_left_us = (Buffer[1*2] | Buffer[1*2+1]<<8);
    front_us = (Buffer[2*2] | Buffer[2*2+1]<<8);
    front_right_us = (Buffer[3*2] | Buffer[3*2+1]<<8);

    if(front_left_us * 10 <= distanceDetect)
    {
        return 1;
    }else if(front_us * 10 <= distanceDetect)
    {
        return 2;
    }else if(front_right_us * 10 <= distanceDetect)
    {
        return 3;
    }

    return 0;
}

int parcour(void)
{
	int capteur_us;
        int degre_a_pivoter;

        if (raz() != 0)
    	    {
    	        perror("raz -> ");
    	        return -1;
    	    }

        if( kh4_SetMode( kh4RegSpeed,dsPic ) < 0)
        	    	    {
        	    	        perror("kh4_SetMode -> ");
        	    	        return -1;
        	    	    }
        kh4_set_speed(200, 200, dsPic);


        while(1) {

            //TODO automatiser l'entier dans detect genre l'utilisateur param la distance
	        while((capteur_us = detect(300)) == 0); //methode bloquante , se débloque quand un obstacle est détecté

            if(capteur_us == 1)
            {
                degre_a_pivoter = 90;
            }else if(capteur_us == 2)
            {
                if(rand() %2 )
                {
                    degre_a_pivoter = -135;
                }else
                {
                    degre_a_pivoter = 135;
                }
            }else if(capteur_us == 3)
            {
               degre_a_pivoter = -90;
            }else
            {
                perror("parcour -> ");
                return -1;
            }

        kh4_set_speed(0, 0, dsPic);
	    /* le robot tourne d'un certain degré sur l'axe d'une roue */
	    if(pivoter(degre_a_pivoter) != 0)
	    {
	        perror("\npivoter -> ");
	        return -1;
	    }

	    usleep(3000000);

	    if( kh4_SetMode( kh4RegSpeed,dsPic ) < 0)
	    	    {
	    	        perror("kh4_SetMode -> ");
	    	        return -1;
	    	    }
	    kh4_set_speed(200, 200, dsPic);
	}
	return 0;
}

/**
 * 
 */
int main(void)
{


    if( parcour() != 0 )
    {
    	perror("parcour -> ");
    	return -1;
    }

    return 0;
}
