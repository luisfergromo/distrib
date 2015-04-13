#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <pthread.h>

#define SRV_IP "127.0.0.1"//"192.168.56.1"
#define BUFLEN 512
#define PORT 9931

using namespace std;

int sockfd, result;
socklen_t slen;
struct sockaddr_in si_other;

int msgCount = 0;
char src[4];

void initSession();
void *inListener(void *arg);
void print(char* xmlElement, char* msg);
void outListener();
void send(int id, int count, char *dest, char *msg);

void diep(char *s)
{
	perror(s);
	exit(1);
}

int main(void)
{
	slen= sizeof(si_other);
	// Crear el socket
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockfd == -1)
		diep("socket");

	memset((char*) &si_other, 0, sizeof(si_other));
	// Nombrar el socket
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	result=inet_aton(SRV_IP, &si_other.sin_addr);
	if(result==0)
		diep("inet_aton() failed\n");

	std::cout << "Client up in port " << PORT << endl;
	initSession();
	pthread_t tid;
	pthread_create(&tid,NULL,inListener,NULL);
	outListener();
	std::cout << "Client down" << endl;

	close(sockfd);
	return 0;
}

void initSession()
{
		std::cout << "Welcome, enter your nickname: " ;
		std::cin >> src;
		send(-1, 0, "0", "LOGIN_REQUEST");
}

void *inListener(void *arg)
{
	char bufIn[BUFLEN];

	while(true)
	{
		result=recvfrom(sockfd, bufIn, BUFLEN, 0, (struct sockaddr*) &si_other,&slen);
		if(result==-1)
			diep("recvfrom()");

		print("src",bufIn);
		std::cout << ": ";
		print("text",bufIn);
		std::cout << endl;
	}

		close(sockfd);
		exit(0);
}

void print(char* xmlElement, char* msg)
{
	int size= strlen(xmlElement);
	char openS[size+2];
	char closeS[size+3];

	sprintf(openS,"<%s>",xmlElement);
	sprintf(closeS,"</%s>",xmlElement);

	char *init=	strstr(msg, openS)+strlen(openS);
	char *end=	strstr(init, closeS);

	//std::cout << init;
	while(init != end)
	{
		std::cout << init[0];
		init++;
	}
}

void outListener()
{
	char msg[BUFLEN];

	while(true)
	{
		std::cin >> msg;

		char *split = strchr(msg, ':');
		if(split)//specific destinatary
		{
			int n= split - msg;
			char dest[n];
			for(int i=0;i<n;i++)
				dest[i]= msg[i];
			
			split++;
			send(msgCount, 0, dest, split);
		}
		else//broadcast
			send(msgCount, 0, "0", msg);

		msgCount++;
	}

	close(sockfd);
	exit(0);
}

void send(int id, int count, char *dest, char *msg)
{
	char bufOut[BUFLEN];
	int size= strlen(msg);

	sprintf(bufOut,
	"<socket>"
	"<id>%d</id>"
	"<count>%d</count>"
	"<size>%d</size>"
	"<src>%s</src>"
	"<dest>%s</dest>"
	"<text>%s</text>"
	"</socket>",id,count,size, src, dest, msg);

	if(sendto(sockfd, &bufOut, BUFLEN, 0, (struct sockaddr*) &si_other, slen)==-1)
		diep("sendto( )");
}
/*
const char* pack(char* text, int id)
{
	char* bufOut= "";
	char* name = "rominita";

	int size_message = strlen(text);
	int count_message = 0;
	char* divide;

	if(size_message > 512)
	{
		for(int i = 0; (size_message/2); i++)
			divide[i] = text[i];

		count_message++;
	}
	else divide = text;

	sprintf(bufOut, "<socket>"
	"<id> %d </id>"
	"<count> %d </count>"
	"<size> %d </size>"
	"<src> %s </src>"
	"<dest> %s </dest>"
	"<text> %s </text>"
	"</socket>",id,count_message,1, name,"0",divide);

	std::cout << bufOut << endl;
	return bufOut;
}
*/
