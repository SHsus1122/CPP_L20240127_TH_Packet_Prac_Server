#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <process.h>
#include <vector>
#include <set>
#include <map>

#pragma comment(lib, "ws2_32")

std::set<SOCKET> SessionList;

CRITICAL_SECTION SessionCS;

unsigned WINAPI WorkerThread(void* Arg)
{
	SOCKET ClientSocket = *((SOCKET*)Arg);
	while (true)
	{
		char Buffer[1024] = { 0, };
		int RecvByte = recv(ClientSocket, Buffer, sizeof(Buffer), 0);
		if (RecvByte <= 0)
		{
			EnterCriticalSection(&SessionCS);
			SessionList.erase(ClientSocket);
			LeaveCriticalSection(&SessionCS);
			closesocket(ClientSocket);
			break;
		}
		else
		{
			EnterCriticalSection(&SessionCS);
			for (auto ConnectSocket : SessionList)
			{
				int SendLength = send(ConnectSocket, Buffer, RecvByte, 0);
			}
			LeaveCriticalSection(&SessionCS);
		}
	}
	return 0;
}

int main()
{
	InitializeCriticalSection(&SessionCS);

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ListenSockAddr;
	memset(&ListenSockAddr, 0, sizeof(ListenSockAddr));

	ListenSockAddr.sin_family = AF_INET;
	ListenSockAddr.sin_addr.s_addr = INADDR_ANY;
	ListenSockAddr.sin_port = htons(5001);

	bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));

	listen(ListenSocket, 5);

	while (true)
	{
		SOCKADDR_IN ClientSockAddr;
		memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
		int ClientSockAddrLength = sizeof(ClientSockAddr);
		SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrLength);

		HANDLE ThreadHandle = (HANDLE)_beginthreadex(0, 0, WorkerThread, (void*)&ClientSocket, 0, 0);

		EnterCriticalSection(&SessionCS);
		SessionList.insert(ClientSocket);
		LeaveCriticalSection(&SessionCS);
	}

	closesocket(ListenSocket);

	WSACleanup();

	DeleteCriticalSection(&SessionCS);

	return 0;
}