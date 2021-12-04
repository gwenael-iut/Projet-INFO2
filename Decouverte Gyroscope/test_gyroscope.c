/* ----------------------------------------------- */
/* -------        PROGRAMME DE TEST        ------- */
/* -------                                 ------- */
/* ----------------------------------------------- */
#include <khepera/khepera.h>

#define TAILLE 100
static knet_dev_t *dsPic;

int main(int argc, char *argv[])
{
	int i = 0,
		rc;

	double valCourante,
		valSuiv;

	char toucheEnfoncee,
		buffer[TAILLE];

	/* Initialisation de la librairie khepera et la connexion au robot */
	if ((rc = kb_init(argc, argv)) < 0)
	{
		printf("\nERREUR: Echec de l'initialisation de la librairie !\n\n");
		return 1;
	}

	/* Initialisation de la connexion au robot */
	dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
	if (dsPic == NULL)
	{
		printf("\nERREUR: Echec de la communication avec le robot (Kh4 dsPic)\n\n");
		return -2;
	}

	printf("Test du gyroscope.");
	printf("Quel mode souhaitez-vous choisir ?\n");
	printf("a -> La moyenne des valeurs récupérées par le gyroscope\n");
	printf("b -> Afficher les 10 valeurs récupérées par le gyroscope et leur moyenne\n");

	toucheEnfoncee = getchar();

	switch (toucheEnfoncee)
	{

	case 'a':
		while (!kb_kbhit())
		{
			usleep(200000);
			kb_clrscr();
			// Récupération des valeurs pour chaque axe X Y Z dans un tableau
			kh4_measure_gyro(buffer, dsPic);
			// Remise à zéro de la valeur courante
			valCourante = 0;
			for (i = 0; i < 10; i++) valCourante += ((short)((buffer[i * 2] | buffer[i * 2 + 1] << 8))) * KH4_GYRO_DEG_S;
			printf("Moyenne des 10 valeurs de X :\n ");
			printf("%6.1f\n", valCourante / 10.0);

			// Remise à zéro de la valeur courante
			valCourante = 0;
			for (i = 10; i < 20; i++) valCourante += ((short)((buffer[i * 2] | buffer[i * 2 + 1] << 8))) * KH4_GYRO_DEG_S;
			printf("Moyenne des 10 valeurs de Y :\n ");
			printf("%6.1f\n", valCourante / 10.0);

			// Remise à zéro de la valeur courante
			valCourante = 0;
			for (i = 20; i < 30; i++) valCourante += ((short)((buffer[i * 2] | buffer[i * 2 + 1] << 8))) * KH4_GYRO_DEG_S;
			printf("Moyenne des 10 valeurs de Z :\n ");
			printf("%6.1f\n", valCourante / 10.0);
		}
		break;

	case 'b':
		while (!kb_kbhit())
		{
			kb_clrscr();
			// Récupération des valeurs pour chaque axe X Y Z dans un tableau
			kh4_measure_gyro(buffer, dsPic);

			// Affichage des 10 valeurs récupérées par le gyroscope et leur moyenne pour chaque axe X Y Z
			printf("Valeurs de X [deg/s] :\n ");
			valCourante = 0;

			// Les valeurs pour l'axe X sont stockées de l'indice 0 à l'indice 9 du buffer
			for (i = 0; i < 10; i++)
			{
				// Décalage binaire puis conversion en deg/s
				valSuiv = ((short)((buffer[i * 2] | buffer[i * 2 + 1] << 8))) * KH4_GYRO_DEG_S;
				printf("%6.1f\n", valSuiv);
				valCourante += valSuiv;
			}
			printf("Moyenne des 10 valeurs de X :\n ");
			printf("%6.1f\n", valCourante / 10.0);

			printf("Valeurs de Y [deg/s] :\n ");
			valCourante = 0;

			// Les valeurs pour l'axe Y sont stockées de l'indice 10 à l'indice 19 du buffer
			for (i = 10; i < 20; i++)
			{
				// Décalage binaire puis conversion en deg/s
				valSuiv = ((short)((buffer[i * 2] | buffer[i * 2 + 1] << 8))) * KH4_GYRO_DEG_S;
				printf("%6.1f\n", valSuiv);
				valCourante += valSuiv;
			}
			printf("Moyenne des 10 valeurs de Y :\n ");
			printf("%6.1f\n", valCourante / 10.0);

			printf("Valeurs de Z [deg/s] :\n ");
			valCourante = 0;

			// Les valeurs pour l'axe Z sont stockées de l'indice 20 à l'indice 29 du buffer
			for (i = 20; i < 30; i++)
			{
				// Décalage binaire puis conversion en deg/s
				valSuiv = ((short)((buffer[i * 2] | buffer[i * 2 + 1] << 8))) * KH4_GYRO_DEG_S;
				printf("%6.1f\n", valSuiv);
				valCourante += valSuiv;
			}
			printf("Moyenne des 10 valeurs de Z :\n ");
			printf("%6.1f\n", valCourante / 10.0);

			usleep(1000000); // Attente de 1s avant de réactualiser les valeurs
		}
		break;
	}
	return 0;
}
