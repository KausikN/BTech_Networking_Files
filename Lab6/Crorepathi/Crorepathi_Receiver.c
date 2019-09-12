#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include <arpa/inet.h>

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

		return 0;
	}
	else
	{
		//printf("\nError in Bind.\n");
		return 1;
	} 
}

int AcceptNewClient()
{
	struct sockaddr_in other;
	memset(&other, 0, sizeof(other));
	socklen_t add = sizeof(other);

	s_server = accept(s_socket, (struct sockaddr*)&other, &add);printf("Hi5\n");
	if(s_server == -1) return 1;
	else 
	{	
		printf("\n[+] Conection accepted from %s,%d\n",inet_ntoa(other.sin_addr),ntohs(other.sin_port));
		return 0;
	}
}


int main()
{
	printf("----------------------Multi Client Chat Receiver-----------------------------\n");

	int ex = 0;

	while(ex == 0)
	{
		int port;
		printf("Enter Port: ");
		scanf("%d", &port);

		int error = ServerCreate(port);
		if(error == 1)
		{
			close(s_server);
			close(s_socket);
			printf("Server Issue.\n");
			return 0;
		}
		else 
		{
			printf("\nServer Waiting...\n");

			while(1)
			{
				int accepterror = AcceptNewClient();
				if(accepterror == 0)
				{
					int pid = fork();
					if(pid == 0)
					{
						char name[50];
						char ipaddr[20];
						char port_str[20];

						char text[200];

						recv(s_server, name, sizeof(name), 0);
						recv(s_server, ipaddr, sizeof(ipaddr), 0);
						recv(s_server, port_str, sizeof(port_str), 0);

						printf("Name: %s\tIp Address: %s\tPort: %s\n\n", name, ipaddr, port_str);

						int end = 0;
						while(end == 0)
						{
							recv(s_server, text, sizeof(text), 0);

							if(strcmp(text, "/exit") == 0)
							{
								printf("Client %s logged off.\n", name);
								end = 1;
								break;
							}
							else 
							{
								printf("%s\n", text);
							}
						}
						close(s_server);
						exit(0);
						return 0;
					}
				}
			}
		}

		close(s_server);
		close(s_socket);

		printf("Do you want to ex: ");
		scanf("%d", &ex);
	}

	

	printf("\n---------------------------------------------------------------------\n");	
	return 0;
}

