#include <khepera/khepera.h>

#define FREQUENCE_ENCHANTILLONNAGE 22050 // Hz
#define SAMPLE_SIZE 16 // Bits du sample
#define STEREO 2 // Mode stereo
#define LONGUEUR 5 // Longueur du son

#define ENDIAN 0 // 0 pour little-endian, 1 pour big-endian
#define SIGNED 1 // 0 pour non-signé, 1 pour signé

#define NB_SAMPLES (FREQUENCE_ENCHANTILLONNAGE * STEREO * LONGUEUR * SAMPLE_SIZE / 8)

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
    char FILENAME[] = "enregistrement.wav";
    char line[80]; // Enregistrement du choix
    char sound[NB_SAMPLES]; // Enregistrement du son 
    
    int errInit, 
        errConfig;
    
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

    // Mise a jour du volume des microphones en %
    set_microphones_volume(80, 80);

    // configuration du son: ((sampling_frequency,sample_size = nb of bits,sign: 0 = unsigned,endian : 0 = LSB,channels))
    if((errConfig = kb_sound_configure(FREQUENCE_ENCHANTILLONNAGE, SAMPLE_SIZE, SIGNED, ENDIAN, STEREO)) < 0)
    {
        fprinf(stderr, "Erreur: Impossible de configurer le son. Erreur n°\d!\n", errConfig);
        kb_sound_release();
        return -1;
    }

    printf("\n Enregistrement de %ds -- appuyer sur ENTREE pour commencer\n", LONGUEUR);
    // Sauvegarde du choix
    fgets(line, 80, stdin);

    // Extinction des hauts-parleurs
    mute_speaker(1);
    switch_speakers_ON_OFF(0);

    // Début de l'enregistrement 
    printf("Début de l'enregistement: \n");
    record_buffer(sound, NB_SAMPLES);

    // Extinction des microphones 
    set_microphones_volume(0, 0);
    // et allumage des hauts-parleurs
    switch_speakers_ON_OFF(1);
    mute_speaker(0);

    // Volume des hauts parleurs(g, d)
    set_microphones_volume(80, 80);

    usleep(1000000); // Attendre 1 seconde
    printf("\n Ecoute du son enregistré \n");
    play_buffer(sound, NB_SAMPLES);

    // Attendre la fin
    wait_end_of_play();

    // Sauvegarde dans un fichier
    printf("Sauvegarde dans un fichier : %s\n", FILENAME);
    save_wav_file(FILENAME, sound, NB_SAMPLES, STEREO, SAMPLE_SIZE, FREQUENCE_ENCHANTILLONNAGE);

    end();
    
    return 0;
}
