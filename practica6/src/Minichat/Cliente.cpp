#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <string.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main()
{
	int sockfd;
	int len;
	struct sockaddr_in address;
	int result;
	/* Crear un socket para el cliente */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	/* Nombrar el socket, de acuerdo con el server */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(9734);
	len = sizeof(address);
	result = connect(sockfd, (struct sockaddr *)&address, len);
	if(result == -1) {
		perror("oops: client1");
		exit(1);
	}
	char text[1024];
	int length;
	while(1){
		printf("\nMensaje:");
		//text = "hey jude";
		//fgets(text,1024 ,stdin);
		//gets(text);
		cin >> text;
		length = strlen (text) + 1;
		//write size
		write (sockfd, &length, sizeof (length));

		/* Write the string. */
		write (sockfd, text, length);
		if(read (sockfd, &length, sizeof (length))){
			read(sockfd, text, length);
			printf("\nstring from server = %s", text);
		}
	}
	//read(sockfd, text, length);
	//printf("string from server = %s\n", ch);
	close(sockfd);
	exit(0);
}
