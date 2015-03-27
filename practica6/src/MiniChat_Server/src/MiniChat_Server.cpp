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
#include "structs.h"

Server startServer(int port);
void*	coneccion_clte(void *arg);
void connectionRequest(Client* cl);
char* getMsg(Client* cl);
void sendMsg(Client* cl);
void sendMsg(Client* cl, char *msg);
void multicastMsg(Client* cl, char *msg);
void login(Client* cl);
void logout(Client* cl);

using namespace std;

Server server;

int main()
{
	server= startServer(9734);

	while(server.totcltes<MAX_CLIENT)
	{
		printf("server esperando, clientes atendidos %d\n",server.totcltes);
		//Aceptar conexión
		server.client[server.totcltes].sockfd = accept(server.server_sockfd,(struct sockaddr *) &server.client_address, &server.client_len);
		//Crea un hilo que atiende al nuevo cliente
		server.client[server.totcltes].id=server.totcltes;
		pthread_create(&server.client[server.totcltes].tid,NULL,coneccion_clte,(void *) &server.client[server.totcltes].id);
		server.totcltes++;
	}

	for(int i=0;i<MAX_CLIENT;i++)
		pthread_join(server.client[i].tid,NULL);

}

Server startServer(int port)
{
	Server server;
	server.totcltes=0;

	if((server.server_sockfd = socket(AF_INET, SOCK_STREAM, 0))==-1)
	{
		printf("Err: could not open socket\n");
		exit(0);
	}

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(port);

	bind(server.server_sockfd,(struct sockaddr *)&server_address,sizeof(server_address));

	listen(server.server_sockfd, MAX_CLIENT);

	return server;
}

void *coneccion_clte(void *arg)
{
	int *n=(int *) arg;
	int mynum=*n;
	int length;

	Client cl = server.client[mynum];
	connectionRequest(&cl);

	if (server.stateMap[mynum] == CONNECTION_OPENED )
	while (1)
	{
		cl.in_buffer= getMsg(&cl);

		if(server.stateMap[mynum] == CONNECTION_CLOSED)
			break;

		if(strcmp (cl.in_buffer, "exit") == EQUAL)
			break;
		else
			multicastMsg(&cl, cl.in_buffer);

		free(cl.in_buffer);
	}

	close(cl.sockfd);
	logout(&cl);
	cout << "connection closed for client " << mynum << endl;
}

void connectionRequest(Client* cl)
{
	sendMsg(cl,"Username:");
	cl->u.name= getMsg(cl);

	sendMsg(cl,"Password:");
	cl->u.pass= getMsg(cl);

	login(cl);

	cout << cl->callback_msg << endl;
	sendMsg(cl,cl->callback_msg);
}

char* getMsg(Client* cl)
{
	int 	length;
	char* msg;

	if ( read(cl->sockfd, &length, sizeof (length)) == 0)
		server.stateMap[cl->id]=CONNECTION_CLOSED;
	else
	{
		msg = (char*) malloc (length);
		read( cl->sockfd, msg, length);
	}

	return msg;
}

void sendMsg(Client* cl)
{
	int length = strlen (cl->out_buffer) + 1;
	write (cl->sockfd, &length, sizeof (length));
	write (cl->sockfd, cl->out_buffer, length);
}


void sendMsg(Client* cl, char *msg)
{
	cl->out_buffer= msg;
	sendMsg(cl);
}

void multicastMsg(Client* cl, char *msg)
{
	int length = strlen(msg)+strlen(cl->u.name)+3;

	char *line= (char*) malloc(length);
	strcat(line, cl->u.name);
	strcat(line, ": ");
	strcat(line, msg);

	for(int i = 0; i <MAX_CLIENT; i++)
		if(server.stateMap[i]==CONNECTION_OPENED &&
			server.client[i].id != cl->id)
		{
			write (server.client[i].sockfd, &length, sizeof (length));
			write (server.client[i].sockfd, line, length);
		}
}

void login(Client* cl)
{
	printf("logging in u(%s)\n",cl->u.name);

	server.stateMap[cl->id]		=	CONNECTION_CLOSED;
	cl->callback_msg		=	"Err: user name not found";

	FILE 				*user_data;
	char 				*user_file 		= "MiniChat_Server/db/users.txt";
	char 				*token_user		= (char*) 	malloc(LENGTH_USER / 2);
	char 				*token_pass		= (char*) 	malloc(LENGTH_USER / 2);
	char 				**next_token 	= (char**) 	malloc(8);
	char				*buffer				= (char*) 	malloc(LENGTH_USER);
	size_t			size=1;
	const char	*delimiter 		= ",";

	user_data= fopen(user_file,"rt");
	if (user_data==NULL)
		cl->callback_msg= "Err: user db couldn't be opened";
	else
	{
		while (fscanf(user_data,"%s\n",buffer) != -1)
		{
			token_user = strtok_r(buffer, delimiter, next_token);
			token_pass = strtok_r(NULL, delimiter, next_token);

			if( strcmp(token_user,cl->u.name)==EQUAL)
			{
				if(strcmp(token_pass,cl->u.pass)==EQUAL)
					{
						cl->callback_msg= "Successfully logged in";
						server.stateMap[cl->id]		= CONNECTION_OPENED;
					}
					else
						cl->callback_msg= "Err: incorrect password";
						break;
					}
			}

		fclose(user_data);
	}

	free(buffer);
}

void logout(Client* cl)
{
	server.stateMap[cl->id]		=	CONNECTION_CLOSED;
}
