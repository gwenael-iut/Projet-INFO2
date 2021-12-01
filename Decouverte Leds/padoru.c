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

void end()
{
    kh4_setRGBLeds(0, 0, 0, 0, 0, 0, 0, 0, 0);
}

int main()
{
    printf("HASHIRE SORI YO\nKAZE NO YOU NI\nTSUKIMIHARA WO\nPADORU PADORU\n(Press a key to stop)");
    while(!kb_kbhit) {
        kh4_setRGBLeds(255, 0, 0, 0, 255, 0, 255, 0, 0);
        usleep(100000);
        kh4_setRGBLeds(0, 255, 0, 255, 0, 0, 255, 0, 0);
        usleep(100000);
        kh4_setRGBLeds(255, 0, 0, 255, 0, 0, 0, 255, 0);
        usleep(100000);
    }
    
    end();
    return 0;
}
