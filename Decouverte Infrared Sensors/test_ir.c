/* ------------------------------------------------ */
/* -------          PROGRAMME TEST          ------- */
/* -------       CAPTEURS INFRAROUGES       ------- */
/* ------------------------------------------------ */
#include <khepera/khepera.h>

static knet_dev_t * dsPic; // Accès au microcontrolleur Pic

/**
 * Initialisation de la librairie khepera et la connexion au robot
 * @return 1 if error appears else 0
 */
int init()
{
	printf("Decouverte des capteur infrarouges sous le robot\r\n");
	
	// Initialisation de la librairie khepera
	if(kh4_init(0, NULL) != 0)
	{
		printf("\nERROR: could not initiate the libkhepera\n\n");
		return 1;
	}

	// connexion au robot
	dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
	if(dsPic == NULL)
	{
		printf("\nERROR: could not initiate communication with kh4 dsPic\n\n");
		return 1;
	}
	printf("Decouverte des capteur infrarouges sous le robot\r\n");
	return 0;
}

int main(void) {

  char Buffer[100];
  int back_right_ir;
  int right_ir;
  int front_right_ir;
  int front_ir;
  int front_left_ir;
  int left_ir;
  int back_left_ir;
  int back_ir;

  if (init() != 0)
  {
  	return 1;
  }

  while(!kb_kbhit()){
    kb_clrscr();
    printf("Test des capteurs infrarouges  :\n");

    /* Récupère les valeurs des capteurs et les stocke dans le tampon  */
    kh4_proximity_ir(Buffer, dsPic);

    back_right_ir = (Buffer[7*2] | Buffer[7*2+1]<<8);
    right_ir = (Buffer[6*2] | Buffer[6*2+1]<<8);
    front_right_ir = (Buffer[5*2] | Buffer[5*2+1]<<8);
    front_ir = (Buffer[4*2] | Buffer[4*2+1]<<8);
    front_left_ir = (Buffer[3*2] | Buffer[3*2+1]<<8);
    left_ir = (Buffer[2*2] | Buffer[2*2+1]<<8);
    back_left_ir = (Buffer[1*2] | Buffer[1*2+1]<<8);
    back_ir = (Buffer[0*2] | Buffer[0*2+1]<<8);

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

    printf("\nAppuyez sur un touche pour stopper le programme.\n");
    usleep(200000);
  }
  return 0;
}
