#include<stdio.h>

#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>

int c_socket;
int s_socket, s_server;

int s_background_socket, s_background_server;

struct Device
{
	char device_name[100];
	char ip_addr[100];
	int connected_to_this;
};

struct RoutingTable
{
	struct Device devices[4]; 
};

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
		printf("\nError in Bind.\n");
		return 1;
	} 
}

int ClientCreate(int port, char IPADDR[])		// Return 1 for error
{
	c_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client;
	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(port);
	client.sin_addr.s_addr = inet_addr(IPADDR);
	//client.sin_addr.s_addr = INADDR_ANY;
	if(connect(c_socket, (struct sockaddr *)&client, sizeof(client)) == -1)
	{
		printf("Connection Issue.\n");
		return 1;
	}
	else return 0;
}

struct RoutingTable rt;

void RoutingTableInit(char devicenames[][], char ipaddrs[][], int no_of_devices, char this_device_name[], int connections[][])
{
	int this_device_index = 0;
	for(int i=0;i<no_of_devices;i++)
	{
		if(strcmp(devicenames[i], this_device_name) == 0) this_device_index = i;
	}

	for(int i=0;i<no_of_devices;i++)
	{
		rt.devices[i].device_name = devicenames[i];
		rt.devices[i].ipaddrs = ipaddrs[i];
		if(connections[this_device_index][i] == 1 && connections[i][this_device_index] == 1) rt.devices[i].connected_to_this = 1;
		else rt.devices[i].connected_to_this = 0;
	}
}

int GetRoutingTableIndex(device_name, no_of_devices)
{
	for(int i=0;i<no_of_devices;i++)
	{
		if(strcmp(rt.devices[i].device_name, device_name) == 0) return i;
	}
	return -1;
}

