#include <khepera/khepera.h>

static knet_dev_t *dsPic; // robot pic microcontroller access

int init()
{
	printf("\nKhepera 4 - testkh4\n");

	// initiate libkhepera and robot access
	if (kh4_init(0, NULL) != 0)
	{
		printf("\nERROR: could not initiate the libkhepera!\n\n");
		return -1;
	}

	/* open robot socket and store the handle in its pointer */
	dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
	if (dsPic == NULL)
	{
		printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
		return -2;
	}

	return 0;
}

int main()
{
  int errInit;
  char buffer[100];

	if ((errInit = init()) != 0)
        return errInit;

    kh4_activate_us(4, dsPic);
    while(kh4_measure_us(buffer, dsPic))

    return 0;
}