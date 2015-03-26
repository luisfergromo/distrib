#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int login(User user)
{
	printf("logging in u(%s)\n",user.name);

	//matching user & pass
	int ans		=	FAILURE;
	char *msg	=	"Err: user name not found";

	FILE 		*user_data;
	char 		*user_file 		= "MiniChat_Server/db/users.txt";
	char 		*token_user		= (char*) 	malloc(LENGTH_USER / 2);
	char 		*token_pass		= (char*) 	malloc(LENGTH_USER / 2);
	char 		**next_token 	= (char**) 	malloc(8);
	char		*buffer				= (char*) 	malloc(LENGTH_USER);
	size_t		size=1;
	const char	*delimiter 		= ",";

	user_data= fopen(user_file,"rt");
	if (user_data==NULL)
		msg= "Err: user db couldn't be opened";
	else
	{
		while (fscanf(user_data,"%s\n",buffer) != -1)
		{
			printf("logging in CYCLE\n");
			token_user = strtok_r(buffer, delimiter, next_token);
			token_pass = strtok_r(NULL, delimiter, next_token);

			if( strcmp(token_user,user.name)==EQUAL)
			{
				if(strcmp(token_pass,user.pass)==EQUAL)
				{
					msg= "Successfully logged in";
					ans= SUCCESS;
				}
				else
					msg= "Err: incorrect password";

				break;
			}
		}

		fclose(user_data);
	}

	//return
  user.callback_msg= msg;
	free(buffer);
	return ans;
}

void logout(int num)
{

}
