/* ------------------------------------------------ */
/* -------          PROGRAMME TEST          ------- */
/* -------         CAPTEUR DE CHOCS         ------- */
/* ------------------------------------------------ */
/* ------- Fichier : test_shock.c           ------- */
/* ------- Auteur  : NAYET Morgan           ------- */
/* ------------------------------------------------ */
#include <khepera/khepera.h>

#define MAX_RAY 500
#define MIN_RAY 150
#define MIN_DIST 5  //cm

static knet_dev_t * dsPic; // Accès au microcontrôleur Pic du robot

/**
 * Méthode permettant l'initialisation du robot
 * Librairie Khepera & connexion au robot
 */
int init()
{
	/* Initialisation de la librairie khepera */
	if(kb_init(0, NULL) < 0) {
		printf("\nERREUR: Echec de l'initialisation de la librairie !\n\n");
		return 1;
	}

	/* Initialisation de la connexion au robot */
	dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
	if(dsPic == NULL) {
		printf("\nERREUR: Echec de la communication avec le robot (Kh4 dsPic)\n\n");
		return 1;
	}
	return 0;
}

/*
 * Fonction permettant le test des capteurs de choc
 * Récupère et affiche les valeurs des capteurs infrarouges sur la sortie standard
 * Affiche un message et arrête le programme si un objet est trop proche
 */
int main(void) 
{
    char Buffer[100],
         * listeIr[] ={"Arriere-Gauche",
                       "Gauche",
                       "Avant-Gauche",
                       "Avant",
                       "Avant-Droit",
                       "Droit",
                       "Arriere-Droit",
                       "Arriere"};
    int tabIRSens[8];
    int i, tamponIR; 
    int sortie = 0;

    if (init() != 0) {
        return 1;
    }

    while(!kb_kbhit() && sortie != 1){
        kb_clrscr();
        printf("-------------------- \
        		\nTEST CAPTEUR DE CHOC \
        		\n-------------------- \
        		\n\nUtilisation des capteurs infrarouges\n");

        /* Récupère les valeurs des capteurs et les stocke dans le tampon  */
        kh4_proximity_ir(Buffer, dsPic);

        for( i = 0 ; i < 8 ; i++ ){

            tamponIR  = (Buffer[i*2] | Buffer[i*2+1]<<8);

            if(tamponIR > MAX_RAY){
                tabIRSens[i] = MAX_RAY;
                sortie = 1;
            }else{
                tabIRSens[i] = tamponIR - MIN_DIST;
            }
        }

        /* Affichage des valeurs sur la sortie standard */
        printf("\nAffichage des valeurs de chaque capteur :\n\
            \nCapteur %s\t: %4d\
            \nCapteur %s\t\t: %4d\
            \nCapteur %s\t: %4d\
            \nCapteur %s\t\t: %4d\
            \nCapteur %s\t: %4d\
            \nCapteur %s\t\t: %4d\
   	        \nCapteur %s\t: %4d\
   	        \nCapteur %s\t\t: %4d\n",
            listeIr[0],tabIRSens[0], listeIr[1],tabIRSens[1],
            listeIr[2],tabIRSens[2], listeIr[3],tabIRSens[3],
            listeIr[4],tabIRSens[4], listeIr[5],tabIRSens[5],
            listeIr[6],tabIRSens[6], listeIr[7],tabIRSens[7]);

        if(sortie != 1){
            printf("\nAppuyez sur une touche pour stopper le programme.\n");
            usleep(200000);
        } else {
            printf("\nLes capteurs qui ont detectes un choc a venir: \n");

            for( i = 0 ; i < 8 ; i++ ){
                if(tabIRSens[i] == MAX_RAY)
                printf("Le capteur %s avec une valeur de %d est a une distance de moins de %d cm\n",
                listeIr[i], tabIRSens[i], MIN_DIST);
            }
        }
    }
    return 0;
}
