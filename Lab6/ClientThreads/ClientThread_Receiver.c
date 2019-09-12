#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

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
		
		//int accepterror = AcceptNewClient();
		//return accepterror;
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
		printf("\n[+] Conection accepted from %s,%d\n",inet_ntoa(other.sin_addr),ntohs(other.sin_port));
		return 0;
	}
}


int main()
{
	printf("----------------------Client Threading Receiver-----------------------------\n");

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
						char ipaddr[20];
						char port_str[20];

						recv(s_server, ipaddr, sizeof(ipaddr), 0);
						recv(s_server, port_str, sizeof(port_str), 0);

						if(strcmp(ipaddr, "sent") != 0 && strcmp(ipaddr, "ex") != 0)
						{
							printf("\n[+] Conection accepted from %s at port %s.\n",ipaddr, port_str);
							printf("Ip Address: %s\nPort: %s\n\n", ipaddr, port_str);
							exit(0);
						}
						else if(strcmp(ipaddr, "ex") == 0)
						{
							exit(0);
							break;
						}
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

