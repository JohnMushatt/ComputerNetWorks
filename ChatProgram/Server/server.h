/*
 * server.h
 *
 *  Created on: Jun 21, 2019
 *      Author: john
 */

#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdatomic.h>

#include "hashmap.h"
#define PORT 8080
#define MAX_USERS 20

typedef struct {
	struct sockaddr_in addr;
	int connfd;
	int uid;
	char username[20];
} client_instance;
void messageConnectedClients(const char *buffer);
client_instance *getClient(int id);
char *getMessageType(const char *buffer);

char *getMessage(const char *buffer);

char* getUsername(const char *buffer);
#endif /* SERVER_SERVER_H_ */
