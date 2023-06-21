#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#define BUFSIZE 1024

void error_handling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

int main (int argc, char **argv) {
	int clnt_sock;
	struct sockaddr_in serv_addr;

	char sAddr[15];
	int sPort;
	int nRcv;
	unsigned int addr;
	char message[BUFSIZ];

	printf("Server Address : ");
	gets(sAddr);
	printf("Port Number : ");
	gets(message);
	sPort = atoi(message);

	clnt_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (clnt_sock == -1)
		error_handling("Socket Error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(sAddr);
	serv_addr.sin_port = htons(sPort);

	if (connect(clnt_sock, (void *) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("Connection Error");
	else
		printf("Connect OK!\nStart...\n");

	while (1) {
		printf("\nSend Message : ");
		gets(message);

		if (strcmp(message, "exit") == 0) {
			send(clnt_sock, message, (int)strlen(message), 0);
			break;
		}

		send(clnt_sock, message, (int)strlen(message), 0);
		printf("Message Receives...\n");

		nRcv = recv(clnt_sock, message, sizeof(message) - 1, 0);
		if (nRcv == -1) {
			printf("Receive Error...\n");
			break;
		}
		message[nRcv] = '\0';

		if (strcmp(message, "exit") == 0) {
			printf("Close Server Connection...\n");
			break;
		}

		printf("Receive Message : %s\n", message);
	}

	close(clnt_sock);
	printf("Close Connection...\n");

	return 0;
}
