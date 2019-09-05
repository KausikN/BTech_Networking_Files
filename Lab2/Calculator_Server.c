#include<stdio.h>

#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>

int s_socket, s_server;

int ServerCreate(int port)		// Return 1 for error
{
	s_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server, other;
	memset(&server, 0, sizeof(server));
	memset(&other, 0, sizeof(other));

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	
	if(bind(s_socket, (struct sockaddr*)&server, sizeof(server)) != -1)
	{
		printf("Server Running.....\n");
		listen(s_socket, 5);
		socklen_t add;
		add = sizeof(other);
		s_server = accept(s_socket, (struct sockaddr*)&other, &add);
		return 0;
	}
	else
	{
		printf("\nError in Bind.\n");
		return 1;
	} 
}
/*
int StrToInt(char c[])
{
	int val = 0;
	int negative = 0;

	int i=0;
	if(c[0] == '-')
	{
		negative = 1;
		i = 1;
	}

	while(1)
	{
		if((int)(c[i]) >= 48 && (int)(c[i]) <= 57)
		{
			val = (10*val) + ((int)c[i]) - 48;
		}
		else if(c[i] = '\0') break;
		else return val;	// Marker Value

		i++;
	}

	if(negative == 1) val = -1 * val;

	return val;
}
*/
int main()
{
	printf("------------------------CALCULATOR SERVER---------------------------\n");

	//Create Server
	int errors = ServerCreate(9009);
	if(errors == 1)return 0;

	//init vars
	char choice[] = {"0"};
	int x = 0, y = 0;
	float ans = 0;


	//Choose Operation
	char text1[] = {"What operation do you want?\n Addition\t-\t0\n Subtraction\t-\t1\n Multiplication\t-\t2\n Division\t-\t3\n Exit\t-\t4\nEnter choice: "};
	send(s_server, text1, sizeof(text1), 0);
	recv(s_server, choice, sizeof(choice), 0);

	if(choice[0] == '4')
	{
		printf("\nExiting CALC.\n");

		close(s_server);
		close(s_socket);

		return 0;
	}

	printf("\nUser Entered Choice %c.\n", choice[0]);

	//Give Inputs

	char text2[] = {"Enter x: "};
	send(s_server, text2, sizeof(text2), 0);
	recv(s_server, &x, sizeof(x), 0);

	char text3[] = {"Enter y: "};
	send(s_server, text3, sizeof(text3), 0);
	recv(s_server, &y, sizeof(y), 0);

	//Do Operation

	if(choice[0] == '0') 		ans = x + y;
	else if(choice[0] == '1') 	ans = x - y;
	else if(choice[0] == '2') 	ans = x * y;
	else if(choice[0] == '3') 	ans = x / y;

	//Give Output

	printf("\nx: %d, y: %d, ans: %f\n", x, y, ans);

	send(s_server, &ans, sizeof(ans), 0);


	printf("\n------------------------------------------------------------\n");	
	return 0;
}