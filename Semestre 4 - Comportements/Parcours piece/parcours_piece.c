/* ----------------------------------------------- */
/* -------      PROGRAMME DE PARCOURS       ------ */
/* -------            D'UNE PIECE           ------ */
/* -------        ROUES & ODOMETRE          ------ */
/* ----------------------------------------------- */
/* ------- Fichier : parcours_piece.c       ------ */
/* -------                                  ------ */
/* ------- Auteur  : NAYET Morgan           ------ */
/* ------- Auteur  : CAUSSE Jade            ------ */
/* ------- Auteur  : DA COSTA Yacine        ------ */
/* ----------------------------------------------- */

#include <khepera/khepera.h>
#include <math.h>

static knet_dev_t *dsPic; // Accès au microcontrôleur Pic du robot

/**
 * Méthode permettant l'initialisation du robot et/ou sa remise à zéro
 * Librairie Khepera & connexion au robot
 */
int raz() {
    /* Initialisation de la librairie khepera */
    if ((kb_init(0, NULL)) < 0) {
        printf("ERREUR: Echec de l'initialisation de la librairie !\n\n");
        return -1;
    }

    /* Initialisation de la connexion au robot */
    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
    if (dsPic == NULL) {
        printf("ERREUR: Echec de la communication avec le robot (Kh4 dsPic)\n\n");
        return -1;
    }

    kh4_set_speed(0, 0, dsPic); // Arrêt du robot

    /* Mise du mode des contrôleurs des moteurs des roues sur 3 pour les contrôler grâce au nombre de pulsation de l'odomètre */
    if (kh4_SetMode(kh4RegIdle, dsPic) < 0) {
        printf("ERREUR: Echec de la position du controle des roues sur les pulsation de l'odometre\n");
        return -1;
    }

    return 0;
}

/**
 * Fait pivoter le robot vers sa droite d'un certain degré
 */
int pivoter(double degre) {
    int roueDroite, roueGauche;
    int maxsp, accinc, accdiv, minspacc, minspdec; // SetSpeedProfile
    double pulsationCible;
    double espacement_roues_2; // Rayon d'un cercle ayant pour centre l'axe central du robot (soit la moitié de la distance entre les deux roues)

    if (raz() != 0) {
        perror("raz -> ");
        return -1;
    }

    accinc = 3;
    accdiv = 0;
    minspacc = 20;
    minspdec = 1;
    maxsp = 400;

    if (kh4_SetSpeedProfile(accinc, accdiv, minspacc, minspdec, maxsp, dsPic) < 0) {
        perror("(kh4_SetSpeedProfile -> ");
        return -1;
    }

    /* Affectation de la position des 2 roues */
    if (kh4_get_position(&roueGauche, &roueDroite, dsPic) < 0) {
        perror("kh4_get_position -> ");
        return -1;
    }

    /* Rotation des roues par une rotation du rapport entre le degré demandé
     * et le rayon du robot
     * (le périmètre du robot étant de 2.PI.r alors 360°=2.PI.r
     * donc si l'on désire pivoter de degré,
     * alors il faut faire le rapport sur 360°)
     */
    espacement_roues_2 = 52, 7;
    pulsationCible = (degre / 360.0) * (2.0 * M_PI * espacement_roues_2) / KH4_PULSE_TO_MM;

    if (kh4_SetMode(kh4RegPosition, dsPic) < 0) {
        perror("kh4_SetMode -> ");
        return -1;
    }

    if (kh4_set_position(roueGauche + (long) pulsationCible,
                         roueDroite - (long) pulsationCible,
                         dsPic) < 0) {
        perror("kh4_set_position -> ");
        return -1;
    }
    return 0;
}

int isOverflown(int us, int us1, int us2) {
    if(us == 1000 || us1 == 1000 || us2 == 1000)
        return 1;
    return 0;
}

/**
 * Renvoie le capteur qui détecte en premier un obstacle
 * @param distanceDetect la distance minimale en mm à laquelle le robot détecte un obstacle
 * @return 0 si le capteur ne détecte aucun obstacle
 * @return 1 si le capteur avant-gauche détecte un obstacle
 * @return 2 si le capteur avant détecte un obstacle
 * @return 3 si le capteur avant-droit détecte un obstacle
 */
int detect() {
    int overflow;
    char BufferUS[100];
    int front_left,
            front,
            front_right;

    kh4_activate_us(14, dsPic);
    kh4_measure_us(BufferUS, dsPic);

    front_left = (BufferUS[1 * 2] | BufferUS[1 * 2 + 1] << 8);
    front = (BufferUS[2 * 2] | BufferUS[2 * 2 + 1] << 8);
    front_right = (BufferUS[3 * 2] | BufferUS[3 * 2 + 1] << 8);

    overflow = isOverflown(front_left, front, front_right);

    if(overflow) {
        char BufferIR[100];
        int bufferValueL,
                bufferValueF,
                bufferValueR;

        kh4_proximity_ir(BufferIR, dsPic);

        bufferValueL = (BufferIR[1*2] | BufferIR[1*2+1] << 8);
        bufferValueF = (BufferIR[2*2] | BufferIR[2*2+1] << 8);
        bufferValueR = (BufferIR[3*2] | BufferIR[3*2+1] << 8);

        if(bufferValueL <= 950)
            return 1;
        if(bufferValueF <= 950)
            return 2;
        if(bufferValueR <= 950)
            return 3;
    }

    return 0;
}

int parcours(void) {
    int capteur_us;
    int degre_a_pivoter;

    if (raz() != 0) {
        perror("raz -> ");
        return -1;
    }

    if (kh4_SetMode(kh4RegSpeed, dsPic) < 0) {
        perror("kh4_SetMode -> ");
        return -1;
    }
    kh4_set_speed(200, 200, dsPic);

    while (1) {

        //TODO automatiser l'entier dans detect genre l'utilisateur param la distance
        while ((capteur_us = detect()) == 0); // Méthode bloquante, se débloque quand un obstacle est détecté

        if (capteur_us == 1) {
            degre_a_pivoter = 90;
        } else if (capteur_us == 2) {
            if (rand() % 2) {
                degre_a_pivoter = -135;
            } else {
                degre_a_pivoter = 135;
            }
        } else if (capteur_us == 3) {
            degre_a_pivoter = -90;
        } else {
            perror("parcour -> ");
            return -1;
        }

        kh4_set_speed(0, 0, dsPic);
        /* Le robot tourne d'un certain degré sur l'axe d'une roue */
        if (pivoter(degre_a_pivoter) != 0) {
            perror("\npivoter -> ");
            return -1;
        }

        usleep(3000000);

        if (kh4_SetMode(kh4RegSpeed, dsPic) < 0) {
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
int main(void) {

    if (parcours() != 0) {
        perror("parcours -> ");
        return -1;
    }

    return 0;
}
