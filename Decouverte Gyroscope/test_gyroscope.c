/* ------------------------------------------------ */
/* -------          PROGRAMME TEST          ------- */
/* -------           DU GYROSCOPE           ------- */
/* ------------------------------------------------ */
/* ------- Fichier : test_gyroscope.c       ------- */
/* -------                                  ------- */
/* ------- Auteur  : BASCOUR Gwenaël        ------- */
/* ------- Auteur  : CAUSSE Jade            ------- */
/* ------- Auteur  : DA COSTA Yacine        ------- */
/* ------------------------------------------------ */
#include <khepera/khepera.h>

#define TAILLE 100

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
 * Fonction permettant de tester le gyroscope du robot
 * 2 modes d'affichage possibles :
 *   - Uniquement les moyennes
 *   - Toutes les valeurs et leur moyenne
 * Affichage des données sur la sortie standard
 */
int main()
{
    int i = 0;
    double valCourante,
           valSuiv;
    char toucheEnfoncee,
         buffer[TAILLE];

    /* Initialisation du robot */
    if (init() != 0) {
        return 1;
    }

    printf("------------------- \
           \nTEST DU GYROSCOPE \
           \n-------------------\n\n");
    printf("Quel mode souhaitez-vous choisir ?\n");
    printf("a -> Afficher la moyenne des valeurs du gyroscope\n");
    printf("b -> Afficher les 10 valeurs recuperees par le gyroscope et leur moyenne\n\n");

    /* Demande du mode d'affichage des données */
    printf("Choix : ");
    toucheEnfoncee = getchar();

    /* Récupération et affichage des données en fonction du mode choisi */
    switch (toucheEnfoncee) {
    case 'a':
        while (!kb_kbhit()) {
            usleep(200000);
            kb_clrscr();

            printf("------------------- \
                   \nTEST DU GYROSCOPE \
                   \n-------------------\n\n");
			
            // Récupération des valeurs pour chaque axe X Y Z dans un tableau
            kh4_measure_gyro(buffer, dsPic);
			
            // Remise à zéro de la valeur courante pour X
            valCourante = 0;
            for (i = 0; i < 10; i++) valCourante += ((short)((buffer[i * 2] | buffer[i * 2 + 1] << 8))) * KH4_GYRO_DEG_S;
            printf("Moyenne des 10 valeurs de X :\n ");
            printf("%6.1f\n\n", valCourante / 10.0);

            // Remise à zéro de la valeur courante pour Y
            valCourante = 0;
            for (i = 10; i < 20; i++) valCourante += ((short)((buffer[i * 2] | buffer[i * 2 + 1] << 8))) * KH4_GYRO_DEG_S;
            printf("Moyenne des 10 valeurs de Y :\n ");
            printf("%6.1f\n\n", valCourante / 10.0);

            // Remise à zéro de la valeur courante pour Z
            valCourante = 0;
            for (i = 20; i < 30; i++) valCourante += ((short)((buffer[i * 2] | buffer[i * 2 + 1] << 8))) * KH4_GYRO_DEG_S;
            printf("Moyenne des 10 valeurs de Z :\n ");
            printf("%6.1f\n\n", valCourante / 10.0);

            printf("\nAppuyez sur une touche pour stopper le programme.\n");
        }
        break;

    case 'b':
        while (!kb_kbhit()) {
            kb_clrscr();

            printf("------------------- \
                   \nTEST DU GYROSCOPE \
                   \n-------------------\n\n");

            // Récupération des valeurs pour chaque axe X Y Z dans un tableau
            kh4_measure_gyro(buffer, dsPic);

            // Affichage des 10 valeurs récupérées par le gyroscope et de la moyenne pour l'axe X
            printf("Valeurs de X [deg/s] :\n");
            valCourante = 0;

            // Les valeurs pour l'axe X sont stockées de l'indice 0 à l'indice 9 du buffer
            for (i = 0; i < 10; i++) {
                // Décalage binaire puis conversion en deg/s
                valSuiv = ((short)((buffer[i * 2] | buffer[i * 2 + 1] << 8))) * KH4_GYRO_DEG_S;
                printf("%6.1f\n", valSuiv);
                valCourante += valSuiv;
            }
            printf("Moyenne des 10 valeurs de X :\n");
            printf("%6.1f\n\n", valCourante / 10.0);


            // Affichage des 10 valeurs récupérées par le gyroscope et de la moyenne pour l'axe Y
            printf("Valeurs de Y [deg/s] :\n");
            valCourante = 0;

            // Les valeurs pour l'axe Y sont stockées de l'indice 10 à l'indice 19 du buffer
            for (i = 10; i < 20; i++) {
                // Décalage binaire puis conversion en deg/s
                valSuiv = ((short)((buffer[i * 2] | buffer[i * 2 + 1] << 8))) * KH4_GYRO_DEG_S;
                printf("%6.1f\n", valSuiv);
                valCourante += valSuiv;
            }
            printf("Moyenne des 10 valeurs de Y :\n");
            printf("%6.1f\n\n", valCourante / 10.0);


            // Affichage des 10 valeurs récupérées par le gyroscope et de la moyenne pour l'axe Z
            printf("Valeurs de Z [deg/s] :\n ");
            valCourante = 0;

            // Les valeurs pour l'axe Z sont stockées de l'indice 20 à l'indice 29 du buffer
            for (i = 20; i < 30; i++) {
                // Décalage binaire puis conversion en deg/s
                valSuiv = ((short)((buffer[i * 2] | buffer[i * 2 + 1] << 8))) * KH4_GYRO_DEG_S;
                printf("%6.1f\n", valSuiv);
                valCourante += valSuiv;
            }
            printf("Moyenne des 10 valeurs de Z :\n ");
            printf("%6.1f\n\n", valCourante / 10.0);

            printf("\nAppuyez sur une touche pour stopper le programme.\n");
            usleep(1000000); // Attente de 1s avant de réactualiser les valeurs
        }
        break;
    }
    return 0;
}
