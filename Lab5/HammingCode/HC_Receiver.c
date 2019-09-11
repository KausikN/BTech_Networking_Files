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

int HC_Check(char arr[], int arr_size)
{
	int errorcheck[arr_size+1];
	for(int i=1;i<=arr_size;i++) errorcheck[i] = -1;

	int pow2 = 1;
	while(pow2 <= arr_size)
	{
		char parity = '0';
		for(int i=pow2;i<=arr_size;i+=pow2*2)
		{
			int j = 0;
			while(j < pow2)
			{
				if(parity == arr[i+j] || i+j == pow2) parity = '0';
				else parity = '1';
				j++;
			}
		}
		if(arr[pow2] == parity)
		{
			for(int i=pow2;i<=arr_size;i+=pow2*2)
			{
				int j = 0;
				while(j < pow2)
				{
					errorcheck[i+j] = 0;
					j++;
				}
			}
		}
		else 
		{
			for(int i=pow2;i<=arr_size;i+=pow2*2)
			{
				int j = 0;
				while(j < pow2)
				{
					if(errorcheck[i+j] == -1) errorcheck[i+j] = 1;
					j++;
				}
			}
		}

		pow2 = pow2 * 2;
	}

	int errno = 0;
	int errindex = -1;

	printf("\nErrors: ");

	int p2 = 1;
	for(int i=1;i<=arr_size;i++)
	{
		printf("%d(%c) ", errorcheck[i], arr[i]);
		if(i == p2)
		{
			p2 = p2 * 2;
		}
		else if(errorcheck[i] == 1)
		{
			errno++;
			errindex = i;
		}
	}
	printf("\n");
	
	if(errno > 1)
	{
		return -2;
	}
	else if(errno == 0) return -1;
	else return errindex;
}


int main()
{
	printf("----------------------HC TCP/IP Receiver-----------------------------\n");

	char arr[100];
	int arr_size = 0;

	printf("Do you want custom array: ");
	int customarrcheck = 0;
	scanf("%d", &customarrcheck);
	if(customarrcheck != 0)
	{
		char temparr[100];

		printf("Enter the array: ");
		scanf("%s", temparr);
		arr_size = strlen(temparr);

		//printf("ArrSA: %s\n", temparr);

		for(int i=1;i<=arr_size;i++)
		{
			arr[i] = temparr[i-1];
		}
		arr[0] = 'B';

		//printf("Arr/0A: %s\n", temparr);

		arr[arr_size+1] = '\0';

		//printf("Arr/0B: %s\n", arr);

		int mid = arr_size/2+1;
		for(int i=1;i<mid;i++)
		{
			char tc = arr[i];
			arr[i] = arr[arr_size-i+1];
			arr[arr_size-i+1] = tc;
		}

		printf("\nReceived message(%d): %s\n", arr_size, arr);

		int check = HC_Check(arr, arr_size);
		if(check == -1) printf("\nNo errors in received message.\n");
		else if(check == -2) printf("\nThere is more than 1 bit error.\n");
		else
		{
			char err = arr[check];
			char corr = 'E';
			if(err == '0') corr = '1';
			else if(err == '1') corr = '0';

			arr[check] = corr;

			printf("There is error in bit %d.\n", check);
			printf("Correction: %c -> %c\n", err, corr);
			printf("Correct array: %s\n", arr);
		}

		printf("\n---------------------------------------------------------------------\n");	
		return 0;
	}

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

		while(1)
		{
			recv(s_server, arr, sizeof(arr), 0);

			if(strcmp(arr, "sent") != 0)
			{
				arr_size = strlen(arr);

				printf("\nReceived message(%d): %s\n", arr_size, arr);

				int check = HC_Check(arr, arr_size);
				if(check == -1) printf("\nNo errors in received message.\n");
				else if(check == -2) printf("\nThere is more than 1 bit error.\n");
				else
				{
					char err = arr[check];
					char corr = 'E';
					if(err == '0') corr = '1';
					else if(err == '1') corr = '0';

					arr[check] = corr;

					printf("There is error in bit %d.\n", check);
					printf("Correction: %c -> %c\n", err, corr);
					printf("Correct array: %s\n", arr);
				}
			}
		}
	}

	close(s_server);
	close(s_socket);

	printf("\n---------------------------------------------------------------------\n");	
	return 0;
}

