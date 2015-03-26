#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "user.h"

#define MAX_CLIENT USERS

int totcltes=0;
int client_sockfd[MAX_CLIENT];

void *coneccion_clte(void *arg);

int main() 
{

	int server_sockfd;
	int server_len;
	socklen_t client_len;

	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	int parametro[MAX_CLIENT];
	pthread_t tid[MAX_CLIENT];
	int i;
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(9734);
	server_len = sizeof(server_address);


	char username[LENGTH_USER / 2];
	char password[LENGTH_USER / 2];

	printf("Username: ");
	scanf_s("%c", &username, _countof(username));

	printf("Password: ");
	scanf_s("%c", &password, _countof(password));

	if ( !login(username, password) )
	{
		printf("Usuario o Contraseña Incorrectos");
		exit(0);
	}

	bind(server_sockfd,(struct sockaddr *)&server_address,server_len);
	/* Crear una cola de conexiones */
	listen(server_sockfd, 5); //Cola tamaño 5, solo 6 pueden pasar
	while(totcltes<MAX_CLIENT)
	{
		printf("server esperando, clientes atendidos %d\n",totcltes);
		/* Aceptar conexión */
		client_sockfd[totcltes] = accept(server_sockfd,(struct sockaddr *) &client_address, &client_len);
		/* Crea un hilo que atiende al nuevo cliente */
		parametro[totcltes]=totcltes;
		pthread_create(&tid[totcltes],NULL,coneccion_clte,(void *) &parametro[totcltes]);
		totcltes++;
	}
	for(i=0;i<MAX_CLIENT;i++)
		pthread_join(tid[i],NULL);

}

void *coneccion_clte(void *arg)
{
	int *n=(int *) arg;
	int mynum=*n;
	int length;
	char* text;
	while (1) {
		/* First, read the length of the text message from the socket.
	      if  read returns zero, the client closed the connection. */
		if (read (client_sockfd[mynum], &length, sizeof (length)) == 0)
			return 0;
		/* Allocate a buffer to hold the text. */
		text = (char*) malloc (length);
		/* Read the text itself, and print it. */
		read (client_sockfd[mynum], text, length);
		printf("message from client = %s\n", text);

		//logout
		if (text == "exit") logout();

		//Mandar mensaje de vuelta a todos los demás clientes
		//sleep(5);
		for(int i = 0; i <totcltes; i++)
		 write (client_sockfd[i], text, length);

		if (!strcmp (text, "quit")){
			break;
		}
	}

	close(client_sockfd[mynum]);

}
