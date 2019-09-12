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

	int ex2 = 0;

	int cur_process = 1;	// 0 - child		1 - parent

	while(ex2 == 0 && cur_process != 0)
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

			int recv_mode = 0;

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


			char dest_name[50];
			printf("Enter Dest Name: ");
			scanf("%s", dest_name);
			send(c_socket, dest_name, sizeof(dest_name), 0);

			int recvonly = 0;

			if(strcmp(dest_name, "_") == 0)
			{
				recvonly = 1;
			}

			char text[200];

			printf("[+] Server Connected\n\n");

			int pidtr = fork();

			if(pidtr == 0 && recvonly == 0)
			{
				printf("Enter Text to send: \n");
				int end = 0;
				while(end == 0)
				{
					scanf("%s", text);
					send(c_socket, text, sizeof(text), 0);
					if(strcmp(text, "/exit") == 0) end = 1;
				}
				exit(0);
				return 0;
			}
			else if(pidtr > 0)
			{
				printf("\n(Text Receiver Started)\n");
				char src_name[50];
				while(1)
				{
					recv(c_socket, src_name, sizeof(src_name), 0);
					recv(c_socket, text, sizeof(text), 0);
					printf("\nReceived from %s: %s\n", src_name, text);
					if(strcmp(text, "/exit") == 0)
					{
						break;
					}
				}
				printf("\n(Text Receiver Stopped)\n");

				printf("[+] Server Disconnected\n\n");
				printf("Do you want to ex2: ");
				scanf("%d", &ex2);

				close(c_socket);
				printf("\n---------------------------------------------------------------------\n");
			}
			cur_process = pidtr;
			printf("EXITSTAT: %d\n", pidtr);
			
		}
	}

		
	return 0;
}

