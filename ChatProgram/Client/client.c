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
static _Atomic(int)
status = 0;

pthread_mutex_t input = PTHREAD_MUTEX_INITIALIZER;

int validUsername(char *username) {
	if (strlen(username) <= 20) {
		return EXIT_SUCCESS;
	} else {
		return EXIT_FAILURE;
	}
}
char* fixInput(char *input) {
	int len = strlen(input);
	char *newString = malloc(sizeof(char) * (len));
	strncpy(newString, input, len - 1);
	return newString;

}
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
char* getMessageType(const char *buffer) {
	char *type;
	char *c = strchr(buffer, ':');
	int index = (int) (c - buffer);
	type = malloc(sizeof(char) * (index));
	strncat(type, buffer, index);
	return type;
}
char* getMessageSource(const char *buffer) {
	char message[1024];
	char *user = malloc(sizeof(char) * (MAX_USERNAME_LENGTH));

	char *type_index = strchr(buffer, ':') + 1;
	char *name = strchr(type_index, ':');
	int name_index = (int) (name - type_index);
	strncpy(user, type_index, name_index);
	strcat(message, user);
	return user;
}
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
char* getMessage(const char *buffer) {
	char *message;
	char *c = strchr(buffer, ':');
	int index = (int) (c - buffer);
	message = malloc(sizeof(char) * (index));
	//strncat(name, buffer, index);
	strcat(message, (strchr(buffer, ':') + 1));
	return message;
}
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
char *getServerMessage(const char *buffer) {

	char *index_1 = strchr(buffer,':');

	char *message_without_type = index_1+1;
	char *index_2 = strchr(message_without_type,':');

	char *message = malloc(sizeof(char) * (1024));
	char *message_without_user = index_2+1;
	strcpy(message,message_without_user);

	return message;
}
void sendMessage(const char *buffer, int fd, int size) {

	int res = send(fd, buffer, size, 0);
	//server_response *response = receiveMessage(fd, size);
	//return response;

}
server_response* receiveMessage(int fd, int size) {

	server_response *response = (server_response*) malloc(
			sizeof(server_response));
	read(fd, response->message, size);
	response->val = 0;
	/*
	 if(strncmp(response->message,"SERVER_MESSAGE",strlen("SERVER_MESSAGE"))==0) {
	 strcpy(overflow,response->message);
	 response->val = 1;
	 }
	 */
	return response;

}

void* handleInput(void *data) {
	char buffer[1024];
	long valread;
	user_data *user_info = (user_data*) data;
	char *username = user_info->username;
	int sock = user_info->connfd;
	int connected = 0;
	printf("Enter a message to send to other people in the chat: ");

	while (status == 0) {
		char message[1024];
		fgets(message, 1024, stdin);

		if (strncmp(message, "disconnect", 10) == 0) {
			printf("Disconnecting %s...\n", username);
			char *disconnect_message = disconnectionMessage(username);
			sendMessage(disconnect_message, sock, 1024);
			//send(sock, disconnect_message, strlen(disconnect_message), 0);

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
			//sprintf(whisper,)
			sprintf(whisper, "USER_WHISPER:%s:%s:%s",
					username, whisper_target, whisper_message);
			send(sock, whisper, strlen(whisper), 0);
			//valread = read(sock, buffer, 1024);
			//char *message_response = getMessage(buffer+1);
			//if (strcmp(message_response, "SUCCESS") == 0) {
				//fprintf(stdout, "Message delivered to %s!\n", whisper_target);

				////printf("Message delivered to %s!\n", whisper_target);
			//}

		}
		//Get the list of active clients
		else if (strncmp(message, "list", 4) == 0) {
			char *list_message = "USER_LIST";
			send(sock, list_message, sizeof(list_message), 0);
			valread = read(sock, buffer, 1024);
			fprintf(stdout, "Connected Clients:\n%s\n", buffer);

			//printf("Connected Clients:\n%s", buffer);

		}
		//Normal message
		else {
			char *format = "USER_MESSAGE:";
			char *m = fixInput(message);
			sprintf(buffer, "%s%s", format, m);
			sendMessage(buffer, sock, 1024);
			/*
			 server_response *response =sendMessage(buffer, sock, 1024);
			 if(response->val==0) {

			 }
			 else if(response->val==1) {
			 fprintf(stdout,"\n%s\n",response->message);
			 }
			 */

		}
	}
	pthread_detach(pthread_self());
	return NULL;
}

void* handleMessages(void *data) {
	char buffer[1024];
	user_data *user = (user_data*) data;
	server_response *response;
	while (status == 0) {
		response = receiveMessage(user->connfd, 1024);
		char *message_type = getMessageType(response->message);

		if (strcmp(message_type, "SERVER_CONNECT") == 0) {
			char *message = getMessage(response->message);
			fprintf(stdout, "\nUser %s, has joined!\n", message);

		} else if (strcmp(message_type, "SERVER_MESSAGE") == 0) {
			char *source = getMessageSource(response->message);
			char *body = getMessageBody(response->message);
			fprintf(stdout, "%s says: %s\n", source, body);

		} else if (strcmp(message_type, "SERVER_DISCONNECT") == 0) {
			fprintf(stdout,"%s Successfully Disconnected!\n", user->username);
			status = -1;
		}
		else if(strcmp(message_type,"USER_WHISPER")==0) {
			char *target = getMessageSource(response->message);
			char *message = getServerMessage(response->message);
			fprintf(stdout,"%s whispers to you: %s\n",target,message);

		}
		else if(strcmp(message_type,"SERVER_LIST")==0){
			strcat(response->message,"\n");
			fprintf(stdout,"Users:\n%s\n",response->message);
		}
	}
	pthread_detach(pthread_self());
	return NULL;
}
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
