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
    tcflush(0, 0);
    fflush(stdin);
    kh4_SetRGBLeds(0, 0, 0, 0, 0, 0, 0, 0, 0, dsPic);
}

int main()
{
    init();

    char leds[9];
    char prompt[80];

    printf("Test des leds du robot\n");

    printf("Entrez les valeurs RGB des trois leds a la suite \n (Ex : 120 0 0 0 122 0 0 0 120) \n");
    tcflush(0, 0);
    fflush(stdin);
    fgets(prompt, 80, stdin);
    
    if (EOF == sscanf(prompt, "%d %d %d %d %d %d %d %d %d", &leds[0], &leds[1], &leds[2], &leds[3], &leds[4], &leds[5], &leds[6], &leds[7], &leds[8]))
        printf("\nErreur : Syntaxe : R1 G1 B1 R2 G2 B2 R3 G3 B3\n");
    else
    {
        kh4_SetRGBLeds(leds[0], leds[1], leds[2], leds[3], leds[4], leds[5], leds[6], leds[7], leds[8], dsPic);
        printf("RGB : \n\t - LED 1 : %d %d %d \n\t - LED 2 : %d %d %d \n\t - LED 3 : %d %d %d\n", leds[0], leds[1], leds[2], leds[3], leds[4], leds[5], leds[6], leds[7], leds[8]);
        usleep(5000000);
    }

    end();

    return 0;
}
