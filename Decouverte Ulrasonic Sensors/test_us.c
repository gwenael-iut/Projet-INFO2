/* ------------------------------------------------ */
/* -------          PROGRAMME TEST          ------- */
/* -------       CAPTEURS À ULTRASONS       ------- */
/* ------------------------------------------------ */
#include <khepera/khepera.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    int rc;
    int i, position;
    char us_activation_value;
    char Buffer[100];
    short us_values[5];
    /*int us_activation_possibilities[16] = {1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 16, 17, 20, 24, 31};*/

    static knet_dev_t * dsPic;
    dsPic  = knet_open( "Khepera4:dsPic" , KNET_BUS_I2C , 0 , NULL );

    /* Set the libkhepera debug level - Highly recommended for development. */
    kb_set_debug_level(2);

    /* Initialisation de la librairie khepera */
    if((rc = kb_init( argc , argv )) < 0 )
        return 1;

    /* Affichage des informations basiques */
    printf("--------------------------- \
           \nTEST CAPTEURS À ULTRASONS \
           \n---------------------------\
           \n\nINFOS\
           \n- Distance max : 250.0 cm\
           \n- Distance min :  25.0 cm\
           \n- Distance = 2000 si capteur désactivé\
           \n- Distance = 0 si objet à moins de 25 cm\
           \n- Distance = 1000 si aucune détection\
           \n\n---------------------------\n\n");

    /* Choix des capteurs à activer */
    printf("Quel(s) capteur(s) activer ?\
           \n1  : Gauche\
           \n2  : Avant-Gauche\
           \n4  : Avant\
           \n8  : Avant-Droit\
           \n16 : Droit\
           \n31 : Tous\
           \n\nAdditionner deux valeurs pour activer plusieurs capteurs spécifiques\
           \nExemple : activer Gauche et Avant, 1+4, saisir 5\n\n");

    printf("Choix : ");
    scanf("%d", &us_activation_value);
    printf("%c", us_activation_value);

    kh4_activate_us(us_activation_value, dsPic);

    /* Affichage des distances retournées par les capteurs US */
    while(!kb_kbhit()) {
        kb_clrscr();

	    kh4_measure_us(Buffer, dsPic);

	    for (i = 0 ; i < 5 ; i++) {
            us_values[i] = (short)(Buffer[i*2] | Buffer[i*2+1]<<8);
        }

        printf("\nAffichage des distance (cm)\
                \n\nCapteur Gauche       : %4d cm\
                \nCapteur Avant-Gauche : %4d cm\
	            \nCapteur Avant        : %4d cm\
                \nCapteur Avant-Droit  : %4d cm\
                \nCapteur Droit        : %4d cm\n",
                us_values[0], us_values[1], us_values[2], us_values[3], us_values[4]);

        printf("\nAppuyez sur un touche pour stopper le programme.\n");
        usleep(200000);
    }
    return 0;
}
