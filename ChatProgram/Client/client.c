/*
 * client.c
 *
 *  Created on: Jun 21, 2019
 *      Author: john
 */

// Client side C/C++ program to demonstrate Socket programming
#define PORT 8080
#include "client.h"

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
char* getMessageType(const char *buffer) {
	char *type;
	char *c = strchr(buffer, ':');
	int index = (int) (c - buffer);
	type = malloc(sizeof(char) * (index));
	strncat(type, buffer, index);
	return type;
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
int main(int argc, char const *argv[]) {
	int sock = 0;
	long valread;
	struct sockaddr_in serv_addr;
	char buffer[1024] = { 0 };
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnection Failed \n");
		return -1;
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
					} else  if(strcmp(message,"INVALID")==0){
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
		printf("Succesfully logged into the chat server!\nYour username: %s!\n", username);

	}

	return 0;
}
