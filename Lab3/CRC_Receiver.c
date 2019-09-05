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

int CRC_Check(char bitarr_crc[], char divisor[], int bitarr_crc_size, int divisor_size)
{
	char x[divisor_size];
	for(int i=0;i<divisor_size;i++) x[i] = bitarr_crc[i];
	x[divisor_size] = '\0';

	

	for(int i=0; i < bitarr_crc_size-divisor_size+1; i++)
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
		
		if(i<bitarr_crc_size-divisor_size+1-1)
		{
			x[divisor_size-1] = bitarr_crc[i+divisor_size];
			//printf("\nFinalBit %d: %c\n", i+divisor_size, bitarr_crc[i+divisor_size]);
		}

		//printf("\nXOUT: %s\n", x);
	}

	int check = 0;
	for(int i=0;i<divisor_size-1;i++)
	{
		if(x[i] != '0') return 1;
	}
	
	return 0;
}


int main()
{
	printf("----------------------CRC TCP/IP Receiver-----------------------------\n");

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
		printf("\nServer Waiting...\n");

		char bitarr_crc[100];
		char divisor[100];

		int bitarr_crc_size = 0;
		int divisor_size = 0;

		//while(1)
		{
			recv(s_server, divisor, sizeof(divisor), 0);
			recv(s_server, bitarr_crc, sizeof(bitarr_crc), 0);

			bitarr_crc[0] = '1';

			bitarr_crc_size = strlen(bitarr_crc);
			divisor_size = strlen(divisor);

			strcat(bitarr_crc, "\0");
			strcat(divisor, "\0");

			char b[bitarr_crc_size-divisor_size+1];
			char crc[divisor_size-1];
			for(int i=0;i<bitarr_crc_size;i++)
			{
				if(i < bitarr_crc_size-divisor_size+1) b[i] = bitarr_crc[i];
				else crc[i-(bitarr_crc_size-divisor_size+1)] = bitarr_crc[i];
			}

			crc[divisor_size] = '\0';
			b[bitarr_crc_size-divisor_size+1] = '\0';

			int check = CRC_Check(bitarr_crc, divisor, bitarr_crc_size, divisor_size);

			if(check == 0)
			{
				printf("No errors in bit array.\n");
				printf("Received bit array: %s\n", b);
				printf("CRC: %s\n", crc);
			}
			else 
			{
				printf("Bit array has ERROR.\n");
				printf("Received bit array: %s\n", b);
				printf("CRC: %s\n", crc);
			}
		}

		

	}

	close(s_server);
	close(s_socket);

	printf("\n---------------------------------------------------------------------\n");	
	return 0;
}

