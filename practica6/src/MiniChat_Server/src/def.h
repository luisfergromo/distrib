#define TRUE 1
#define FALSE 0
#define SUCCESS 1
#define FAILURE 0
#define EQUAL 0

#define CONNECTION_CLOSED 5

#define USERS 10 //Numero de usuarios
#define MAX_CLIENT USERS
#define LENGTH_USER 500 //Longitud de acceso de usuario incluyendo username y password

struct User
{
	char *name;
	char *pass;
	char *callback_msg;
};

struct Client
{
	int 				sockfd;
	int 				param;
	pthread_t 	tid;
	int 				state;
};

struct Server
{
	int 						server_sockfd;
	struct 					sockaddr_in 	client_address;
	socklen_t 			client_len;

	int totcltes;
	Client					client[MAX_CLIENT];

	//int client_sockfd	[MAX_CLIENT];

	//int parametro[MAX_CLIENT];
	//pthread_t tid[MAX_CLIENT];
	//int connection_map[MAX_CLIENT];

};
