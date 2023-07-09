#include <sys/socket.h>
#include <unistd.h> //sockaddr_in, read, write
#include <arpa/inet.h> //sockaddr_in, htonl, htons, INADDR_ANY
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define BUFSIZE 1024

void error_handling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

int main(int argc, char **argv) {
	int serv_sock;
	int clnt_sock;

	//sockaddr_in은 소켓 주소의 틀을 형성해주는 구조체로 AF_INET(IPv4)일 경우 사용
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr; //accept 함수에서 사용됨
	socklen_t clnt_addr_size;

	char message[BUFSIZE];

	if (argc != 2)
	{
		printf("Please, Insert Port Number\n");
		exit(1);
	}

	//TCP 연결지향형이고 IPv4 도메인을 위한 소켓 생성
	if ((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		error_handling("socket error");

	//SO_REUSEADDR 옵션 설정
	int option = 1;
	if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
		error_handling("setsock error");

	//NONBLOCK 플래그 설정
	if (fcntl(serv_sock, F_SETFL, O_NONBLOCK) < 0)
		error_handling("fcntl error");

	char *end;
	long port = strtol(argv[1], &end, 10);

	//주소를 초기화한 후 IP 주소와 포트 지정
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;	//타입 : IPv4
	serv_addr.sin_addr.s_addr = INADDR_ANY;	//ip 주소 (htonl(inet_addr(INADDR_ANY)))
	serv_addr.sin_port = htons(port);	//포트

	//소켓과 서버 주소를 바인딩
	if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind error");

	//연결 대기열 2개 생성
	if (listen(serv_sock, 2) == -1)
		error_handling("listen error");
	
	/* reactor - kqueue, kevent 활용 후 이벤트 감지 */

	return 0;
}