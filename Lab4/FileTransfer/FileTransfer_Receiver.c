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
	printf("----------------------File Transfer TCP/IP Receiver-----------------------------\n");

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

		char filename[100];
		char filename_withext[100] = "";
		char ext[10];

		char buffer[2000];
		char charbuf[1];

		recv(s_server, filename, sizeof(filename), 0);
		recv(s_server, ext, sizeof(ext), 0);

		if(strlen(ext) == 0) return 0;

		strcat(filename_withext, filename);
		strcat(filename_withext, "_recv.");
		strcat(filename_withext, ext);
		
		FILE *fp;
		fp = fopen(filename_withext, "w");

		char endoffile[1] = "0";

		int check = 0;
		while(check == 0)
		{
			/*
			recv(s_server, buffer, sizeof(buffer), 0);

			//printf("buf: %s\n", buffer);

			if(!strcmp(buffer, "end")) check = 1;
			else fprintf(fp, "%s ", buffer);
			*/

			recv(s_server, endoffile, sizeof(endoffile), 0);
			if(endoffile[0] == '1') check = 1;
			else 
			{
				recv(s_server, charbuf, sizeof(charbuf), 0);
				//printf("buf: %s\n", charbuf);
				fprintf(fp, "%c", charbuf[0]);
			}

			//printf("buf: %s\n", charbuf);
			//if(charbuf[0] == '\b') check = 1;
			//else fprintf(fp, "%c", charbuf[0]);

		}
		printf("\n\n File Ended \n\n");

		fclose(fp);
		
		printf("File %s Received Succesfully.\n", filename_withext);
	}

	close(s_server);
	close(s_socket);

	printf("\n---------------------------------------------------------------------\n");	
	return 0;
}

