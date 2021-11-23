#include <khepera/khepera.h>

#define IR_BAR_LEN 15 //display bar length for IR sensor

static knet_dev_t * dsPic; // robot pic microcontroller access

/**
 * initiate dsPic, libkhepera and robot access
 * @return 1 if error appears else 0
 */
int init()
{
	printf("Decouverte des capteur infrarouges sous le robot\r\n");
	
	// initiate libkhepera and robot access
	if(kh4_init(0, NULL) != 0)
	{
		printf("\nERROR: could not initiate the libkhepera\n\n");
		return 1;
	}

	/* open robot socket and store the handle in its pointer */
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
  int left_ir_g;
  int right_ir_g;
  int front_left_ir_g;
  int front_right_ir_g;

  if (init() != 0)
  {
  	return 1;
  }

  while(!kb_kbhit()){
    kb_clrscr();
    printf("Test des capteurs infrarouges au sol :\n");

    /* get ir sensor  */
    kh4_proximity_ir(Buffer, dsPic);

    /* get ir values for ground infrared sensors */
    left_ir_g=(Buffer[8*2] | Buffer[8*2+1]<<8);
    right_ir_g=(Buffer[9*2] | Buffer[9*2+1]<<8);
    front_left_ir_g=(Buffer[10*2] | Buffer[10*2+1]<<8);
    front_right_ir_g=(Buffer[11*2] | Buffer[11*2+1]<<8);

    printf("\nLa valeur du capteur droit: %d\n\
La valeur du capteur gauche est %d\n\
La valeur du capteur avant gauche est %d\n\
La valeur du capteur avant droit est %d\n",
    right_ir_g, left_ir_g, front_left_ir_g, front_right_ir_g);

    printf("\nAppuyez sur un touche pour stopper le programme.\n");
    usleep(200000);
  }
  return 0;
}
