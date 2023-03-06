#include <stdio.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>	   //write

#define PORT_NUMBER 60000

int main(int argc, char *argv[])
{

	int socket_desc, client_sock, c, read_size; // socket description is the variable where socket is kept. Client_sock is the variable where the client socket is kept. Read size variable that holds the size of the message from the client.
	struct sockaddr_in server, client; //struct that holds the addresses of server and client sockets.
	char client_message[20];//keeps the message from the client
    //Client's Date Requests
	char getDayofWeek[] = "GET_DAY_OF_WEEK";
	char getTime[] = "GET_TIME";
	char getDate[] = "GET_DATE";
	char getTimeDate[] = "GET_TIME_DATE";
	char getTimeZone[] = "GET_TIME_ZONE";
    //variables created to use the popen function
	FILE *command;
	FILE *command2;
    //Variables created to send the correct response to the client as a result of the popen function.
	char answer[13];
	char answer2[12];
	char answer3[] = " ";

	// Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket"); // error when socket is not created
	}
	puts("Socket created");

	// Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT_NUMBER);

	// Bind
	if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		// print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");

	// Listen
	listen(socket_desc, 3);

	// Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);

	// accept connection from an incoming client
	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}
	puts("Connection accepted");

	// Receive a message from client

	while ((read_size = recv(client_sock, client_message, 20, 0)) > 0) //loop created to continuously receive requests from the client we connect with telnet
	{
		
		if (strlen(client_message) == 17) //We grouped the requests according to the size of the request we received from the client
		{
			if (strncmp(client_message, "GET_DAY_OF_WEEK", 15) == 0)//and here we checked if they are equal. We checked whether client message and getdayofweek are equal.
			{
				command = popen("date +%A", "r");// According to the client's request, we sent the necessary parameters to the popen function
				while (fgets(answer, 13, command) != NULL)
				{
					send(client_sock, answer, strlen(answer), 0);//we send the correct answer to the client side
				}
                // To avoid confusion later, we reset the answer we will send to the user and the clientmessage we receive from the client each time.
				memset(&answer[0], '\0', sizeof(answer));
				pclose(command);//Command shutdown
				memset(&client_message[0], '\0', sizeof(client_message));
			}
			else
			{
                // Message sent to the client when Incorrect request is entered
				send(client_sock, "INCORRECT REQUEST\n", 19, 0);
				memset(&client_message[0], '\0', sizeof(client_message));

			}

		}
		else if (strlen(client_message) == 10)
		{
			if (strncmp(client_message, "GET_DATE", 8) == 0)//We checked if client message and getDate are equal.
			{
				command = popen("date +%D", "r");
				while (fgets(answer, 13, command) != NULL)
				{
					send(client_sock, answer, strlen(answer), 0);
				}
				memset(&answer[0], '\0', sizeof(answer));
				pclose(command);
				memset(&client_message[0], '\0', sizeof(client_message));
			}

			else if (strncmp(client_message, "GET_TIME", 8) == 0)//We checked whether client message and getTime are equal.
			{

				command = popen("date +%r", "r");
				while (fgets(answer, 13, command) != NULL)
				{
					send(client_sock, answer, strlen(answer), 0);
				}
				memset(&answer[0], '\0', sizeof(answer));
				pclose(command);
				memset(&client_message[0], '\0', sizeof(client_message));
			}
			else
			{
				send(client_sock, "INCORRECT REQUEST\n", 19, 0);
				memset(&client_message[0], '\0', sizeof(client_message));
			}

		}
		else if (strlen(client_message) == 15)
		{

			if (strncmp(client_message, "GET_TIME_DATE", 13) == 0)//We checked whether client message and getTimeDate are equal.
			{

				command = popen("date +%r", "r");
				command2 = popen("date +%D", "r");
				while (fgets(answer, 25, command) != NULL)
				{
				}
				answer[strlen(answer) - 1] = '\0';
				while (fgets(answer2, 12, command2) != NULL)
				{
				}

				strcat(answer, answer3);
				strcat(answer, answer2);
				send(client_sock, answer, strlen(answer), 0);
				memset(&answer[0], '\0', sizeof(answer));
				pclose(command);
				memset(&client_message[0], '\0', sizeof(client_message));
			}
			else if (strncmp(client_message, "GET_TIME_ZONE", 13) == 0)//We checked whether client message and getTimeZone are equal.
			{	
				printf("timezone'a girdim.");
				command = popen("date +%:z", "r");
				while (fgets(answer, 13, command) != NULL)
				{
					send(client_sock, answer, strlen(answer), 0);
				}
				memset(&answer[0], '\0', strlen(answer));
				pclose(command);
				memset(&client_message[0], '\0', sizeof(client_message));
			}
			else
			{
				send(client_sock, "INCORRECT REQUEST\n", 19, 0);
				memset(&client_message[0], '\0', sizeof(client_message));

			}
			
		}
		else if (strlen(client_message) == 14)
		{
			
			if (strncmp(client_message, "CLOSE_SERVER", 12) == 0)//Client message We checked whether CLOSE_SERVER is equal.
			{
				send(client_sock, "GOOD BYE\n", 9, 0);
				memset(&client_message[0], '\0', sizeof(client_message));
                // Closing the sockets so that the port can be used again
                close(socket_desc);
	            close(client_sock);
			}
			break;
		}

		else if ((strlen(client_message) != 15) && (strlen(client_message) != 10) && (strlen(client_message) != 17)&& (strlen(client_message) != 14))
		{
			
			send(client_sock, "INCORRECT REQUEST\n", 19, 0);
			memset(&client_message[0], '\0', sizeof(client_message));
			
		}
	}

	if (read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if (read_size == -1)
	{
		perror("recv failed");
	}
    // Closing the sockets so that the port can be used again
	close(socket_desc);
	close(client_sock);

	return 0;
}