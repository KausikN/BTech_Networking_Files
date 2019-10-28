#include<stdio.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

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

// ----------------------------------------------------------------------------------
void * ReceiverThread(void * arg)
{
	printf("\n(Text Receiver Started)\n");
	char src_name[50];
	char text[200];
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

	pthread_exit(0);
}
// ----------------------------------------------------------------------------------
void * SenderThread(void * arg)
{
	printf("Enter Text to send: \n");
	char text[200];
	while(1)
	{
		scanf(" %[^\n]", text);
		send(c_socket, text, sizeof(text), 0);
		if(strcmp(text, "/exit") == 0) break;
	}

	pthread_exit(0);
}
// ----------------------------------------------------------------------------------

int main()
{
	printf("----------------------Multi Client Chat Transmitter-----------------------------\n");

	int ex2 = 0;

	while(ex2 == 0)
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

			char dest_name[50];
			printf("Enter Dest Name: ");
			scanf("%s", dest_name);
			send(c_socket, dest_name, sizeof(dest_name), 0);

			printf("[+] Server Connected\n\n");

			pthread_t receiver_thread, sender_thread;
			pthread_attr_t attr;
			pthread_attr_init(&attr);

			pthread_create(receiver_thread, attr, ReceiverThread, NULL);
			pthread_create(sender_thread, attr, SenderThread, NULL);

			pthread_join(&sender_thread, NULL);
			pthread_join(&receiver_thread, NULL);

			printf("[+] Server Disconnected\n\n");
			printf("Do you want to ex2: ");
			scanf("%d", &ex2);

			close(c_socket);
			printf("\n---------------------------------------------------------------------\n");
		}
	}
	return 0;
}
