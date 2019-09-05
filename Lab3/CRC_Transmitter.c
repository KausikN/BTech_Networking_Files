#include<stdio.h>

#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>

int c_socket;

char crc[100];

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

void CRC_Generate(char bitarr[], char divisor[], int bitarr_size, int divisor_size)
{
	char b[100];
	strcpy(b, bitarr);

	for(int i=0;i<divisor_size-1;i++) strcat(b, "0");

	char x[divisor_size];
	for(int i=0;i<divisor_size;i++) x[i] = b[i];
	x[divisor_size] = '\0';

	

	for(int i=0; i < bitarr_size; i++)
	{
		//printf("\nXIN: %s\n", x);

		if(x[0] == '1')
		{
			for(int j=1;j<divisor_size;j++)
			{
				if(x[j] == divisor[j])
				{
					x[j-1] = '0';
				}
				else 
				{
					x[j-1] = '1';
				}
				//printf("\ninterx %d: %c - %c\n", j, divisor[j], x[j]);
			}
		}
		else 
		{
			for(int j=1;j<divisor_size;j++)
			{
				if(x[j] == '0')
				{
					x[j-1] = '0';
				}
				else 
				{
					x[j-1] = '1';
				}
				//printf("\ninterx %d: %s - %c\n", j, "Zero", x[j]);
			}
		}
		
		if(i<bitarr_size-1)
		{
			x[divisor_size-1] = b[i+divisor_size];
			//printf("\nFinalBit %d: %c\n", i+divisor_size, b[i+divisor_size]);
		}

		//printf("\nXOUT: %s\n", x);
	}

	for(int i=0;i<divisor_size-1;i++)
	{
		crc[i] = x[i];
	}
}

int main()
{
	printf("----------------------CRC TCP/IP Transmitter-----------------------------\n");

	int error = ClientCreate(9009, 1, "");
	if(error == 1)
	{
		close(c_socket);
		printf("Connection Issue.\n");
		return 0;
	}
	else 
	{
		char bitarr[100];
		char divisor[100];

		int bitarr_size = 0;
		int divisor_size = 0;

		printf("Enter bit array: ");
		scanf("%s", bitarr);

		printf("Enter divisor: ");
		scanf("%s", divisor);

		bitarr_size = strlen(bitarr);
		divisor_size = strlen(divisor);

		printf("\nSizes: bit: %d, div: %d\n", bitarr_size, divisor_size);

		CRC_Generate(bitarr, divisor, bitarr_size, divisor_size);

		printf("Original bit array: %s\n", bitarr);
		printf("CRC: %s\n", crc);

		strcat(bitarr, crc);

		strcat(bitarr, "\0");
		strcat(divisor, "\0");

		send(c_socket, divisor, sizeof(divisor), 0);
		send(c_socket, bitarr, sizeof(bitarr), 0);
		printf("Bit Array Sent.\n");
	}

	close(c_socket);

	printf("\n---------------------------------------------------------------------\n");	
	return 0;
}

