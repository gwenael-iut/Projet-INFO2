/* ------------------------------------------------ */
/* -------          PROGRAMME TEST          ------- */
/* -------       CAPTEURS INFRAROUGES       ------- */
/* ------------------------------------------------ */
/* ------- Fichier : test_ir.c              ------- */
/* -------                                  ------- */
/* ------- Auteur  : BASCOUR Gwenaël        ------- */
/* ------- Auteur  : CAUSSE Jade            ------- */
/* ------- Auteur  : DA COSTA Yacine        ------- */
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
 * Fonction permettant le test des capteurs infrarouges situés autour du robot.
 * Récupère les valeurs des capteurs et les affichent sur la sortie standard.
 */
int main()
{
    char Buffer[100];
    int back_right_ir;
    int right_ir;
    int front_right_ir;
    int front_ir;
    int front_left_ir;
    int left_ir;
    int back_left_ir;
    int back_ir;

    /* Initialisation du robot */
    if (init() != 0) {
        return 1;
    }

    while(!kb_kbhit()) {
        kb_clrscr();
        printf("--------------------------- \
               \nTEST CAPTEURS INFRAROUGES \
               \n---------------------------\n\n");

        /* Récupère les valeurs des capteurs et les stocke dans le tampon  */
        kh4_proximity_ir(Buffer, dsPic);

        /* Attribue les valeurs du tampon à la variable du capteur correspondant */
        back_ir = (Buffer[7*2] | Buffer[7*2+1]<<8);
        back_right_ir = (Buffer[6*2] | Buffer[6*2+1]<<8);
        right_ir = (Buffer[5*2] | Buffer[5*2+1]<<8);
        front_right_ir = (Buffer[4*2] | Buffer[4*2+1]<<8);
        front_ir = (Buffer[3*2] | Buffer[3*2+1]<<8);
        front_left_ir = (Buffer[2*2] | Buffer[2*2+1]<<8);
        left_ir = (Buffer[1*2] | Buffer[1*2+1]<<8);
        back_left_ir = (Buffer[0*2] | Buffer[0*2+1]<<8);

        /* Affichage des valeurs sur la sortie standard */
        printf("\nAffichage valeurs de chaque capteur :\
                \n\nCapteur Arriere Droit  : %4d\
                \nCapteur Droit          : %4d\
                \nCapteur Avant Droit    : %4d\
                \nCapteur Avant          : %4d\
                \nCapteur Avant Gauche   : %4d\
                \nCapteur Gauche         : %4d\
                \nCapteur Arriere Gauche : %4d\
                \nCapteur Arriere        : %4d\n",
            back_right_ir, right_ir, front_right_ir, front_ir, front_left_ir, left_ir, back_left_ir, back_ir);

        printf("\nAppuyez sur une touche pour stopper le programme.\n");
        usleep(200000);
    }
    return 0;
}
