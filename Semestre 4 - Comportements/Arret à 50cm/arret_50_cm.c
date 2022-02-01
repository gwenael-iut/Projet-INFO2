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

void end() {
    kh4_SetMode(kh4RegSpeed, dsPic);
    kh4_set_speed(0, 0, dsPic);
    kh4_SetMode(kh4RegIdle, dsPic);
}

int main()
{
    int errInit;
    char buffer[100];
    short distance_value;

    int rDroite,
        rGauche;

    double cible;

    if ((errInit = init()) != 0)
        return errInit;
    kh4_SetMode(kh4RegSpeed, dsPic);
    kh4_activate_us(4, dsPic);
    kh4_set_speed(200, 200, dsPic);
    do {
        kb_clrscr();
        kh4_measure_us(buffer, dsPic);
        distance_value = (short)(buffer[4] | buffer[5]<<8);
        printf("Distance actuelle : %4d cm.", distance_value);
    } while(distance_value == 1000);

    // Distance value != 1000 : Le capteur a détécté le mur.
    usleep(10000);
    kh4_SetSpeedProfile(3, 0, 20, 1, 400, dsPic);
    // On mesure la distance entre l'objet et le robot (en milimètres);
    kh4_measure_us(buffer, dsPic);
    distance_value = (short)(buffer[4] | buffer[5]<<8);
    distance_value *= 10;

    kh4_get_position(&rGauche, &rDroite, dsPic);

    cible = distance_value/KH4_PULSE_TO_MM;

    kh4_SetMode(kh4RegPosition, dsPic);
    kh4_set_position(rGauche+(long)cible), rDroite+(long)cible, dsPic);

    end();
    return 0;
}