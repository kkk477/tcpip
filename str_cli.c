#include "unp.h"

void str_cli(FILE *fp, int sockfd)
{
	int maxfdp1, stdineof;
	fd_set rset;
	char buf[MAXLINE];
	int n;
	stdineof = 0;
	FD_ZERO(&rset);
	for(;;)
	{
		if(stdineof == 0)	// stdin에서 받을거면 등록
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);	// sockfd 등록
		maxfdp1 = max(fileno(fp), sockfd)+1;
		select(maxfdp1, &rset, NULL, NULL, NULL);
		if(FD_ISSET(sockfd, &rset))	// 소켓에 뭔가 들어왔으면
		{
			if((n = read(sockfd, buf, MAXLINE)) == 0)	// 그래서 읽었는데 서버에서 보낸게 eof면
			{
				if(stdineof == 1)	// stdin에서는 받을게 없는 상태면 서로 종료절차
					return;
				else	// 나는 입력 받을려했는데 서버가 종료한거니까
					err_quit("str_cli : server terminated prematurely");
			}
			write(fileno(stdout), buf, n);	// 서버가 보낸게 eof가 아니면 화면에 출력
		}
		if(FD_ISSET(fileno(fp), &rset))	// fp(보통 stdin)가 들어온거면
		{
			if((n = read(fileno(fp), buf, MAXLINE)) == 0)	// 읽었는데 eof면
			{
				stdineof = 1;	// 입력 끝났음을 표시
				shutdown(sockfd, SHUT_WR);	// 소켓의 입력부 종료
				FD_CLR(fileno(fp), &rset);	// rset의 fp부분 클리어
				continue;	//뒤에 write 무시
			}
			write(sockfd, buf, n);	// 아니면 소켓에 써서 전송
		}
	}
}