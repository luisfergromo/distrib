struct User
{
  char *name;
  char *pass;
};

struct Client
{
  int 				sockfd;
  int 				id;
  pthread_t 	tid;

  struct      User u;
  char*       callback_msg;
  char*       in_buffer;
  char*       out_buffer;
};

struct Server
{
  int 						server_sockfd;
  struct 					sockaddr_in 	client_address;
  socklen_t 			client_len;

  int 						totcltes;
  int             stateMap[MAX_CLIENT];
  Client					client[MAX_CLIENT];

};
