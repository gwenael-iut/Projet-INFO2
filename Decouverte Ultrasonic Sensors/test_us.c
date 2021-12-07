/* ------------------------------------------------ */
/* -------          PROGRAMME TEST          ------- */
/* -------       CAPTEURS À ULTRASONS       ------- */
/* ------------------------------------------------ */
/* ------- Fichier : test_us.c              ------- */
/* ------- Auteur  : BASCOUR Gwenaël        ------- */
/* ------------------------------------------------ */
#include <khepera/khepera.h>

static knet_dev_t * dsPic; // Accès au microcontrôleur Pic du robot

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
 * Fonction permettant le test des capteurs à ultrasons du robot.
 * Demande des capteurs à activer et affichage sur la sortie
 * standard des valeurs retournées.
 */
int main() 
{
    int i;
    short us_values[5];
    char Buffer[100],
         line[80], 
         us_activation_value;

    /* Initialisation du robot */
    if (init() != 0) {
        return 1;
    }

    /* Choix des capteurs à activer */
    printf("--------------------------- \
           \nTEST CAPTEURS A ULTRASONS \
           \n---------------------------\
           \n\nQuel(s) capteur(s) activer ?\
           \n1  : Gauche\
           \n2  : Avant-Gauche\
           \n4  : Avant\
           \n8  : Avant-Droit\
           \n16 : Droit\
           \n31 : Tous\
           \n\nAdditionner les valeurs pour activer plusieurs capteurs specifiques\
           \nExemple : activer Gauche et Avant, 1+4, saisir 5\
           \n\nSi aucune valeur saisie : Tous actifs par defaut\n\n");

    printf("Choix : ");
    if (fgets(line, 80, stdin) != NULL && line[0] != '\n') { 
        if (EOF != sscanf(line, "%d", &us_activation_value)) {
            /* Activation des capteurs choisis */
            kh4_activate_us(us_activation_value, dsPic);
        }                             
    } else {
        /* Activation des capteurs par défaut */
        kh4_activate_us(31, dsPic);
    }

    /* Boucle permettant l'actualisation des données affichées */
    while(!kb_kbhit()) { 
        kb_clrscr();

        /* Affichage des informations basiques */
        printf("--------------------------- \
              \nTEST CAPTEURS A ULTRASONS \
              \n---------------------------\
              \n\nINFOS\
              \n- Distance max : 250.0 cm\
              \n- Distance min :  25.0 cm\
              \n- Distance = 2000 si capteur desactive\
              \n- Distance = 1000 si objet a moins de 25 cm ou aucune detection\
              \n\n- Marge d'erreur : +/- 2cm\
              \n\n---------------------------\n\n");

        /* Mesure des distances et stockage dans le tampon */
        kh4_measure_us(Buffer, dsPic);
        for (i = 0; i < 5; i++) {
            us_values[i] = (short)(Buffer[i*2] | Buffer[i*2+1]<<8);                       
        }

        /* Affichage des distances retournées par les capteurs US */ 
        printf("\nAffichage des distance (cm)\
                \n\nCapteur Gauche       : %4d cm\
                \nCapteur Avant-Gauche : %4d cm\
                \nCapteur Avant        : %4d cm\
                \nCapteur Avant-Droit  : %4d cm\
                \nCapteur Droit        : %4d cm\n",
                us_values[0], us_values[1], us_values[2], us_values[3], us_values[4]);

        printf("\nAppuyez sur une touche pour stopper le programme.\n");
        usleep(200000);
    }
    return 0;
}
