#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN sendAdr;

	WSABUF dataBuf;
	char msg[] = "Network is Computer!";
	int sendByte = 0;

	WSAEVENT evObj;
	WSAOVERLAPPED overlapped;

	if (argc != 3) {
		printf("Usage : %s <IP> <port> \n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&sendAdr, 0, sizeof(sendAdr));
	sendAdr.sin_family = AF_INET;
	sendAdr.sin_addr.s_addr = inet_addr(argv[1]);
	sendAdr.sin_port = htons(atoi(argv[2]));

	if (connect(hSock, (SOCKADDR*)& sendAdr, sizeof(sendAdr)) == SOCKET_ERROR)
		ErrorHandling("connect() error");

	evObj = WSACreateEvent();
	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.hEvent = evObj;
	dataBuf.len = strlen(msg) + 1;
	dataBuf.buf = msg;

	if (WSASend(hSock, &dataBuf, 1, &sendByte, 0, &overlapped, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			puts("Background data send");
			WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
			WSAGetOverlappedResult(hSock, &overlapped, &sendByte, FALSE, NULL);
		}
		else
		{
			ErrorHandling("WSASend() error!");
		}
	}

	printf("Send data size : %d \n", sendByte);
	WSACloseEvent(evObj);
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