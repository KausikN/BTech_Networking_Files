#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include <arpa/inet.h>


#include<time.h>

char questions[][25] = {"What is 1+1?", "What is 2+1?", "What is 1+3?"};
char answers[][25] = {"2", "3", "4"};
int no_of_ques = 3;

int maxtime_to_ans = 5;

int s_socket, s_server;
char clientip[20];

int loop() 
{
    time_t start, end;
    double elapsed;
 	int k=0;
    time(&start);  /* start the timer */
 
    do 
    {
        time(&end);
 
        elapsed = difftime(end, start);
        
        /* For most data types one could simply use
            elapsed = end - start;
            but for time_t one needs to use difftime(). */
 		if(elapsed==k)
 		{
 			printf("elapsed %f\n\r", elapsed);
 			k++;
 		}
    } while(elapsed < maxtime_to_ans);  /* run for ten seconds */

return 1;
}

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
		strcpy(clientip, inet_ntoa(other.sin_addr));
		printf("\n[+] Conection accepted from %s,%d\n",inet_ntoa(other.sin_addr),ntohs(other.sin_port));
		return 0;
	}
}

struct Client
{
	char name[50];
	char ipaddr[20];
	char port_str[20];

	int s_server;

	int avg_time;
	int correct;
	int wrong;
	int late;
};

void DisplayConnectedClients(struct Client clients[], int clients_size)
{
	printf("\nConnected Clients: ");
	for(int i=0;i<clients_size;i++)
	{
		printf("%s, ", clients[i].name);
	}
	printf("\n");
}

int GetCurTime(char returnval[], int display)
{
	time_t t;
	struct tm * timeinfo;
	time(&t);
	timeinfo = localtime(&t);

	int year = timeinfo->tm_year + 1900;
	int month = timeinfo->tm_mon + 1;
	int date = timeinfo->tm_mday;
	int hour = timeinfo->tm_hour;
	int minutes = timeinfo->tm_min;
	int seconds = timeinfo->tm_sec;

	if(display == 1) printf("\nCurTime: %d:%d:%d - %d/%d/%d\n", hour, minutes, seconds, date, month, year);

	if(strcmp(returnval, "year") == 0) return year;
	else if(strcmp(returnval, "month") == 0) return month;
	else if(strcmp(returnval, "date") == 0) return date;
	else if(strcmp(returnval, "hour") == 0) return hour;
	else if(strcmp(returnval, "minutes") == 0) return minutes;
	else if(strcmp(returnval, "seconds") == 0) return seconds;
	else return -1;
}

int GetCurTimeInSec()
{
	int sec = GetCurTime("seconds", 0) + 60*GetCurTime("minutes", 0) + 3600*GetCurTime("hour", 0);
	return sec;
}

int main()
{
	printf("----------------------CROREPATHI Receiver-----------------------------\n");

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
				printf("Ready to accept new client...\n");
				int accepterror = AcceptNewClient();
				if(accepterror == 0)
				{

					int pid = fork();
					if(pid > 0)
					{
						//return 0;
						continue;
					} 
					else if(pid == 0)
					{
						struct Client client;

						client.avg_time = 0;
						client.correct = 0;
						client.wrong = 0;
						client.late = 0;

						int s_server_child = s_server;
						client.s_server = s_server_child;
						strcpy(client.ipaddr, clientip);

						char name[50];
						char ipaddr[20];
						char port_str[20];
						//char dest_name[50];

						//char text[200];

						recv(s_server_child, name, sizeof(name), 0);
						recv(s_server_child, ipaddr, sizeof(ipaddr), 0);
						recv(s_server_child, port_str, sizeof(port_str), 0);
						

						strcpy(client.name, name);
						
						strcpy(client.port_str, port_str);

						//printf("%s\n", );
						/*
						int pidnextclient = fork();
						if(pidnextclient > 0)
						{
							continue;
						}
						*/

						int time_s = 0, time_e = 0;
						int time_to_ans = 0;

						char correct[15] = "Correct!";
						char wrong[100] = "Better Luck Next Time...";
						char late[15] = "Too Late...";

						char ans[200];

						char over[2];
						over[0] = '0';
						over[1] = '\0';

						for(int i=0;i<no_of_ques;i++)
						{
							send(s_server_child, over, sizeof(over), 0);
							printf("\nQuestion for %s: %s\n", client.name, questions[i]);
							send(s_server_child, questions[i], sizeof(questions[i]), 0);

							//loop()
							//time_s = GetCurTimeInSec();

							recv(s_server_child, ans, sizeof(ans), 0);
							client.avg_time += time_to_ans / no_of_ques;

							//printf("\n\n%d\n\n", strlen(ans));
							if(strcmp(ans,"-666")==0)
							{
								client.late++;
								printf("\nAnswer entered by %s: %s -- Late Answer (%d sec to answer)\n", client.name, ans, time_to_ans);
								send(s_server_child, late, sizeof(late), 0);
							}
							else if(strcmp(answers[i], ans) == 0)
							{
								client.correct++;
								printf("\nAnswer entered by %s: %s -- Correct (%d sec to answer)\n", client.name, ans, time_to_ans);
								send(s_server_child, correct, sizeof(correct), 0);
							}
							else 
							{
								client.wrong++;
								printf("\nAnswer entered by %s: %s -- Wrong -- Expected %s\n", client.name, ans, answers[i]);
								send(s_server_child, wrong, sizeof(wrong), 0);
								break;

							}
						}
						printf("\nOVER\n");
						over[0] = '1';
						over[1] = '\0';
						send(s_server_child, over, sizeof(over), 0);

						char result[25];
						sprintf(result, "%dc%dw%dl%dq", client.correct, client.wrong, client.late, no_of_ques);
						send(s_server_child, result, sizeof(result), 0);

						close(s_server_child);
						exit(0);
						//return 0;
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