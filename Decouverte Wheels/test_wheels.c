/* ----------------------------------------------- */
/* -------        PROGRAMME DE TEST        ------- */
/* -------         ROUES & MOTEURS         ------- */
/* ----------------------------------------------- */
/* ------- Fichier : test_wheels.c          ------ */
/* -------                                  ------ */
/* ------- Auteur  : CAUSSE Jade            ------ */
/* ------- Auteur  : DA COSTA Yacine        ------ */
/* ----------------------------------------------- */
#include <khepera/khepera.h>

#define RATIO 0.678181   // Conversion des unités de vitesse en mm/s
#define INCREMENT_P 25   // Petit incrément des unités de vitesse
#define INCREMENT_G 100  // Grand incrément des unités de vitesse

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
 */
void end()
{
    kb_change_term_mode(0);     // Terminal en mode initial
    kh4_set_speed(0, 0, dsPic); // Arret du robot
    kh4_SetMode(kh4RegIdle, dsPic);
}

/*
 * Fonction permettant le test des moteurs et du fonctionnement des roues
 * du robot.
 * L'utilisateur peut déplacer le robot et augmenter ou diminuer sa vitesse
 * à l'aide des touches du clavier.
 */
int main()
{
    int sortieBoucle = 0,
        vitessePulse = 200,
        vitesseMMS = vitessePulse * RATIO;
    char toucheEnfoncee,
         prec = 'a';

	  /* Initialisation du robot */
    if (init() != 0) {
        return 1;
    }

    printf("--------------------- \
           \nTEST ROUES & MOTEURS \
           \n---------------------\n\n");
    printf("Direction avec les touches ZQSD, arret avec A\n");
    printf("Augmentation de la vitesse (+25) avec B, Reduction (-25) avec N\n");
    printf("Augmentation de la vitesse (+100) avec H, Reduction (-100) avec J\n");
    printf("Quitter le programme avec X.\n\n");

    kb_change_term_mode(1); // Changement du mode du terminal afin de pouvoir récuperer les entrées
    kh4_SetMode(kh4RegSpeed, dsPic);

    while (!sortieBoucle)
    {
        if (kb_kbhit()) // Fonction permettant de savoir si une touche est actuellement enfoncée
        {
            toucheEnfoncee = getchar();
            aSpeed:
            switch (toucheEnfoncee) {
            // Cas de déplacement du robot (avant / arrière / droite / gauche)
            case 'z':
                kh4_set_speed(vitessePulse, vitessePulse, dsPic);
                prec = 'z';
                break;
            case 's':
                kh4_set_speed(-vitessePulse, -vitessePulse, dsPic);
                prec = 's';
                break;
            case 'd':
                kh4_set_speed(vitessePulse, -vitessePulse, dsPic);
                prec = 'd';
                break;
            case 'q':
                kh4_set_speed(-vitessePulse, vitessePulse, dsPic);
                prec = 'q';
                break;

            // Cas d'arrêt des moteurs
            case 'a':
                kh4_set_speed(0, 0, dsPic);
                prec = 'a';
                break;

            // Cas de fin du programme de test
            case 'x':
                sortieBoucle = 1;
                break;

            // Augmentation / Diminution de la vitesse par petit incrément
            case 'b' :
                vitessePulse += INCREMENT_P;
                if (vitessePulse > 1200)
                    vitessePulse = 1200;
                vitesseMMS = vitessePulse * RATIO;
                printf("Vitesse actuelle : %d (%d mm/s)\n", vitessePulse, vitesseMMS);
                toucheEnfoncee = prec;
                goto aSpeed;
                break;
			      case 'n' :
                vitessePulse -= INCREMENT_P;
                if (vitessePulse < 5)
                    vitessePulse = 5;
                vitesseMMS = vitessePulse * RATIO;
                printf("Vitesse actuelle : %d (%d mm/s)\n", vitessePulse, vitesseMMS);
                toucheEnfoncee = prec;
                goto aSpeed;
                break;

            // Augmentation / Diminution de la vitesse par grand incrément
            case 'h' :
                vitessePulse += INCREMENT_G;
                if (vitessePulse > 1200)
                    vitessePulse = 1200;
                vitesseMMS = vitessePulse * RATIO;
                printf("Vitesse actuelle : %d (%d mm/s)\n", vitessePulse, vitesseMMS);
                toucheEnfoncee = prec;
                goto aSpeed;
                break;
            case 'j' :
                vitessePulse -= INCREMENT_G;
                if (vitessePulse < 5)
                    vitessePulse = 5;
                vitesseMMS = vitessePulse * RATIO;
                printf("Vitesse actuelle : %d (%d mm/s)\n", vitessePulse, vitesseMMS);
                toucheEnfoncee = prec;
                goto aSpeed;
                break;

            default:
                break;
            }
        }
    }

    // Remettre le robot dans son état initial
    end();
    return 0;
}
