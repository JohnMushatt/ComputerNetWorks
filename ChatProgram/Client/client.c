/*
 * client.c
 *
 *  Created on: Jun 21, 2019
 *      Author: john
 */

// Client side C/C++ program to demonstrate Socket programming
#define PORT 8080
#define MAX_USERNAME_LENGTH 20
#include "client.h"
static int status = 0;

pthread_mutex_t input = PTHREAD_MUTEX_INITIALIZER;

/**
 * Validate user name on the client
 */
int validUsername(char *username) {
	if (strlen(username) <= 20) {
		return EXIT_SUCCESS;
	} else {
		return EXIT_FAILURE;
	}
}
/**
 * Remove \n character from user input
 */
char* fixInput(char *input) {
	int len = strlen(input);
	char *newString = malloc(sizeof(char) * (len));
	strncpy(newString, input, len - 1);
	return newString;

}
/**
 * Format connection message
 */
char* connectionMessage(char *username) {
	char *newString;
	char *message_type = "USER_CONNECT:";
	int len = strlen(username) + strlen(message_type) + 1;
	newString = malloc(sizeof(char) * (len));
	strcat(newString, message_type);
	strcat(newString, username);
	*(newString + len) = '\0';
	return newString;

}
/**
 * Format disconect message
 */
char* disconnectionMessage(char *username) {
	char *newString;
	char *message_type = "USER_DISCONNECT:";
	int len = strlen(username) + strlen(message_type) + 1;
	newString = malloc(sizeof(char) * (len));
	strcat(newString, message_type);
	strcat(newString, username);
	*(newString + len) = '\0';
	return newString;

}
/**
 * Get the type of message
 */
char* getMessageType(const char *buffer) {
	char *type;
	char *c = strchr(buffer, ':');
	int index = (int) (c - buffer);
	type = malloc(sizeof(char) * (index));
	strncat(type, buffer, index);
	return type;
}
/**
 * Get the source user of the message
 */
char* getMessageSource(const char *buffer) {
	char message[1024];
	char *user = malloc(sizeof(char) * (MAX_USERNAME_LENGTH));

	char *type_index = strchr(buffer, ':') + 1;
	char *name = strchr(type_index, ':');
	int name_index = (int) (name - type_index);
	if (name_index > strlen(buffer)) {
		return type_index;
	}
	strncpy(user, type_index, name_index);
	strcat(message, user);
	return user;
}
/**
 * Get the body/content of the message
 */
char* getMessageBody(const char *buffer) {

	//char *user = malloc(sizeof(char) * (MAX_USERNAME_LENGTH));

	char *type_index = strchr(buffer, ':') + 1;
	char *name = strchr(type_index, ':');
	//int name_index = (int) (name - type_index);
	//strncpy(user, type_index, name_index);
	char *message = malloc(sizeof(char) * (strlen(name + 1)));
	strcat(message, name + 1);
	return message;
}
/**
 * Similar to getMessageBody, but on longer strings
 */
char* getMessage(const char *buffer) {
	char *message;
	char *c = strchr(buffer, ':');
	int index = (int) (c - buffer);
	message = malloc(sizeof(char) * (index));
	//strncat(name, buffer, index);
	strcat(message, (strchr(buffer, ':') + 1));
	return message;
}
/**
 * Get target from user input
 */
char* getTarget(const char *buffer) {

	char *target = malloc(sizeof(char) * (MAX_USERNAME_LENGTH));
	char *temp = malloc(sizeof(char) * (MAX_USERNAME_LENGTH));
	char *c = strchr(buffer, ' ');
	int index = (int) (c - buffer + 1);
	//name = malloc(sizeof(char) * (index));
	//strncat(target, buffer, index);
	strcpy(temp, buffer + index);
	c = strchr(temp, ' ');
	index = (int) (c - temp);
	strncpy(target, temp, index);
	return target;

}
/**
 * Parse raw text for the message of the whisper
 */
