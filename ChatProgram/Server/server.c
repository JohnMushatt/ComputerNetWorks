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
static _Atomic(int) connected_clients = 0;

int id = 0;
int status = 1;

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
	strcat(name, (strchr(buffer, ':') + 1));
	return name;
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

void displayClients(int id) {
	client_instance *client = getClient(id);

	pthread_mutex_lock(&client_mutex);

	char *client_list[connected_clients];
	int count = 0;
	for(int i =0; i < MAX_USERS;i++) {
		char *username = connected_users[i]->username;

		client_list[count] = malloc(sizeof(char) * (strlen(username)));
		strcat(client_list[count],username);
	}

	char *message;

	write(client->connfd,message,sizeof(message));


}

int connectClient(client_instance *client) {
	pthread_mutex_lock (&client_mutex);
	for(int i = 0; i < MAX_USERS;i++) {
		if(connected_users[i]==NULL) {
			connected_users[i] = client;
			i = MAX_USERS;
			pthread_mutex_unlock (&client_mutex);

			return 1;
		}
	}
	pthread_mutex_unlock (&client_mutex);
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
client_instance *getClient(int id) {
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

void* handleClients(void *arg) {
	/*
	char buffer[30000] = { 0 };
	char response[1024];

	long valread = read(new_socket, buffer, 30000);

	char *message_type = getMessageType(buffer);

	if (strcmp(message_type, "USER_CONNECT") == 0) {
		char *username = getUsername(buffer);
		printf(
				"User %s has attempted to join, checking if the username is valid...\n",
				username);
		char *retrievedName = ht_get(connected_users, username);
		if (retrievedName == NULL) {
			printf("Username: %s is valid, adding to server!\n", username);
			ht_set(connected_users, username, "connected");
			snprintf(response, sizeof(response), "SERVER_USERNAME:VALID");

		} else {
			printf("Username: %s is NOT valid\n", username);
			snprintf(response, sizeof(response), "SERVER_USERNAME:INVALID");
		}
		write(new_socket, response, strlen(response));

	}
	//TODO
	else if (strcmp(message_type, "USER_MESSAGE") == 0) {
		//char *message = getMessage(buffer);

	}
	//TODO
	else if (strcmp(message_type, "USER_WHISPER") == 0) {

	}
	//TODO
	else if (strcmp(message_type, "USER_DISCONNECT") == 0) {

		char *username = getUsername(buffer);
		ht_set(connected_users, username, "disconnected");
		printf("%s has been disconnected!\n", username);
		snprintf(response, sizeof(response), "SERVER_DISCONNECT:SUCCESS");
		write(new_socket, response, strlen(response));
		close(new_socket);

	}
	*/
}

/*
int main(int argc, char const *argv[]) {
	struct Node *chat_log = NULL;
	hashtable_t *connected_users = ht_create(MAX_USERS);
	int server_fd, new_socket;
	long valread = 0;
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

	if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		perror("In bind");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, MAX_USERS) < 0) {
		perror("In listen");
		exit(EXIT_FAILURE);
	}
	int STATUS = 1;
	while (STATUS==1) {
		char buffer[30000] = { 0 };
		char response[1024];
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		if ((new_socket = accept(server_fd, (struct sockaddr*) &address,
				(socklen_t*) &addrlen)) < 0) {
			perror("In accept");
			exit(EXIT_FAILURE);
		}
		if(connected_clients== MAX_USERS) {
			printf("Max # of users has already been reached, rejecting connection!\n");
			snprintf(response,sizeof(response),"SERVER_FULL");
			write(new_socket,response,sizeof(response));
			close(new_socket);

		}
		else {

		}

		if ( (valread = read(new_socket, buffer, 30000)) > 0) {

			char *message_type = getMessageType(buffer);

			if (strcmp(message_type, "USER_CONNECT") == 0) {
				char *username = getUsername(buffer);
				printf(
						"User %s has attempted to join, checking if the username is valid...\n",
						username);
				char *retrievedName = ht_get(connected_users, username);
				if (retrievedName == NULL) {
					printf("Username: %s is valid, adding to server!\n",
							username);
					//ht_set(connected_users, username, "connected");
					snprintf(response, sizeof(response),
							"SERVER_USERNAME:VALID");

				} else {
					printf("Username: %s is NOT valid\n", username);
					snprintf(response, sizeof(response),
							"SERVER_USERNAME:INVALID");
				}
				write(new_socket, response, strlen(response));

			}
			//TODO
			else if (strcmp(message_type, "USER_MESSAGE") == 0) {
				char *message = getMessage(buffer);

			}
			//TODO
			else if (strcmp(message_type, "USER_WHISPER") == 0) {

			}
			//TODO
			else if (strcmp(message_type, "USER_DISCONNECT") == 0) {
				char *username = getUsername(buffer);
				ht_set(connected_users, username, "disconnected");
				printf("%s has been disconnected!\n", username);
				snprintf(response, sizeof(response),
						"SERVER_DISCONNECT:SUCCESS");
				write(new_socket, response, strlen(response));
				close(new_socket);
			}
			//write(new_socket , hello , strlen(hello));
			//printf("------------------Hello message sent-------------------\n");
		}
	}
	return 0;
}
*/

int main(int argc, char const *argv[]) {

	//struct Node *chat_log = NULL;
	//hashtable_t *connected_users = ht_create(MAX_USERS);
	int server_fd, new_socket;
	long valread;
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

	while (status==1) {
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		socklen_t client_size = sizeof(client_address);
		char buffer[30000] = {0};
		char response[1024];

		if ((new_socket = accept(server_fd, (struct sockaddr*) &server_address,
				(socklen_t*) &addrlen)) < 0) {
			perror("In accept");
			exit(EXIT_FAILURE);
		}
		if (connected_clients == MAX_USERS) {
			snprintf(response, sizeof(response), "SERVER_CONNECT:FAILURE");
			write(new_socket, response, strlen(response));
			close(new_socket);
		}
		client_instance *client = (client_instance *) malloc(sizeof(client_instance));
		client->addr= client_address;
		client->connfd = new_socket;
		client->uid= id++;


		char *message_type = getMessageType(buffer);

		if (strcmp(message_type, "USER_CONNECT") == 0) {
			char *username = getUsername(buffer);
			printf(
					"User %s has attempted to join, checking if the username is valid...\n",
					username);
			char *retrievedName = ht_get(connected_users, username);
			if (retrievedName == NULL) {
				printf("Username: %s is valid, adding to server!\n", username);
				strcat(client->username,username);
				snprintf(response, sizeof(response), "SERVER_USERNAME:VALID");

			} else {
				printf("Username: %s is NOT valid\n", username);
				snprintf(response, sizeof(response), "SERVER_USERNAME:INVALID");
			}
			write(new_socket, response, strlen(response));

		}
		int attempt=connectClient(client);
		if(attempt!=0) {
			printf("Successfully added client!\n");


		}
		pthread_create(&tid, NULL, &handleClients, (void *)client);
		sleep(1);

	}
	return 0;
}

