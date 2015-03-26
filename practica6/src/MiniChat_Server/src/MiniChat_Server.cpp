#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <pthread.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>

#include "def.h"
#include "chat_util.h"
#include "login.h"

using namespace std;

Server server;

void *coneccion_clte(void *arg);
char* getMsg(int nclient);
void sendMsg(int nclient, char *text);

int main()
{
	server= openChat(9734, MAX_CLIENT);

	while(server.totcltes<MAX_CLIENT)
	{
		printf("server esperando, clientes atendidos %d\n",server.totcltes);
		//Aceptar conexión
		server.client[server.totcltes].sockfd = accept(server.server_sockfd,(struct sockaddr *) &server.client_address, &server.client_len);
		//Crea un hilo que atiende al nuevo cliente
		server.client[server.totcltes].param=server.totcltes;
		pthread_create(&server.client[server.totcltes].tid,NULL,coneccion_clte,(void *) &server.client[server.totcltes].param);
		server.totcltes++;
	}
	for(int i=0;i<MAX_CLIENT;i++)
		pthread_join(server.client[i].tid,NULL);

}

void connectionRequest(int nclient)
{
	//client session request
	User u;

	sendMsg(nclient,"Username:");
	u.name= getMsg(nclient);

	sendMsg(nclient,"Password:");
	u.pass= getMsg(nclient);

	if ( login(u) == FAILURE )
		exit(0);
}

void *coneccion_clte(void *arg)
{
	int *n=(int *) arg;
	int mynum=*n;
	int length;
	char* text;

	connectionRequest(mynum);

	while (1)
	{
		text= getMsg(mynum);

		if(server.client[mynum].state==CONNECTION_CLOSED)
			break;

		if(strcmp (text, "exit") == EQUAL)
			break;
		else
			//Mandar mensaje de vuelta a todos los demás clientes
			for(int i = 0; i <server.totcltes; i++)
			{
				write (server.client[i].sockfd, &length, sizeof (length));
				write (server.client[i].sockfd, text, length);
			}
	}

	close(server.client[mynum].sockfd);
	logout(mynum);
	cout << "connection closed for client " << mynum << endl;
	return 0;

}

char* getMsg(int nclient)
{
	int 	length;
	char	*text;

	if (read (server.client[nclient].sockfd, &length, sizeof (length)) == 0)
	{
		cout << "connection closed for client " << nclient << endl;
		server.client[nclient].state=CONNECTION_CLOSED;
	}
	else
	{
		text = (char*) malloc (length);
		read (server.client[nclient].sockfd, text, length);
	}

	return text;
}

void sendMsg(int nclient, char *text)
{
	int length = strlen (text) + 1;
	write (server.client[nclient].sockfd, &length, sizeof (length));
	write (server.client[nclient].sockfd, text, length);
}