int main()
{
	int no_of_devices = 4;
	char devicenames[no_of_devices][100] = {"A", "B", "C", "D"};
	char ipaddrs[no_of_devices][100] = {"", "", "", ""};
	int connections[no_of_devices][no_of_devices] = {{-1, 1, 0, 0}, 
													 {1, -1, 0, 1}, 
													 {0, 0, -1, 1}, 
													 {0, 1, 1, -1}};




	char this_device_name[100];
	printf("Enter this Device name: ");
	scanf("%s", this_device_name);

	RoutingTableInit(devicenames, ipaddrs, no_of_devices, this_device_name, connections);

	printf("\n------------------------NODE %s---------------------------\n", this_device_name);

	//Create Server
	int errors = ServerCreate(9009);
	if(errors == 1)
	{
		close(s_server);
		close(s_socket);
		return 0;
	}

	//init vars
	int choice = 0;
	printf("Do you want to send(0/1): ");
	scanf("%d", &choice);

	if(choice == 1)
	{
		char text[500];
		char dest_devicename[100];

		printf("Enter Device Name: ");
		scanf("%s", dest_devicename);
/*
		printf("Enter IP Address to send: ");
		scanf("%s", rt.ip_addr);
*/
		printf("Enter text to send: ");
		scanf("%s", text);

		int this_device_index = GetRoutingTableIndex(this_device_name, no_of_devices);
		int dest_device_index = GetRoutingTableIndex(dest_devicename, no_of_devices);

		if(this_device_index != -1 && dest_device_index != -1 && this_device_index != dest_device_index)
		{
			if(rt.devices[dest_device_index].connected_to_this == 1)
			{
				errors = ClientCreate(9009, rt.devices[dest_device_index].ip_addr);
				if(errors == 1)
				{
					printf("\nERROR in Connection to %s.\n", rt.devices[dest_device_index].device_name);
					close(c_socket);
					//return 0;
				}
				else 
				{
					send(c_socket, rt.devices[this_device_index].device_name, sizeof(rt.devices[this_device_index].device_name), 0);
					send(c_socket, rt.devices[this_device_index].ip_addr, sizeof(rt.devices[this_device_index].ip_addr), 0);
					send(c_socket, rt.devices[dest_device_index].device_name, sizeof(rt.devices[dest_device_index].device_name), 0);
					send(c_socket, rt.devices[dest_device_index].ip_addr, sizeof(rt.devices[dest_device_index].ip_addr), 0);
					send(c_socket, text, sizeof(text), 0);
					close(c_socket);
				}
			}
			else 
			{
				for(int i=0;i<no_of_devices;i++)
				{
					if(rt.devices[i].connected_to_this == 1)
					{
						errors = ClientCreate(9009, rt.devices[i].ip_addr);
						if(errors == 1)
						{
							printf("\nERROR in Connection to %s.\n", rt.devices[i].device_name);
							close(c_socket);
							//return 0;
						}
						else 
						{
							send(c_socket, rt.devices[this_device_index].device_name, sizeof(rt.devices[this_device_index].device_name), 0);
							send(c_socket, rt.devices[this_device_index].ip_addr, sizeof(rt.devices[this_device_index].ip_addr), 0);
							send(c_socket, rt.devices[i].device_name, sizeof(rt.devices[i].device_name), 0);
							send(c_socket, rt.devices[i].ip_addr, sizeof(rt.devices[i].ip_addr), 0);
							send(c_socket, text, sizeof(text), 0);
							close(c_socket);
						}
					}
				}
			}
		}
	}
	else 
	{
		printf("\nWaiting for data......\n");

		char src_devicename[100];
		char dest_devicename[100];
		char dest_ipaddr[100];
		char src_ipaddr[100];
		char dest_text[500];

		recv(s_server, src_devicename, sizeof(src_devicename), 0);
		recv(s_server, src_ipaddr, sizeof(src_ipaddr), 0);
		recv(s_server, dest_devicename, sizeof(dest_devicename), 0);
		recv(s_server, dest_ipaddr, sizeof(dest_ipaddr), 0);
		recv(s_server, dest_text, sizeof(dest_text), 0);

		if(strcmp(dest_devicename, this_device_name) == 0)
		{
			printf("Device %s with IP %s sent text: %s", dest_devicename, dest_ipaddr, dest_text);
		}
		else 
		{
			int this_device_index = GetRoutingTableIndex(this_device_name, no_of_devices);
			int dest_device_index = GetRoutingTableIndex(dest_devicename, no_of_devices);

			if(this_device_index != -1 && dest_device_index != -1 && this_device_index != dest_device_index)
			{
				if(rt.devices[dest_device_index].connected_to_this == 1)
				{
					errors = ClientCreate(9009, rt.devices[dest_device_index].ip_addr);
					if(errors == 1)
					{
						printf("\nERROR in Connection to %s.\n", dest_devicename);
						close(c_socket);
						//return 0;
					}
					else 
					{
						send(c_socket, rt.devices[this_device_index].device_name, sizeof(rt.devices[this_device_index].device_name), 0);
						send(c_socket, rt.devices[this_device_index].ip_addr, sizeof(rt.devices[this_device_index].ip_addr), 0);
						send(c_socket, dest_devicename, sizeof(dest_devicename), 0);
						send(c_socket, dest_ipaddr, sizeof(dest_ipaddr), 0);
						send(c_socket, dest_text, sizeof(dest_text), 0);
						close(c_socket);
					}
				}
				else 
				{
					for(int i=0;i<no_of_devices;i++)
					{
						if(rt.devices[i].connected_to_this == 1)
						{
							if(!(strcmp(rt.devices[i].device_name, src_devicename)) && !(strcmp(rt.devices[i].ip_addr, src_ipaddr)))
							{
								errors = ClientCreate(9009, rt.devices[i].ip_addr);
								if(errors == 1)
								{
									printf("\nERROR in Connection to %s.\n", rt.devices[i].device_name);
									close(c_socket);
									//return 0;
								}
								else 
								{
									send(c_socket, rt.devices[this_device_index].device_name, sizeof(rt.devices[this_device_index].device_name), 0);
									send(c_socket, rt.devices[this_device_index].ip_addr, sizeof(rt.devices[this_device_index].ip_addr), 0);
									send(c_socket, dest_devicename, sizeof(dest_devicename), 0);
									send(c_socket, dest_ipaddr, sizeof(dest_ipaddr), 0);
									send(c_socket, dest_text, sizeof(dest_text), 0);
									close(c_socket);
								}
							}
						}
					}
				}
			}
		}
	}
	printf("\n------------------------------------------------------------\n");	
	return 0;
}

