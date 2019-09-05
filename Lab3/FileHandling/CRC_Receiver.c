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
		//printf("\nError in Bind.\n");
		return 1;
	} 
}

int main()
{
	printf("----------------------CRC TCP/IP Receiver-----------------------------\n");

	int error = ServerCreate(9009);
	if(error == 1)
	{
		close(s_server);
		close(s_socket);
		printf("Server Binding Issue.\n");
		return 0;
	}
	else 
	{
		printf("\nServer Started...\n");

		char filename[500];
		char filename_withext[100];
		char text[500];

		recv(s_server, filename, sizeof(filename), 0);
		recv(s_server, text, sizeof(text), 0);

		strcpy(filename_withext, filename);
		strcat(filename_withext, "_recv.txt");

		FILE *fp;
		fp = fopen(filename, "w");
		fprintf(fp, "%s", text);
		fclose(fp);

		printf("File %s Received Succesfully.\n", filename);
	}

	close(s_server);
	close(s_socket);

	printf("\n---------------------------------------------------------------------\n");	
	return 0;
}

