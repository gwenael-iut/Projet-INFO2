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
    char line[80], l[3];
    int first[3],
        second[3],
        third[3];

    printf("Test des leds du robot\n");
    printf("Configuration de la led n°1: Rouge Vert Bleu (entre 0 et 63) : ");
    fgets(line, 80, stdout);

    if (EOF == sscanf(line, "%d %d %d", &l[0], &l[1], &l[2]))
    {
        fprintf(stderr, "Erreur : la ligne doit être composée de 3 entiers.");
        return 1;
    }

    for (size_t i = 0; i < 3; i++)
    {
        first[i] = l[i];
    }

    printf("Configuration de la led n°2: Rouge Vert Bleu (entre 0 et 63) : ");
    fgets(line, 80, stdout);

    if (EOF == sscanf(line, "%d %d %d", &l[0], &l[1], &l[2]))
    {
        fprintf(stderr, "Erreur : la ligne doit être composée de 3 entiers.");
        return 1;
    }

    for (size_t i = 0; i < 3; i++)
    {
        second[i] = l[i];
    }

    printf("Configuration de la led n°3: Rouge Vert Bleu (entre 0 et 63) : ");
    fgets(line, 80, stdout);

    if (EOF == sscanf(line, "%d %d %d", &l[0], &l[1], &l[2]))
    {
        fprintf(stderr, "Erreur : la ligne doit être composée de 3 entiers.");
        return 1;
    }

    for (size_t i = 0; i < 3; i++)
    {
        third[i] = l[i];
    }
    
    kh4_SetRGBLeds(first[0], first[1], first[2], second[0], second[1], second[2], third[0], third[1], third[2], dsPic);
    printf("Led 1 : R:%d G:%d B: %d\nLed 2 : R:%d G:%d B: %d\nLed 3 : R:%d G:%d B: %d\n", first[0], first[1], first[2], second[0], second[1], second[2], third[0], third[1], third[2]);
    
    return 0;
}
