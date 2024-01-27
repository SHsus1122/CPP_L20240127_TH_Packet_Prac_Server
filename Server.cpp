#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include "Packet.h"

#pragma comment(lib, "ws2_32")

using namespace std;

int main()
{
	srand((u_int)(time(nullptr)));

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in ListenSockAddr;
	memset(&ListenSockAddr, 0, sizeof(ListenSockAddr));

	ListenSockAddr.sin_family = AF_INET;
	ListenSockAddr.sin_addr.s_addr = INADDR_ANY;
	ListenSockAddr.sin_port = htons(5001);

	bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));

	listen(ListenSocket, 5);

	struct sockaddr_in ClientSockAddr;
	memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
	int ClientSockAddrLength = sizeof(ClientSockAddr);

	SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrLength);

	while (true)
	{
		Header DataHeader;

		DataHeader.Length = htons(sizeof(Data));
		DataHeader.PacketType = htons((unsigned short)(EPacketType::Caculate));

		send(ClientSocket, (char*)&DataHeader, (u_int)sizeof(DataHeader), 0);

		Data Packet;
		Packet.FirstNumber = htonl((rand() % 20000) - 10000);
		Packet.SecondNumber = htonl((rand() % 20000) - 10000);
		Packet.SecondNumber == (Packet.SecondNumber == 0) ? 1 : Packet.SecondNumber;
		Packet.Operator = rand() % 5;

		send(ClientSocket, (char*)&Packet, (u_int)sizeof(Packet), 0);

		char Buffer[1024] = { 0, };
		int RecvByte = recv(ClientSocket, Buffer, 1024, 0);
		if (RecvByte <= 0)
			break;

		long long Result = 0;
		memcpy(&Result, Buffer, sizeof(Result));
		cout << (long long)ntohll(Result) << endl;
	}

	closesocket(ClientSocket);
	closesocket(ListenSocket);

	WSACleanup();


	return 0;
}