#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

unsigned WINAPI SendMsg(void* arg);
unsigned WINAPI RecvMsg(void* argv);
void ErrorHandling(char* msg);

char name[NAME_SIZE];
char msg[BUF_SIZE];

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN sockAdr;
	HANDLE hSndThread, hRcvThread;
	if (argc != 4) {
		printf("Usage : %s <IP> <port> <name> \n", argv[0]);
		exit(1);
	}
	if (WSAStartup((MAKEWORD(2, 2)), &wsaData) != 0)
		ErrorHandling("WSAStartip() error");

	sprintf(name, "[%s]", argv[3]);
	hSock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&sockAdr, 0, sizeof(sockAdr));
	sockAdr.sin_family = AF_INET;
	sockAdr.sin_addr.s_addr = inet_addr(argv[1]);
	sockAdr.sin_port = htons(atoi(argv[2]));

	if (connect(hSock, (SOCKADDR*) &sockAdr, sizeof(sockAdr)) == -1)
		ErrorHandling("connect() error");

	hSndThread = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*) &hSock, 0, NULL);
	hRcvThread = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*) &hSock, 0, NULL);

	WaitForSingleObject(hSndThread, INFINITE);
	WaitForSingleObject(hRcvThread, INFINITE);
	closesocket(hSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI SendMsg(void* arg)
{
	SOCKET hSock = *((SOCKET*)arg);
	char nameMsg[NAME_SIZE + BUF_SIZE];
	while (1)
	{
		fgets(msg, BUF_SIZE, stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			closesocket(hSock);
			exit(1);
		}
		sprintf(nameMsg, "%s %s", name, msg);
		send(hSock, nameMsg, strlen(nameMsg), 0);
	}
	return 0;
}

unsigned WINAPI RecvMsg(void* arg)
{
	SOCKET hSock = *((SOCKET*)arg);
	char nameMsg[NAME_SIZE + BUF_SIZE];
	int strLen;
	while (1)
	{
		strLen = recv(hSock, nameMsg, NAME_SIZE + BUF_SIZE, 0);
		if (strLen == -1)
			return -1;
		nameMsg[strLen] = 0;
		fputs(nameMsg, stdout);
	}
	return 0;
}

void ErrorHandling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}