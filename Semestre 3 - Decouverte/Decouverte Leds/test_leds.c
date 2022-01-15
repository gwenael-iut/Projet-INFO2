/* ----------------------------------------------- */
/* -------        PROGRAMME DE TEST        ------- */
/* -------            DES LEDS             ------- */
/* ----------------------------------------------- */
/* ------- Fichier : test_leds.c           ------- */
/* ------- Auteur  : DA COSTA Yacine       ------- */
/* ----------------------------------------------- */
#include <khepera/khepera.h>

static knet_dev_t *dsPic; // Accès au microcontrôleur Pic du robot

/*
 * Méthode permettant l'initialisation du robot
 * Librairie Khepera & connexion au robot
 */
int init()
{
    /* Initialisation de la librairie khepera */
    if ((kb_init(0 , NULL)) < 0 ) {
        printf("\nERREUR: Echec de l'initialisation de la librairie !\n\n");
        return 1;
    }

    /* Initialisation de la connexion au robot */
    dsPic  = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL );
    if (dsPic == NULL) {
        printf("\nERREUR: Echec de la communication avec le robot (Kh4 dsPic)\n\n");
        return 1;
    }

    return 0;
}

/*
 * Méthode permettant de remettre le robot dans
 * son état intial.
 * Éteindre toutes les leds.
 */
void end()
{
    tcflush(0, 0);
    fflush(stdin);
    kh4_SetRGBLeds(0, 0, 0, 0, 0, 0, 0, 0, 0, dsPic);
}

/*
 * Fonction permettant le test des 3 leds RGB du robot
 * Demande des valeurs RGB pour les trois leds
 * Éclairage des leds
 */
int main()
{
    char leds[9];
    char prompt[80];

    /* Initialisation du robot */
    init();

    printf("------------------- \
           \nTEST DES LEDS RGB \
           \n-------------------\n\n");;

    /* Demande des valeurs pour le réglage des leds RGB */
    printf("Entrez les valeurs RGB des trois leds a la suite \n(Ex : 120 0 0 0 122 0 0 0 120) \n\nSaisie : ");
    tcflush(0, 0);
    fflush(stdin);
    fgets(prompt, 80, stdin);
    
    /* Éclairage des leds en fonction des valeurs saisies */
    if (EOF == sscanf(prompt, "%d %d %d %d %d %d %d %d %d",  &leds[0], &leds[1], &leds[2], &leds[3], &leds[4], &leds[5], &leds[6], &leds[7], &leds[8]))
        printf("\nErreur : Syntaxe : R1 G1 B1 R2 G2 B2 R3 G3 B3\n");
    else
    {
        kh4_SetRGBLeds(leds[0], leds[1], leds[2], leds[3], leds[4], leds[5], leds[6], leds[7], leds[8], dsPic);
        printf("\nRGB : \n\t - LED 1 : %d %d %d \n\t - LED 2 : %d %d %d \n\t - LED 3 : %d %d %d\n",
               leds[0], leds[1], leds[2], leds[3], leds[4], leds[5], leds[6], leds[7], leds[8]);
        usleep(5000000);
    }

    /* Remettre le robot dans son état initial */
    end();

    return 0;
}
