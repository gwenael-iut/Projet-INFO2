/* ------------------------------------------------ */
/* -------          PROGRAMME TEST          ------- */
/* -------       CAPTEURS INFRAROUGES       ------- */
/* -------           SOUS LE ROBOT          ------- */
/* ------------------------------------------------ */
/* ------- Fichier : test_ir_bottom.c       ------- */
/* ------- Auteur  : NAYET Morgan           ------- */
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
 * Fonction permettant le test des capteurs infrarouges situés sous robot.
 * Récupère les valeurs des capteurs et les affichent sur la sortie standard.
 */
int main(void) 
{
    char Buffer[100];
    int left_ir_g;
    int right_ir_g;
    int front_left_ir_g;
    int front_right_ir_g;

    /* Initialisation du robot */
    if (init() != 0) {
        return 1;
    }

    while(!kb_kbhit()){
        kb_clrscr();
        printf("----------------------------------------- \
               \nTEST CAPTEURS INFRAROUGES SOUS LE ROBOT \
               \n-----------------------------------------\n\n");

        /* Récupère les valeurs des capteurs et les stocke dans le tampon  */
        kh4_proximity_ir(Buffer, dsPic);

        /* Attribue les valeurs du tampon à la variable du capteur correspondant */
        left_ir_g = (Buffer[8*2] | Buffer[8*2+1]<<8);
        right_ir_g = (Buffer[11*2] | Buffer[11*2+1]<<8);
        front_left_ir_g = (Buffer[9*2] | Buffer[9*2+1]<<8);
        front_right_ir_g = (Buffer[10*2] | Buffer[10*2+1]<<8);

        /* Affichage des valeurs sur la sortie standard */
        printf("\nLa valeur du capteur droit: %d\
               \nLa valeur du capteur gauche est %d\
               \nLa valeur du capteur avant gauche est %d\
               \nLa valeur du capteur avant droit est %d\n",
            right_ir_g, left_ir_g, front_left_ir_g, front_right_ir_g);

        printf("\nAppuyez sur une touche pour stopper le programme.\n");
        usleep(200000);
    }
    return 0;
}
