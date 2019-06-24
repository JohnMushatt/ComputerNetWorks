/*
 * server.c
 *
 *  Created on: Jun 21, 2019
 *      Author: john
 */

// Server side C program to demonstrate Socket programming
#include "server.h"
client_instance *connected_users[MAX_USERS];
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t topic_mutex = PTHREAD_MUTEX_INITIALIZER;
static _Atomic(int)
connected_clients = 0;

int id = 0;
int status = 1;

char* getMessageType(const char *buffer) {
	pthread_mutex_lock(&client_mutex);
	char *type;
	char *c = strchr(buffer, ':');
	int index = (int) (c - buffer);
	type = malloc(sizeof(char) * (index));
	strncat(type, buffer, index);
	pthread_mutex_unlock(&client_mutex);

	return type;
}
char* getUsername(const char *buffer) {
	pthread_mutex_lock(&client_mutex);

	char *name;
	char *c = strchr(buffer, ':');
	int index = (int) (c - buffer);
	name = malloc(sizeof(char) * (index));
	//strncat(name, buffer, index);
	strcat(name, (strchr(buffer, ':') + 1));
	pthread_mutex_unlock(&client_mutex);

	return name;
}
char* getMessage(const char *buffer) {
	pthread_mutex_lock(&client_mutex);

	char *message;
	char *c = strchr(buffer, ':');
	int index = (int) (c - buffer);
	message = malloc(sizeof(char) * (index));
	//strncat(name, buffer, index);
	strcat(message, (strchr(buffer, ':') + 1));
	pthread_mutex_unlock(&client_mutex);

	return message;
}

void displayClients(int id) {
	client_instance *client = getClient(id);

	pthread_mutex_lock(&client_mutex);

	char *message = malloc(sizeof(char) * (connected_clients * 30));
	for (int i = 0; i < MAX_USERS; i++) {
		if (connected_users[i] != NULL) {
			char *username = connected_users[i]->username;

			char temp[20];
			snprintf(temp, sizeof(temp), "User: %s\n", username);
			strcat(message, temp);
		}
	}

	write(client->connfd, message, sizeof(message));
	pthread_mutex_unlock(&client_mutex);

}
/**
 *
 */
int checkName(const char *buffer) {
	pthread_mutex_lock(&client_mutex);
	for (int i = 0; i < MAX_USERS; i++) {
		if (connected_users[i] != NULL) {
			if (strcmp(buffer, connected_users[i]->username) == 0) {
				pthread_mutex_unlock(&client_mutex);

				return EXIT_FAILURE;
			}
		}
	}
	pthread_mutex_unlock(&client_mutex);

	return EXIT_SUCCESS;
}
int connectClient(client_instance *client) {
	pthread_mutex_lock(&client_mutex);
	for (int i = 0; i < MAX_USERS; i++) {
		if (connected_users[i] == NULL) {
			connected_users[i] = client;
			pthread_mutex_unlock(&client_mutex);

			return 1;
		}
	}
	pthread_mutex_unlock(&client_mutex);
	return 0;
}
int disconnectClient(client_instance *client) {
	pthread_mutex_lock(&client_mutex);
	for (int i = 0; i < MAX_USERS; i++) {
		if (connected_users[i] != NULL) {
			if (connected_users[i]->uid == client->uid) {
				connected_users[i] = NULL;
				pthread_mutex_unlock(&client_mutex);
				return 1;
			}
		}
	}
	pthread_mutex_unlock(&client_mutex);
	return 0;
}

char* getWhisperTarget(const char *buffer) {
	pthread_mutex_lock(&client_mutex);

	char *target = malloc(sizeof(char) * (MAX_USERNAME_LENGTH));
	char *temp = malloc(sizeof(char) * (MAX_USERNAME_LENGTH));
	char *c = strchr(buffer, ':');
	int index = (int) (c - buffer + 1);

	strcpy(temp, buffer + index);
	c = strchr(temp, ':');
	index = (int) (c - temp+1);
	strcpy(temp, temp + index);
	c = strchr(temp, ':');
	index = (int) (c - temp);
	strncpy(target, temp, index);
	pthread_mutex_unlock(&client_mutex);

	return target;

}
char* getWhisperMessage(const char *buffer) {
	pthread_mutex_lock(&client_mutex);

	char *message = malloc(sizeof(char) * (MAX_USERNAME_LENGTH));
	char temp[1024];
	char *c = strchr(buffer, ':');
	int index = (int) (c - buffer + 1);
	//name = malloc(sizeof(char) * (index));
	//strncat(target, buffer, index);
	strcpy(temp, buffer + index);
	c = strchr(temp, ':');
	index = (int) (c - temp + 1);
	strcpy(temp, temp + index);
	c = strchr(temp, ':');
	index = (int) (c - temp + 1);
	strcpy(message, temp + index);

	pthread_mutex_unlock(&client_mutex);

	return message;

}
/**
 *
 */
int whisper(const char *buffer, int id) {
	pthread_mutex_lock(&client_mutex);
	for (int i = 0; i < MAX_USERS; i++) {
		if (connected_users[i]->uid == id) {
			int clientFD = connected_users[i]->connfd;

			write(clientFD, buffer, sizeof(buffer));
			pthread_mutex_unlock(&client_mutex);
			return 1;
		}
	}
	pthread_mutex_unlock(&client_mutex);
	return 0;

}

