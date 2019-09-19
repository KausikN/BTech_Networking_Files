#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include <arpa/inet.h>

int s_socket, s_server;
char clientip[20];

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

	s_server = accept(s_socket, (struct sockaddr*)&other, &add);
	if(s_server == -1) return 1;
	else 
	{	
		strcpy(clientip, inet_ntoa(other.sin_addr));
		printf("\n[+] Conection accepted from %s,%d\n",inet_ntoa(other.sin_addr),ntohs(other.sin_port));
		return 0;
	}
}

struct Client
{
	char name[50];
	char ipaddr[20];
	char port_str[20];

	int s_server;
};

int FindClient(struct Client clients[], int clients_size, char name[])
{
	for(int i=0;i<clients_size;i++)
	{
		if(strcmp(clients[i].name, name) == 0)
		{
			return i;
		}
	}
	return -1;
}

void DisplayConnectedClients(struct Client clients[], int clients_size)
{
	printf("\nConnected Clients: ");
	for(int i=0;i<clients_size;i++)
	{
		printf("%s, ", clients[i].name);
	}
	printf("\n");
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

			// Common Vars
			struct Client clients[10];
			int clients_size = 0;
			// Common Vars

			while(1)
			{ 
				printf("Ready to accept new client...\n");
				int accepterror = AcceptNewClient();
				if(accepterror == 0)
				{

					int pid = fork();
					if(pid > 0)
					{
						//return 0;
						continue;
					} 
					else if(pid == 0)
					{
						int s_server_child = s_server;
						int clientindex = clients_size;
						clients[clientindex].s_server = s_server_child;
						strcpy(clients[clientindex].ipaddr, clientip);

						char name[50];
						char ipaddr[20];
						char port_str[20];
						char dest_name[50];

						char text[200];

						recv(s_server_child, name, sizeof(name), 0);
						recv(s_server_child, ipaddr, sizeof(ipaddr), 0);
						recv(s_server_child, port_str, sizeof(port_str), 0);
						

						strcpy(clients[clientindex].name, name);
						//strcpy(clients[clientindex].ipaddr, ipaddr);
						
						strcpy(clients[clientindex].port_str, port_str);
						clients_size++;

						DisplayConnectedClients(clients, clients_size);
						/*
						int pidnextclient = fork();
						if(pidnextclient > 0)
						{
							continue;
						}
						*/
						recv(s_server_child, dest_name, sizeof(dest_name), 0);

						if(strcmp(dest_name, "_") == 0)
						{
							
						}
						else 
						{
							
							int destindex = FindClient(clients, clients_size, dest_name);

							if(destindex == -1) printf("Dest %s not found.\n", dest_name);
							else 
							{
								printf("Name: %s\tIp Address: %s\tPort: %s\nConnecting to Name: %s\tIp Address: %s\tPort: %s\n", name, clientip , port_str, clients[destindex].name, clients[destindex].ipaddr, clients[destindex].port_str);

								int end = 0;
								while(end == 0)
								{
									recv(s_server_child, text, sizeof(text), 0);

									if(strcmp(text, "/exit") == 0)
									{
										printf("Client %s logged off.\n", name);
										end = 1;
										break;
									}
									else 
									{
										printf("%s\n", text);
										send(clients[destindex].s_server, name, sizeof(name), 0);
										send(clients[destindex].s_server, text, sizeof(text), 0);
									}
								}
							}
						}

						
						close(s_server_child);
						exit(0);
						//return 0;
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

