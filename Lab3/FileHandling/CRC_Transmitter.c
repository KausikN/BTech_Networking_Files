#include<stdio.h>

#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>

int c_socket;

int ClientCreate(int port, int anyip, char IPADDR[])		// Return 1 for error
{
	c_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client;
	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(port);
	if(anyip == 0) client.sin_addr.s_addr = inet_addr(IPADDR);
	else client.sin_addr.s_addr = INADDR_ANY;
	if(connect(c_socket, (struct sockaddr *)&client, sizeof(client)) == -1)
	{
		printf("Connection Issue.\n");
		return 1;
	}
	else return 0;
}

int main()
{
	printf("----------------------CRC TCP/IP Transmitter-----------------------------\n");

	int error = ClientCreate(9009, 1, "");
	if(error == 1)
	{
		close(c_socket);
		printf("Connection Issue.\n");
		return 0;
	}
	else 
	{
		char filename[100];
		char filename_withext[100];

		char text[500];

		printf("Enter filename: ");
		scanf("%s", filename);
		strcpy(filename_withext, filename);
		strcat(filename_withext, ".txt");

		printf("Filename %s.\n", filename_withext);

		FILE *fp;printf("1");
		fp = fopen(filename_withext, "r");printf("2");
		fscanf(fp, "%s", text);

		printf("File %s opened.\n", filename_withext);
/*
		send(c_socket, filename, sizeof(filename), 0);
		send(c_socket, text, sizeof(text), 0);
		printf("Text Sent.\n");*/
	}

	close(c_socket);

	printf("\n---------------------------------------------------------------------\n");	
	return 0;
}

