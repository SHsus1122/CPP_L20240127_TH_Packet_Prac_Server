#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

using namespace std;

int main()
{
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

	fd_set ReadSocketList;
	FD_ZERO(&ReadSocketList);

	FD_SET(ListenSocket, &ReadSocketList);

	struct timeval TimeOut;
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 100;

	fd_set CopyReadSocketList;
	FD_ZERO(&CopyReadSocketList);

	while (true)
	{
		CopyReadSocketList = ReadSocketList;

		int EventSocketCount = select(0, &CopyReadSocketList, nullptr, nullptr, &TimeOut);
		if (EventSocketCount == 0)
		{
			// no event �̺�Ʈ �̹߻�
		}
		else if (EventSocketCount < 0)
		{
			// error �Լ� ȣ�� ��� ���� ��ȯ�� ���
		}
		else
		{
			// process ���� ����(�̺�Ʈ �߻�)
			// ���� ���� ��ŭ..
			for (int i = 0; i < (int)ReadSocketList.fd_count; i++)
			{
				// i ��° ������ ������ ���� ������ �̺�Ʈ ����..
				if (FD_ISSET(ReadSocketList.fd_array[i], &CopyReadSocketList))
				{
					// ���� ó�� ����, �̺�Ʈ�� �߻��� ������ Ȯ�� �ؾ� �ϴ� ������ ListenSocket �� ���ϰ� ��ġ����...
					if (ReadSocketList.fd_array[i] == ListenSocket)
					{
						// ��� ������ ��� ������ ���� �۾�
						struct sockaddr_in ClientSockAddr;
						memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
						int ClientSockAddrLength = sizeof(ListenSockAddr);

						SOCKET NewClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrLength);

						FD_SET(NewClientSocket, &ReadSocketList);
						cout << "connect socket : " << inet_ntoa(ClientSockAddr.sin_addr) << endl;
					}
					else
					{
						char Buffer[1024] = { 0, };
						int RecvByte = recv(ReadSocketList.fd_array[i], Buffer, 1024, 0);

						if (RecvByte == 0)
						{
							// Disconnected
							cout << "Disconnected Client : " << ReadSocketList.fd_array[i] << endl;

							FD_CLR(ReadSocketList.fd_array[i], &ReadSocketList);
						}
						else if (RecvByte < 0)
						{
							// Error Disconnected
							cout << "Error Disconnected Client : " << ReadSocketList.fd_array[i] << endl;

							FD_CLR(ReadSocketList.fd_array[i], &ReadSocketList);
						}
						else
						{
							cout << "Recv Client : " << ReadSocketList.fd_array[i] << endl;
							
							send(ReadSocketList.fd_array[i], Buffer, RecvByte, 0);
						}
					}
				}
			}
		}
	}

	WSACleanup();

	return 0;
}