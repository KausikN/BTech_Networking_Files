#include<stdio.h>

#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>

int c_socket;

char arr[100];
int arr_size;

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
	printf("----------------------Multi Client Chat Transmitter-----------------------------\n");

	int exit = 0;

	while(exit == 0)
	{
		char name[50];
		printf("Enter Name: ");
		scanf("%s", name);

		char ipaddr[20];
		printf("Enter IP Address: ");
		scanf("%s", ipaddr);

		int port;
		printf("Enter Port: ");
		scanf("%d", &port);

		int error = ClientCreate(port, 0, ipaddr);
		if(error == 1)
		{
			close(c_socket);
			printf("Connection Issue.\n");
			//return 0;
		}
		else 
		{
				char port_str[20];

				int i=0;
				int p = port;
				while(p > 0)
				{
					port_str[i] = (char)(48 + p%10);
					i++;
					p = p/10;
				}

				port_str[i] = '\0';	

				send(c_socket, name, sizeof(name), 0);
				send(c_socket, ipaddr, sizeof(ipaddr), 0);
				send(c_socket, port_str, sizeof(port_str), 0);

				char text[200];

				printf("[+] Server Connected\n\n");

				int end = 0;
				while(end == 0)
				{
					scanf("%s", text);
					send(c_socket, text, sizeof(text), 0);
					if(strcmp(text, "/exit") == 0) end = 1;
				}
				printf("[+] Server Disconnected\n\n");
		}

		printf("Do you want to exit: ");
		scanf("%d", &exit);

		close(c_socket);
	}

	

	printf("\n---------------------------------------------------------------------\n");	
	return 0;
}