char* getWhisperMessage(const char *buffer) {
	char *message = malloc(sizeof(char) * (MAX_USERNAME_LENGTH));
	char temp[1024];
	char *c = strchr(buffer, ' ');
	int index = (int) (c - buffer + 1);
	//name = malloc(sizeof(char) * (index));
	//strncat(target, buffer, index);
	strcpy(temp, buffer + index);
	c = strchr(temp, ' ');
	index = (int) (c - temp + 1);
	strcpy(message, temp + index);
	message = fixInput(message);
	return message;

}
/**
 * Get the server message from special string
 */
char* getServerMessage(const char *buffer) {

	char *index_1 = strchr(buffer, ':');

	char *message_without_type = index_1 + 1;
	char *index_2 = strchr(message_without_type, ':');

	char *message = malloc(sizeof(char) * (1024));
	char *message_without_user = index_2 + 1;
	strcpy(message, message_without_user);

	return message;
}
/**
 * Send message to specified file directory
 */
void sendMessage(const char *buffer, int fd, int size) {

	send(fd, buffer, size, 0);
	//server_response *response = receiveMessage(fd, size);
	//return response;

}
/**
 * Generate a server_resposne struct containing message and response value
 */
server_response* receiveMessage(int fd, int size) {

	server_response *response = (server_response*) malloc(
			sizeof(server_response));
	int res = read(fd, response->message, size);

	response->val = res;

	return response;

}

/**
 * Input handler function to be used in threads
 * Handles user input and sends messages to server
 */
void* handleInput(void *data) {
	char buffer[1024];
	user_data *user_info = (user_data*) data;
	char *username = user_info->username;
	int sock = user_info->connfd;
	printf("Enter a message to send to other people in the chat:\n");
	//If we are still connected
	while (status == 0) {
		char message[1024] = { 0 };
		//Get input
		fgets(message, 1024, stdin);
		//Disconnect message
		if (strncmp(message, "disconnect", 10) == 0) {
			printf("Disconnecting %s...\n", username);
			char *disconnect_message = disconnectionMessage(username);
			sendMessage(disconnect_message, sock, 1024);

		}
		//Whisper
		else if (strncmp(message, "whisper", 7) == 0) {
			char *whisper_target = getTarget(message);
			char *whisper_message = getWhisperMessage(message);
			//char *whisper = whisperMessage(username,whisper_target,whisper_message);
			int a = strlen("USER_WHISPER:");
			int b = strlen(username);
			int c = strlen(":");
			int d = strlen(whisper_target);
			int e = strlen(":");
			int f = strlen(whisper_message);
			int message_size = a + b + c + d + e + f;

			char whisper[message_size];
			sprintf(whisper, "USER_WHISPER:%s:%s:%s", username, whisper_target,
					whisper_message);
			send(sock, whisper, strlen(whisper), 0);


		}
		//Get the list of active clients
		else if (strncmp(message, "list", 4) == 0) {
			char *list_message = "USER_LIST:";
			sendMessage(list_message, user_info->connfd, strlen("USER_LIST:"));


		}
		//Normal message
		else {
			char *format = "USER_MESSAGE:";
			char *m = fixInput(message);
			sprintf(buffer, "%s%s", format, m);
			sendMessage(buffer, sock, 1024);


		}
	}
	//End thread
	pthread_detach(pthread_self());
	return NULL;
}

/**
 * Handler for messages from server
 * Responds to new arriving messages from the server and updates client of new events
 */
