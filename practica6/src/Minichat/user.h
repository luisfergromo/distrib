//omar, federico, robina

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USERS 10 //Número de usuarios
#define LENGTH_USER 500 //Lognitud de acceso de usuario incluyendo username y password

int login(char username[], char password[])
{

	FILE *user_data;
	char* db = "users.log";
	unsigned char buffer[2048]; // Buffer de 2 Kbytes
	int bytesLeidos;

	errno_t capture_status_log;

	// Abrir el fichero de entrada en lectura y binario
	capture_status_log = fopen_s(&user_data, "users.log", "r");
	if (capture_status_log != 0)
	{
		printf("Error al abrir el log de usuarios.\n", NULL);
		scanf_s("%s", NULL);
		return 0;
	}

	char read_user_data[LENGTH_USER];
	char info_file[USERS][LENGTH_USER];

	int i;

	for (i = 0; fgets(read_user_data, LENGTH_USER, user_data) != NULL; i++)
		if (i < USERS) strcpy_s(info_file[i], read_user_data);

	printf("Es: %s", info_file[1]);

	//Comprobar usuario
	char *token_user;
	char *next_token = NULL;

	for (i = 0; i < USERS; i++)
	{
		token_user = strtok_s(info_file[i], "|", &next_token);
		if ((strcmp(&token_user[0], username) == 0) && (strcmp(&token_user[1], password) == 0))
			return 1;
	}

	fclose(user_data);

	scanf_s("%s", NULL);

	return 0;

}

void logout()
{
	exit(0);
}