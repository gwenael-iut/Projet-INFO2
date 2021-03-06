/* ----------------------------------------------- */
/* -------        PROGRAMME DE TEST        ------- */
/* -------          HAUT-PARLEURS          ------- */
/* ----------------------------------------------- */
/* ------- Fichier : test_speakers.c       ------- */
/* ------- Auteur  : BASCOUR Gwenaël       ------- */
/* ----------------------------------------------- */
#include <khepera/khepera.h>

#define SAMPLING_FREQ 22050 // Fréquence en Hertz (Hz)
#define SAMPLE_SIZE 16      // Bits du sample
#define NB_CHANNELS 2       // Mode stereo
#define ENDIAN 0            // 0 pour little-endian, 1 pour big-endian
#define SIGNED 1            // 0 pour non-signé, 1 pour signé

static knet_dev_t * dsPic;  // Accès au microcontrôleur Pic du robot

/*
 * Méthode permettant l'initialisation du robot
 * Librairie Khepera & connexion au robot
 */
int init(int argc, char * argv[])
{
    int rc;

    /* Initialisation de la librairie khepera */
    if ((rc = kb_init(argc , argv)) < 0 ) {
        printf("\nERREUR: Echec de l'initialisation de la librairie !\n\n");
        return 1;
    }

    /* Initialisation de la connexion au robot */
    dsPic  = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL );
    if (dsPic == NULL) {
        printf("\nERREUR: Echec de la communication avec le robot (Kh4 dsPic)\n\n");
        return 1;
    }

    return 0;
}

/*
 * Méthode permettant l'initialisation et la configuration 
 * du système de son du robot.
 */
int init_sound()
{
    /* Initialisation du son sur le robot */
    if (kb_sound_init() < 0) {
        fprintf(stderr, "ERREUR: Echec de l'initialisation du son !\r\n");
        return -1;
    }

    /* Configuration du son */
    if ((kb_sound_configure(SAMPLING_FREQ, SAMPLE_SIZE, SIGNED, ENDIAN, NB_CHANNELS)) < 0) {
        fprintf(stderr,"ERREUR: Echec de la configuration du son !\r\n");
        kb_sound_release();
        return -1;
    }

    /* Couper les ultrasons */
    kh4_activate_us(0, dsPic);

    return 0;
}

/*
 * Méthode permettant de remettre le robot et son système
 * de son dans son état initial.
 */
int reset_sound(char * sound_buff)
{
    // Vider le tampon contenant le son
    free(sound_buff);

    // Remettre les haut-parleurs dans l'état initial
    switch_speakers_ON_OFF(0);
    mute_speaker(1);
    kb_sound_release();

    // Réactiver les capteurs à ultrasons
    kh4_activate_us(31, dsPic);

    return 0;
}

/*
 * Fonction permettant le test des haut-parleurs du robot.
 * - Initialise et configure le système de son
 * - Demande le volume des haut-parleurs
 * - Lit le fichier passer en argument ou le fichier par défaut
 */
int main(int argc, char *argv[]) 
{
	char * DEFAULT_FILENAME = "beep.wav"; // Fichier audio par défaut
    char * sound_buff = NULL;

    char * FILENAME;
    int data_size, sample_rate, volSpeakG, volSpeakD;
    short channels, bits_per_sample;

    /* Initialisation du robot */
    if (init(argc, argv) != 0) {
        return 1;
    }

    /* l'initialisation et la configuration du son */
    if (init_sound()) != 0) {
        return 1;
    }

    /* Définir le volumes des haut-parleurs (%) */
    printf("-------------------- \
           \nTEST HAUT-PARLEURS \
           \n--------------------\n\n");
    do {
    	  printf("Entrez le volume des haut-parleurs (entre 0 et 100)\n");
    	  printf("Haut-parleur gauche : ");
    	  scanf("%d", &volSpeakG);
    	  printf("Haut-parleur droit : ");
    	  scanf("%d", &volSpeakD);

    	  if (volSpeakG < 0 || volSpeakG > 100 || volSpeakD < 0 || volSpeakD > 100) {
            fprintf(stderr, "ERREUR: Veuillez rentrer un volume correct, entre 0 et 100.\n ");
        }
    } while (volSpeakG < 0 || volSpeakG > 100 || volSpeakD < 0 || volSpeakD > 100);
    set_speakers_volume(volSpeakG, volSpeakD);

    /* Ouverture du fichier audio (.wav) */
    if (argc == 2 && (fopen(argv[1], "r") != NULL)) {
    	  FILENAME = argv[1];
    } else if (argc == 2 && fopen(argv[1], "r") == NULL) {
    	  fprintf(stderr, "\nERREUR: Probleme d'ouverture du fichier. Utilisation du fichier par defaut.\n");
    	  FILENAME = DEFAULT_FILENAME;
    } else {
    	  printf("\nAucun fichier choisi. Utilisation du fichier par defaut.\n");
    	  FILENAME = DEFAULT_FILENAME;
    }

    /* Lecture du fichier audio et stockage dans le tampon */
    if ((load_wav_file(FILENAME, &sound_buff, &data_size, &channels, &bits_per_sample, &sample_rate)) < 0) {
    	  printf("ERREUR: Impossible de lire le fichier audio %s !\r\n", FILENAME);
    	  reset_sound(sound_buff);
    	  return -1;
    }

    /* Activation des haut-parleurs */
    switch_speakers_ON_OFF(1);
    mute_speaker(0);

    /* Jouer le son du fichier choisi */
    printf("\nLe fichier lu sera : %s\n\n", FILENAME);
    printf("Lecture en cours...\n\n");
    play_buffer(sound_buff, data_size);
    wait_end_of_play();
    printf("Lecture terminee.\n\n");

    /* Remettre le robot dans son état initial */
    reset_sound(sound_buff);

    return 0;
}
