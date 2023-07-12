#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>

#define BUF_SIZE 10000
void error_handling(char *buf);
int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;
	fd_set reads, writes, cpy_reads, cpy_writes;

	socklen_t adr_sz;
	int fd_max, str_len, fd_num, i;
	char buf[BUF_SIZE];
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	printf("serv_sock : %d\n", serv_sock);

	//SO_REUSEADDR 옵션 설정
	int option = 1;
	if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
		error_handling("setsock error");

	int flag=fcntl(serv_sock, F_GETFL, 0);
	fcntl(serv_sock, F_SETFL, flag|O_NONBLOCK);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	FD_ZERO(&reads);
	FD_ZERO(&writes);
	FD_SET(serv_sock, &reads);
	FD_SET(serv_sock, &writes);
	fd_max=serv_sock;

	while(1)
	{
		cpy_reads=reads;
		cpy_writes=writes;
		timeout.tv_sec=5;
		timeout.tv_usec=5000;
		if((fd_num=select(fd_max+1, &cpy_reads, 0, 0, &timeout))==-1)
			break;
		if(fd_num==0)
		{	
			printf("timeout\n");
			continue;
		}
		for(i=0; i<fd_max+1; i++)
		{
			if(FD_ISSET(i, &cpy_reads))
			{
				if(i==serv_sock)     // connection request!
				{
					printf("i'm serv_sock\n");
					adr_sz=sizeof(clnt_adr);
					clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
					int flag=fcntl(clnt_sock, F_GETFL, 0);
					fcntl(clnt_sock, F_SETFL, flag|O_NONBLOCK);
					FD_SET(clnt_sock, &reads);
					if(fd_max<clnt_sock)
						fd_max=clnt_sock;
					printf("connected client: %d \n", clnt_sock);
				}
				else    // read message!
				{
					printf("i'm client_sock\n");
					str_len=read(i, buf, BUF_SIZE);
					printf("%d\n", str_len);
					if(str_len==0)    // close request!
					{
						FD_CLR(i, &reads);
						close(i);
						printf("closed client: %d \n", i);
					}
					else
					{
						write(i, buf, str_len);    // echo!
					}
				}
			}
		}
	}
	close(serv_sock);
	return 0;
}
void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}