void messageConnectedClients(const char *buffer) {
	pthread_mutex_lock(&client_mutex);
	for (int i = 0; i < MAX_USERS; i++) {
		if (connected_users[i] != NULL) {
			int clientFD = connected_users[i]->connfd;
			write(clientFD, buffer, sizeof(buffer));
		}
	}
	pthread_mutex_unlock(&client_mutex);
}
/**
 * Returns the client structure with the given id if it exists
 */
client_instance* getClient(int id) {
	pthread_mutex_lock(&client_mutex);
	for (int i = 0; i < MAX_USERS; i++) {
		if (connected_users[i]->uid == id) {
			client_instance *client = connected_users[i];
			pthread_mutex_unlock(&client_mutex);

			return client;
		}
	}
	pthread_mutex_unlock(&client_mutex);
	return NULL;
}
client_instance* getClientByName(const char *buffer) {
	pthread_mutex_lock(&client_mutex);
	for (int i = 0; i < MAX_USERS; i++) {
		if (strcmp(buffer, connected_users[i]->username) == 0) {
			pthread_mutex_unlock(&client_mutex);

			return connected_users[i];
		}
	}
	pthread_mutex_unlock(&client_mutex);
	return NULL;
}
//*

void* handleClients(void *arg) {

	char buffer[30000] = { 0 };
	char response[1024];
	connected_clients++;
	client_instance *current_client = (client_instance*) arg;

	long valread = 0;
	while ((valread = read(current_client->connfd, buffer, sizeof(buffer))) > 0) {

		char *message_type = getMessageType(buffer);
		if (strcmp(message_type, "USER_CONNECT") == 0) {
			char *username = getUsername(buffer);
			if (checkName(username) == EXIT_SUCCESS) {
				strcpy(current_client->username, username);
				printf("User: %s, has joined the server!\n",
						current_client->username);
				snprintf(response, sizeof(response), "SERVER_USERNAME:VALID");
				write(current_client->connfd, response, sizeof(response));

			} else {
				printf(
						"Username: %s has already been taken, alerting client to provide new name!\n",
						username);
				snprintf(response, sizeof(response), "SERVER_USERNAME:INVALID");
				write(current_client->connfd, response, sizeof(response));
			}
		}
		//If user is sending a message
		else if (strcmp(message_type, "USER_MESSAGE") == 0) {
			char *message = getMessage(buffer);
			message = getMessage(message);
			messageConnectedClients(message);

		} else if (strcmp(message_type, "USER_WHISPER") == 0) {

			char *source = current_client->username;
			char *target = getWhisperTarget(buffer);
			char *whisper_message = getWhisperMessage(buffer);
			char message[1024];
			sprintf(message, "%s says: %s", source, whisper_message);
			client_instance *target_client = getClientByName(target);
			whisper(message, target_client->uid);

		} else if (strcmp(message_type, "USER_DISCONNECT") == 0) {

			char *username = getUsername(buffer);
			printf("%s has been disconnected!\n", username);
			snprintf(response, sizeof(response), "SERVER_DISCONNECT:SUCCESS");
			write(current_client->connfd, response, strlen(response));
			close(current_client->connfd);

		}
	}
	pthread_detach(pthread_self());
	return NULL;
}

int main(int argc, char const *argv[]) {

	//struct Node *chat_log = NULL;
	//hashtable_t *connected_users = ht_create(MAX_USERS);
	int server_fd, new_socket;
	//long valread;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	pthread_t tid;
	int addrlen = sizeof(server_address);
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("In socket");
		exit(EXIT_FAILURE);
	}

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons( PORT);

	memset(server_address.sin_zero, '\0', sizeof server_address.sin_zero);
	signal(SIGPIPE, SIG_IGN);
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse,
			sizeof(reuse)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
	}
#ifdef SO_REUSEPORT
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, (const char*) &reuse,
			sizeof(reuse)) < 0) {
		perror("setsockopt(SO_REUSEPORT) failed");
	}
#endif

	if (bind(server_fd, (struct sockaddr*) &server_address,
			sizeof(server_address)) < 0) {
		perror("In bind");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 20) < 0) {
		perror("In listen");
		exit(EXIT_FAILURE);
	}

	while (status == 1) {
		//printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		socklen_t client_size = sizeof(client_address);
		char response[1024];

		if ((new_socket = accept(server_fd, (struct sockaddr*) &client_address,
				(socklen_t*) &client_size)) < 0) {
			perror("In accept");
			exit(EXIT_FAILURE);
		}
		if (connected_clients == MAX_USERS) {
			snprintf(response, sizeof(response), "SERVER_CONNECT:FAILURE");
			write(new_socket, response, strlen(response));
			close(new_socket);
		}
		client_instance *client = (client_instance*) malloc(
				sizeof(client_instance));
		client->addr = client_address;
		client->connfd = new_socket;
		client->uid = id++;

		connectClient(client);
		pthread_create(&tid, NULL, &handleClients, (void*) client);
		sleep(1);

	}
	return 0;
}

