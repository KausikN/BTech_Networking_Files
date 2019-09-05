#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>

int main()
{
	char ipaddr[16];
	//printf("Enter IP Address to connect: ");
	//scanf("%s", ipaddr);
	//printf("%s", ipaddr);

	int c_socket;

	char buf[100] = "\nHello server from 123.\n";
	//buf = msg;

	c_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client;
	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(9009);
	//client.sin_addr.s_addr = inet_addr("192.168.43.229");
	client.sin_addr.s_addr = INADDR_ANY;
	if(connect(c_socket, (struct sockaddr *)&client, sizeof(client)) == -1)
	{
		printf("Connection Issue.\n");
		return 0;
	}

	while(1)
	{
		printf("Enter message to transfer: \n");
		char msg[100];
		scanf("%s", msg);
		strcpy(buf, msg);
		send(c_socket, buf, sizeof(buf), 0);
		recv(c_socket, buf, sizeof(buf), 0);

		printf("Message from Server: %s\n", buf);
	}

	close(c_socket);
	return 0;
	
}
