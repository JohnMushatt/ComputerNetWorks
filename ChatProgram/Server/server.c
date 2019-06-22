/*
 * server.c
 *
 *  Created on: Jun 21, 2019
 *      Author: john
 */

// Server side C program to demonstrate Socket programming
#include "server.h"
#include "hashmap.h"

char* getMessageType(const char *buffer) {
	char *type;
	char *c = strchr(buffer, ':');
	int index = (int) (c - buffer);
	type = malloc(sizeof(char) * (index));
	strncat(type, buffer, index);
	return type;
}
char* getUsername(const char *buffer) {
	char *name;
	char *c = strchr(buffer, ':');
	int index = (int) (c - buffer);
	name = malloc(sizeof(char) * (index));
	//strncat(name, buffer, index);
	strcat(name,(strchr(buffer,':')+1));
	return name;
}
int main(int argc, char const *argv[]) {

	struct table *connected_users = createTable(MAX_USERS);
	int server_fd, new_socket;
	long valread;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("In socket");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT);

	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		perror("In bind");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 10) < 0) {
		perror("In listen");
		exit(EXIT_FAILURE);
	}
	while (1) {
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		if ((new_socket = accept(server_fd, (struct sockaddr*) &address,
				(socklen_t*) &addrlen)) < 0) {
			perror("In accept");
			exit(EXIT_FAILURE);
		}

		char buffer[30000] = { 0 };
		valread = read(new_socket, buffer, 30000);

		char *message_type = getMessageType(buffer);

		if (strcmp(message_type, "USER_CONNECT") == 0) {
			char *username = getUsername(buffer);
			printf(
					"User %s has attempted to join, checking if the username is valid...\n",
					username);
			char response[1024];
			if(lookup(connected_users, username)==-1) {
				printf("Username: %s is valid, adding to server!\n",username);
				insert(connected_users, username, 1);
				snprintf(response,sizeof(response),"Username: %s is valid, adding you to the server!\n",username);

			}
			else {
				printf("Username: %s is NOT valid\n",username);
				snprintf(response,sizeof(response),"Username: %s is NOT valid because it already exists!\nPlease use a different one!\n",username);
			}
			write(new_socket,response,strlen(response));

		}
		//TODO
		else if (strcmp(message_type, "USER_MESSAGE") == 0) {

		}
		//TODO
		else if (strcmp(message_type, "USER_WHISPER") == 0) {

		}
		//TODO
		else if (strcmp(message_type, "USER_DISCONNECT") == 0) {

		}
		//write(new_socket , hello , strlen(hello));
		//printf("------------------Hello message sent-------------------\n");
		close(new_socket);
	}
	return 0;
}
