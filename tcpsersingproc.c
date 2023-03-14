#include "unp.h"

int main(int argc, char *argv[])
{
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SIZE];
	ssize_t n;
	fd_set rset, allset;
	char buf[MAXLINE];
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	listenfd = socket(PF_INET, SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
	bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	listen(listenfd, 5);
	maxfd = listenfd;
	maxi = -1;
	
	for(i = 0; i < FD_SETSIZE; i++)	// FD_SETSIZE 값은 시스템 상에 이미 정해져있음
		client[i] = -1;				// FD들을 기록할 배열
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	
	for(;;)
	{
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		
		if(FD_ISSET(listenfd, &rset))	// listenfd가 셋 되면
		{
			clilen = sizeof(cliaddr);	// 연결요청임
			connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);
			
			for(i = 0; i < FD_SETSIZE; i++)
			{
				if(client[i] < 0)	// client 배열의 빈칸(-1)에 connfd를 넣고 나옴.
				{
					client[i] = connfd;
					break;
				}
			}
			
			if(i == FD_SETSIZE)		// for문 중간에 못 빠져나왔으면 i가 FD_SETSIZE까지 커졌음. 즉, 꽉 찬 상태
				err_quit("too many clients");
			
			FD_SET(connfd, &allset);	// 연결한 connfd도 원본FD에 넣음
			if(connfd > maxfd)
				maxi = i;
			if(--nready <= 0)	// select결과값을 줄여 모두 체크했는지 확인
				continue;
		}
		
		for(i = 0; i <= maxi; i++)
		{
			if((sockfd = client[i]) < 0)	// client 배열에 넣어놨던 descriptor 꺼내서 없으면(-1)
				continue;					// 넘어감
			if(FD_ISSET(sockfd, &rset))		// sockfd가 rset에서 셋 되어있으면(listenfd는 먼저 확인함)
			{
				if((n = read(sockfd, buf, MAXLINE)) == 0)	// 읽었는데 EOF면 종료절차
				{
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}else									// 아니면 echo 해줌
					writen(sockfd, buf, n);
				if(--nready <= 0)				// 다 처리한건지 확인
					break;
			}
		}
	}
	return 0;
}