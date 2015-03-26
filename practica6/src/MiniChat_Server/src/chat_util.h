#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Server openChat(int port, int maxConnections)
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

	listen(server.server_sockfd, maxConnections);

	return server;
}
