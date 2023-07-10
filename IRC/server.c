#include <sys/socket.h>
#include <unistd.h> //sockaddr_in, read, write
#include <arpa/inet.h> //sockaddr_in, htonl, htons, INADDR_ANY
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/event.h>

#define BUFSIZE 512

// 서버 소켓
int g_serv_sock;
// 이벤트 큐
int g_event_queue;

void error_handling(char* message) 
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void add_socket(int socket) 
{
	struct kevent event;
	EV_SET(&event, socket, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, 0);
	kevent(g_event_queue, &event, 1, 0, 0, 0);
}

void success_handler(int socket)
{
	//서버 소켓일 경우 클라이언트 소켓의 접속 요청
	if (g_serv_sock == socket)
	{
		int client_sock = accept(g_serv_sock, 0, 0);
		if (client_sock == -1)
			error_handling("socket accept error");

		if (fcntl(client_sock, F_SETFL, O_NONBLOCK) == -1)
			error_handling("client fcntl error");
	
		add_socket(client_sock);
	}
	else
	{
		//클라이언트 소켓일 경우 채팅 요청
		char message[BUFSIZE];

		int nRcv = recv(socket, message, sizeof(message) - 1, 0);
		if (nRcv < 1)
		{
			//클라이언트 종료
		}

		message[nRcv] = '\0';

		printf("ReceiveMessage : %s\n", message);

		char *check_message = "Server accept\n\0";
		send(socket, check_message, (int)strlen(check_message), 0);
	}
}

int main(int argc, char **argv) 
{
	int clnt_sock;

	//sockaddr_in은 소켓 주소의 틀을 형성해주는 구조체로 AF_INET(IPv4)일 경우 사용
	struct sockaddr_in serv_addr;

	if (argc != 2)
	{
		printf("Please, Insert Port Number\n");
		exit(1);
	}

	//TCP 연결지향형이고 IPv4 도메인을 위한 소켓 생성
	if ((g_serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		error_handling("socket error");

	//SO_REUSEADDR 옵션 설정
	int option = 1;
	if (setsockopt(g_serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
		error_handling("setsock error");

	//NONBLOCK 플래그 설정
	if (fcntl(g_serv_sock, F_SETFL, O_NONBLOCK) < 0)
		error_handling("fcntl error");

	char *end;
	long port = strtol(argv[1], &end, 10);

	//주소를 초기화한 후 IP 주소와 포트 지정
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;	//타입 : IPv4
	serv_addr.sin_addr.s_addr = INADDR_ANY;	//ip 주소 (htonl(inet_addr(INADDR_ANY)))
	serv_addr.sin_port = htons(port);	//포트

	//소켓과 서버 주소를 바인딩
	if (bind(g_serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind error");

	//연결 대기열 2개 생성
	if (listen(g_serv_sock, 2) == -1)
		error_handling("listen error");

	//kqueue 생성
	if ((g_event_queue = kqueue()) < 0)
		error_handling("kqueue error");

	//서버 소켓 kqueue, kevent 연결
	add_socket(g_serv_sock);

	//이벤트 루프
	while(1)
	{
		//이벤트 감지
		const int MAX_EVENT = 100;

		struct kevent events[MAX_EVENT];
		int nEvents = kevent(g_event_queue, 0, 0, events, MAX_EVENT, 0);

		for (int i = 0; i < nEvents; i++)
		{
			struct kevent* cur_event = &events[i];

			if (cur_event->filter == EVFILT_WRITE)
			{
				printf("This is write");
				exit(1);
			}
			if (cur_event->filter == EVFILT_READ)
			{
				success_handler(cur_event->ident);
			}
			if (cur_event->flags & EV_EOF)
			{
				printf("This is EOF");
				exit(1);
			}
			if (cur_event->flags & EV_ERROR)
			{
				printf("This is error");
				exit(1);
			}
		}
	}


	return 0;
}