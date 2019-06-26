/*
 * client.h
 *
 *  Created on: Jun 21, 2019
 *      Author: john
 */

#ifndef CLIENT_CLIENT_H_
#define CLIENT_CLIENT_H_


#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

typedef struct {
	int connfd;
	int uid;
	char username[20];
} user_data;

typedef struct {
	int val;
	char message[1024];
} server_response;
char *fixInput(char *input);
server_response *receiveMessage(int fd, int size);
int validUsername(char *username);
#endif /* CLIENT_CLIENT_H_ */
