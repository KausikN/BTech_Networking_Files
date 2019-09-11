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

void ReverseArr()
{
	int mid = arr_size/2+1;
	for(int i=1;i<mid;i++)
	{
		arr[i] = arr[arr_size-i];
	}
}

void HC_printArr()
{
	printf("The HC array(%d): ", arr_size);
	for(int i=0;i<arr_size;i++)
	{
		printf("%c ", arr[arr_size-i]);
	}
	printf("\n");
}

void HC_InitArr(char bitarr[], int bitarr_size)
{
	arr[0] = 'B';
	arr_size = 0;

	int pow2 = 2;
	arr[1] = 'R';
	int index = 2;
	for(int i=0;i<bitarr_size;i++)
	{
		if(index == pow2)
		{
			arr[index] = 'R';
			index++;
			pow2 = pow2 * 2;
		}
		arr[index] = bitarr[bitarr_size-i-1];
		index++;
	}

	arr_size = index-1;
}

void HC_Generate(char bitarr[], int bitarr_size)
{
	HC_InitArr(bitarr, bitarr_size);

	printf("\nBefore Red,\n");
	HC_printArr();


	int pow2 = 1;
	while(pow2 <= arr_size)
	{
		char parity = '0';
		for(int i=pow2;i<=arr_size;i+=pow2*2)
		{
			int j = 0;
			while(j < pow2)
			{
				if(parity == arr[i+j] || arr[i+j] == 'R') parity = '0';
				else parity = '1';
				j++;
			}
		}
		arr[pow2] = parity;

		pow2 = pow2 * 2;
	}
	
	printf("\nAfter Red,\n");
	HC_printArr();

}



int main()
{
	printf("----------------------HC TCP/IP Transmitter-----------------------------\n");

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
		int bitarr_size = 0;

		int exit = 0;

		while(exit == 0)
		{
			printf("Enter bit array: ");
			scanf("%s", bitarr);

			bitarr_size = strlen(bitarr);

			printf("\nSizes: bit: %d\n", bitarr_size);
			printf("\nOriginal bit array: %s\n", bitarr);

			HC_Generate(bitarr, bitarr_size);

			printf("\nSending message(%d): %s\n", arr_size, arr);

			strcat(arr, "\0");

			send(c_socket, arr, sizeof(arr), 0);
			printf("\nHC Array Sent.\n");

			char sent[] = "sent";
			send(c_socket, sent, sizeof(sent), 0);

			printf("Do you want to exit: ");
			scanf("%d", &exit);
		}
	}

	close(c_socket);

	printf("\n---------------------------------------------------------------------\n");	
	return 0;
}

