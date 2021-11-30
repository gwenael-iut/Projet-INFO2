#include <khepera/khepera.h>
#include <stdio.h>
#include <string.h>

#define FREQUENCE_ENCHANTILLONNAGE 22050 // Hz
#define SAMPLE_SIZE 16 // Bits du sample
#define STEREO 2 // Mode stereo
#define ENDIAN 0 // 0 pour little-endian, 1 pour big-endian
#define SIGNED 1 // 0 pour non-signé, 1 pour signé
#define SIZE 80 // nombre de caractères lus
#define VOL_BASIQUE 80 // volume par défaut des microphones et haut-parleurs

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
    // Arret des hauts-paleurs
	switch_speakers_ON_OFF(0);
    mute_speaker(1);

    kb_sound_release();

    // Réactiver les capteurs a ultrason
    kh4_activate_us(31, dsPic);
}

int main()
{
    char FILENAME[SIZE];
    char line[SIZE]; // Enregistrement du choix
    
    int errInit, 
        errConfig,
        longueur; // durée de l'enregistrement

    unsigned int volMicG, 
                 volMicD,
                 volSpeakG,
                 volSpeakD;

    unsigned long nb_samples;

	if ((errInit = init()) != 0)
        return errInit;

    // Initialisation du son
    if (kb_sound_init() < 0) 
    {
        fprintf(stderr, "Erreur: Impossible d'initialiser le son");
        return -2;
    }

    // Couper les capteurs a ultrasons pour éviter l'effet larsen
    kh4_activate_us(0, dsPic);
    // Couper les hauts parleurs pour éviter l'effet larsen
    set_speakers_volume(0, 0);

    do {
        printf("Entrez le volume des microphones (entre 0 et 100, entrez 0 pour valeur par defaut) : \n");
        printf("Microphone gauche : ");
        scanf("%u", &volMicG);
        printf("Microphone droit : ");
        scanf("%u", &volMicD);

        if (volMicG == 0) volMicG = VOL_BASIQUE;
        if (volMicD == 0) volMicD = VOL_BASIQUE; 
        if (volMicG < 0 || volMicG > 100 || volMicD < 0 || volMicD > 100) {
            fprintf(stderr, "Erreur : Veuillez rentrer un volume correct, c'est-a-dire compris entre 0 et 100.\n ");
        }
    } while (volMicG < 0 || volMicG > 100 || volMicD < 0 || volMicD > 100);
    // Mise a jour du volume des microphones en %
    set_microphones_volume(volMicG, volMicD);

    // configuration du son: ((sampling_frequency,sample_size = nb of bits,sign: 0 = unsigned,endian : 0 = LSB,channels))
    if((errConfig = kb_sound_configure(FREQUENCE_ENCHANTILLONNAGE, SAMPLE_SIZE, SIGNED, ENDIAN, STEREO)) < 0)
    {
        fprintf(stderr, "Erreur: Impossible de configurer le son. Erreur n°%d!\n", errConfig);
        kb_sound_release();
        return -1;
    }

    do {
        printf("Entrez une duree d'enregistrement : \n");
        scanf("%d", &longueur);

        if (longueur < 0) {
            fprintf(stderr, "Veuillez rentrer une duree correcte, c'est-a-dire superieure à 0.\n");
        }
    } while (longueur < 0);

    nb_samples = (FREQUENCE_ENCHANTILLONNAGE * STEREO * longueur * SAMPLE_SIZE / 8);
    char sound[nb_samples]; // Enregistrement du son 

    entree:
    printf("\n Enregistrement de %ds -- entrez a pour ressaisir, autre chose pour continuer \n", longueur);
    // Sauvegarde du choix
    scanf("%s", line);
    if (line[0] == 'a') goto entree;

    // Extinction des haut-parleurs
    mute_speaker(1);
    switch_speakers_ON_OFF(0);

    // Début de l'enregistrement 
    printf("Debut de l'enregistement: \n");
    record_buffer(sound, nb_samples);

    // Extinction des microphones 
    set_microphones_volume(0, 0);
    // et allumage des haut-parleurs
    switch_speakers_ON_OFF(1);
    mute_speaker(0);

    do {
        printf("Entrez le volume des haut-parleurs (entre 0 et 100, entrez 0 pour valeur par defaut) : \n");
        printf("Haut-parleur gauche : ");
        scanf("%u", &volSpeakG);
        printf("Haut-parleur droit : ");
        scanf("%u", &volSpeakD);

        if (volSpeakG == 0) volSpeakG = VOL_BASIQUE;
        if (volSpeakD == 0) volSpeakD = VOL_BASIQUE; 
        if (volSpeakG < 0 || volSpeakG > 100 || volSpeakD < 0 || volSpeakD > 100) {
            fprintf(stderr, "Erreur : Veuillez rentrer un volume correct, c'est-a-dire compris entre 0 et 100.\n ");
        }
    } while (volSpeakG < 0 || volSpeakG > 100 || volSpeakD < 0 || volSpeakD > 100);

    // Volume des hauts parleurs(gauche, droit)
    set_speakers_volume(volSpeakG, volSpeakD);

    usleep(1000000); // Attendre 1 seconde
    printf("\n Ecoute du son enregistre \n");
    play_buffer(sound, nb_samples);

    // Attendre la fin de l'écoute
    wait_end_of_play();

    // Sauvegarde dans un fichier audio
    printf("Choississez un nom de fichier (sans l'extension) pour le sauvegarder : \n");
    scanf("%s", FILENAME); // récupération du nom de fichier
    strcat(FILENAME, ".wav"); // concaténation pour ajouter l'extension audio

    save_wav_file(FILENAME, sound, nb_samples, STEREO, SAMPLE_SIZE, FREQUENCE_ENCHANTILLONNAGE);

    end();
    
    return 0;
}
