#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN sockAdr;
	char buf[BUF_SIZE];
	int strLen;

	if (argc != 3) {
		printf("Usage : %s <IP> <port> \n", argv[0]);
		exit(1);
	}
	
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error");

	hSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&sockAdr, 0, sizeof(sockAdr));
	sockAdr.sin_family = AF_INET;
	sockAdr.sin_addr.s_addr = inet_addr(argv[1]);
	sockAdr.sin_port = htons(atoi(argv[2]));

	if (connect(hSock, (SOCKADDR*)& sockAdr, sizeof(sockAdr)) == -1)
		ErrorHandling("connect() error");
	else
		puts("connected......");
	
	while (1)
	{
		fputs("Input message(Q to quit) : ", stdout);
		fgets(buf, BUF_SIZE, stdin);

		if (!strcmp(buf, "Q\n") || !strcmp(buf, "q\n"))
			break;

		send(hSock, buf, strlen(buf), 0);
		strLen = recv(hSock, buf, BUF_SIZE, 0);
		buf[strLen] = 0;
		printf("Message from server : %s", buf);
	}
	closesocket(hSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}