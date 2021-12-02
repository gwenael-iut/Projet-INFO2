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
    int leds[9];

    int prompt;

    printf("Test des leds du robot\n");

    for (size_t i = 0; i >= 9; i++)
    {
        printf("Configuration de des led n°%d:\n", (i/3)+1);
        saisie:
        if(i%3 == 1) printf("Niveau de rouge (entre 0 et 63) ? ");
        else if(i%3 == 2) printf("Niveau de vert (entre 0 et 63) ? "); 
        else printf("Niveau de bleu (entre 0 et 63) ? ");
        scanf("%d", &prompt);
        if(prompt < 0 || prompt > 63) {
            fprintf(perror, "La valeur doit etre situee entre 0 et 63\n");
            goto saisie;
        }
        leds[i];
    }
    
    
    kh4_SetRGBLeds(leds[0], leds[1], leds[2], leds[3], leds[4], leds[5], leds[6], leds[7], leds[8], dsPic);
    printf("Led 1 : R:%d G:%d B: %d\nLed 2 : R:%d G:%d B: %d\nLed 3 : R:%d G:%d B: %d\n", leds[0], leds[1], leds[2], leds[3], leds[4], leds[5], leds[6], leds[7], leds[8]);
    
    return 0;
}