void* handleMessages(void *data) {
	char buffer[1024];
	user_data *user = (user_data*) data;
	server_response *response;
	//Still connected
	while (status == 0) {
		//Get the message
		response = receiveMessage(user->connfd, 1024);
		//If still connected to the server
		if (response->val > 0) {
			//Get the type of message
			char *message_type = getMessageType(response->message);
			//Connection message
			if (strcmp(message_type, "SERVER_CONNECT") == 0) {
				char *message = getMessage(response->message);
				fprintf(stdout, "\nUser %s, has joined!\n", message);

			}
			//User message
			else if (strcmp(message_type, "SERVER_MESSAGE") == 0) {
				char *source = getMessageSource(response->message);
				char *body = getMessageBody(response->message);
				fprintf(stdout, "%s says: %s\n", source, body);

			}
			//Disconnection of current user
			else if (strcmp(message_type, "SERVER_DISCONNECT") == 0) {
				fprintf(stdout, "%s Successfully Disconnected!\n",
						user->username);
				status = -1;
			}
			//Whisper
			else if (strcmp(message_type, "USER_WHISPER") == 0) {
				char *target = getMessageSource(response->message);
				char *message = getServerMessage(response->message);
				fprintf(stdout, "%s whispers to you: %s\n", target, message);

			}
			//If invalid whisper
			else if(strcmp(message_type,"WHISPER_FAILURE")==0) {
				char *target = getMessageSource(response->message);
				fprintf(stdout,"%s is not a valid user\n",target);
			}
			//List of active clients
			else if (strcmp(message_type, "SERVER_LIST") == 0) {
				char *list = response->message + (strlen("SERVER_LIST") + 1);
				//strcat(response->message,"\n");
				fprintf(stdout, "Users:\n%s\n", list);
			}
			//Another user has disconnected
			else if (strcmp(message_type, "USER_DISCONNECT") == 0) {
				char *disonnected_user = response->message
						+ strlen("USER_DISCONNECT") + 1;//getMessageSource(response->message);
				fprintf(stdout, "%s has disconnect from the server!\n",
						disonnected_user);
			}
			//Reset buffer to prevent overlapping requests
			memset(buffer, 0, sizeof(buffer));
		}
		//Server disconnect
		else {
			fprintf(stdout,"Something went wrong, most likely the server has been turned off!\nDisconnecting from the server...\nError code: %d",response->val);
			status= -1;
		}
	}
	//End thread
	pthread_detach(pthread_self());
	return NULL;
}
/**
 * Lots of boiler plate socket/connection code
 */
int main(int argc, char const *argv[]) {
	pthread_t tid;

	int sock = 0;
	long valread;
	struct sockaddr_in serv_addr;
	char buffer[1024] = { 0 };
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return EXIT_FAILURE;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
		return EXIT_FAILURE;
	}

	if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnection Failed \n");
		return EXIT_FAILURE;
	} else {
		printf(
				"Please enter username to connect as (Limited to 20 characters):");
		char *username = malloc(sizeof(char) * 20);
		fgets(username, 20, stdin);
		username = fixInput(username);
		int validName = -1;
		//Validate name
		while (validName != 0) {
			//If the name is under 20 characters
			if (validUsername(username) == 0) {

				//Build message
				char *client_message = connectionMessage(username);
				//Send it to server for validation
				send(sock, client_message, strlen(client_message), 0);
				//Get server resposne
				valread = read(sock, buffer, 1024);
				//Get message type
				char *message_type = getMessageType(buffer);

				//If the server has sent a username related event
				if (strcmp(message_type, "SERVER_USERNAME") == 0) {
					char *message = getMessage(buffer);

					//If the username is valid
					if (strcmp(message, "VALID") == 0) {
						validName = 0;
					} else if (strcmp(message, "INVALID") == 0) {
						printf("%s is not a valid username.\n", username);
						printf(
								"Please enter a valid username (Limited to 20 characters):");
						fgets(username, 20, stdin);
						username = fixInput(username);

					}
				}

				else {
					printf("%s is not a valid username.\n", username);
					printf(
							"Please enter a valid username (Limited to 20 characters):");
					fgets(username, 20, stdin);
					username = fixInput(username);

				}
			}
		}
		//User is valid and is now connected to server
		//Setup client data and spawn 2 threads
		printf("Succesfully logged into the chat server!\nYour username: %s!\n",
				username);
		user_data *user = (user_data*) malloc(sizeof(user_data));
		user->connfd = sock;
		strcpy(user->username, username);
		pthread_create(&tid, NULL, &handleInput, (void*) user);
		pthread_create(&tid, NULL, &handleMessages, (void*) user);
		while (status == 0) {

		}
		return 0;

	}

}
