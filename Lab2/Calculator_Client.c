#include<stdio.h>

#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>

int c_socket;

int main()
{
	printf("------------------------CALCULATOR---------------------------\n");

	//Create Server
	int errors = ClientCreate(9009);
	if(errors == 1)return 0;

	//init vars
	char choice[] = {"0"};

	int x;
	int y;

	float ans;


	//Choose Operation
	char text[500];
	recv(c_socket, text, sizeof(text), 0);
	printf("%s\n", text);
	scanf("%s", choice);
	send(c_socket, choice, sizeof(choice), 0);

	if(choice[0] == '4')
	{
		printf("Exiting CALC.\n");
		close(c_socket);
		return 0;
	}

	//Give Inputs

	//x
	char text2[500];
	recv(c_socket, text2, sizeof(text2), 0);
	printf("%s", text2);
	scanf("%d", &x);
	send(c_socket, &x, sizeof(x), 0);
	//y
	char text3[500];
	recv(c_socket, text3, sizeof(text3), 0);
	printf("%s", text3);
	scanf("%d", &y);
	send(c_socket, &y, sizeof(y), 0);

	//Give Output
	recv(c_socket, &ans, sizeof(ans), 0);
	printf("\nAnswer: %f", ans);


	printf("\n------------------------------------------------------------\n");	
	return 0;
}

int ClientCreate(int port)		// Return 1 for error
{
	c_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client;
	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(port);
	client.sin_addr.s_addr = inet_addr("192.168.43.229");
	//client.sin_addr.s_addr = INADDR_ANY;
	if(connect(c_socket, (struct sockaddr *)&client, sizeof(client)) == -1)
	{
		printf("Connection Issue.\n");
		return 1;
	}
	else return 0;
}
