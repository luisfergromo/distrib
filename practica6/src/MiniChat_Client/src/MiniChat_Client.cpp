#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <pthread.h>

using namespace std;

void openClient(int port, char* addr);
void *inListener(void *arg);
void outListener();
int sockfd;

int main()
{
	openClient(9734,"127.0.0.1");
}

void openClient(int port, char* addr)
{
	// Crear un socket para el cliente
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// Nombrar el socket, de acuerdo con el server
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(addr);
	address.sin_port = htons(port);

	// Establecer coneccion
	if(connect(sockfd, (struct sockaddr *)&address, sizeof(address)) == -1)
	{
		perror("oops: connection refused");
		exit(1);
	}

	//launch in/out listeners in different threads
	pthread_t tid;
	char *arg="";
	pthread_create(&tid,NULL,inListener,NULL);

	outListener();
}

void *inListener(void *arg)
{
	char 	*text;
	int 	length;

	while(1)
	{
		if(read (sockfd, &length, sizeof (length))==0)
		{
			cout << endl << "connection closed" << endl;
			exit(0);
		}
		else
		{
			text = (char*) malloc (length);
			read(sockfd, text, length);
			cout << text << endl;

			free(text);
			cout << "> ";
		}
	}

	close(sockfd);
	exit(0);
}

void outListener()
{
	char text[1024];
	int length;

	while(1)
	{
		cout << "> ";
		cin >> text;
		length = strlen (text) + 1;

		write (sockfd, &length, sizeof (length));
		write (sockfd, text, length);
	}
	close(sockfd);
	exit(0);
}
