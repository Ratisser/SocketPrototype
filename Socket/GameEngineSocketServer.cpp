#include "GameEngineSocketServer.h"

#include "GameEngineDebug.h"
#include <algorithm>
#include <functional>
#include <iostream>

GameEngineSocketServer::GameEngineSocketServer()
	: serverSocket_(0)
	, acceptThread_(nullptr)
{
}

GameEngineSocketServer::~GameEngineSocketServer()
{
	if (nullptr != acceptThread_)
	{
		closesocket(serverSocket_);
		serverSocket_ = 0;
		acceptThread_->join();
		delete acceptThread_;
		acceptThread_ = nullptr;
	}
}

void GameEngineSocketServer::Initialize()
{
	WSAData wsaData;
	int errorCode = WSAStartup(MAKEWORD(2, 2), &wsaData);

	std::cout << "Winsock �ʱ�ȭ�� �ǽ��մϴ�.\n";
	GameEngineDebug::OutPutDebugString("Winsock �ʱ�ȭ�� �ǽ��մϴ�.\n");
	if (SOCKET_ERROR == errorCode)
	{
		GameEngineDebug::MsgBoxError("WSAStartup falied");
		return;
	}
}

void GameEngineSocketServer::OpenServer()
{
	std::cout << "������ ���ϴ�.\n";
	GameEngineDebug::OutPutDebugString("������ ���ϴ�.\n");
	if (serverSocket_ != 0)
	{
		std::cout << "������ �̹� �����ֽ��ϴ�.\n";
		GameEngineDebug::OutPutDebugString("������ �̹� �����ֽ��ϴ�.\n");
		return;
	}

	serverSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == serverSocket_)
	{
		std::cout << "���� ������ �����߽��ϴ�.\n";
		GameEngineDebug::OutPutDebugString("���� ������ �����߽��ϴ�.\n");
		return;
	}

	SOCKADDR_IN address = { 0, };
	address.sin_family = AF_INET; // ipv4
	address.sin_port = htons(30001);
	address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == inet_pton(AF_INET, "0.0.0.0", &address.sin_addr)) // ���� ȣ��Ʈ
	{
		std::cout << "�ּ� �ʱ�ȭ�� �����߽��ϴ�.\n";
		GameEngineDebug::OutPutDebugString("�ּ� �ʱ�ȭ�� �����߽��ϴ�.\n");
		return;
	}

	if (SOCKET_ERROR == bind(serverSocket_, (const sockaddr*)&address, sizeof(SOCKADDR_IN)))
	{
		std::cout << "ERROR: ���Ͽ� IP�ּҿ� ��Ʈ�� ���ε� �� �� �����ϴ�.\n";
		GameEngineDebug::OutPutDebugString("ERROR: ���Ͽ� IP�ּҿ� ��Ʈ�� ���ε� �� �� �����ϴ�.\n");
		return;
	}

	if (SOCKET_ERROR == listen(serverSocket_, 512))
	{
		std::cout << "ERROR: ���� ���·� ��ȯ�� �� �����ϴ�.\n";
		GameEngineDebug::OutPutDebugString("ERROR: ���� ���·� ��ȯ�� �� �����ϴ�.\n");
		return;
	}

	std::cout << "������ �������ϴ�.\n";
	GameEngineDebug::OutPutDebugString("������ �������ϴ�.\n");

	acceptThread_ = new std::thread(std::bind(&GameEngineSocketServer::acceptFunction, this));
}

void GameEngineSocketServer::CloseServer()
{
	if (nullptr != acceptThread_)
	{
		closesocket(serverSocket_);
		serverSocket_ = 0;
		acceptThread_->join();
		delete acceptThread_;
		acceptThread_ = nullptr;
	}
}

void GameEngineSocketServer::acceptFunction()
{
	SOCKADDR_IN userAddress = { 0, };
	int size = sizeof(SOCKADDR_IN);

	while (true)
	{
		SOCKET socketNewUser = accept(serverSocket_, (sockaddr*)&userAddress, &size);
		if (INVALID_SOCKET == socketNewUser)
		{
			return;
		}

		std::cout << "���ο� Ŭ���̾�Ʈ�� �����߽��ϴ�.\n";
		GameEngineDebug::OutPutDebugString("���ο� Ŭ���̾�Ʈ�� �����߽��ϴ�.\n");

		locker_.lock();

		clientSocketList_.push_back(socketNewUser);

		std::thread newReceiveThread(std::bind(&GameEngineSocketServer::receiveFunction, this, std::ref(socketNewUser)));
		clientReceiveThreadList_.insert(std::pair<SOCKET, std::thread>(socketNewUser, std::move(newReceiveThread)));

		locker_.unlock();
	}
}

void GameEngineSocketServer::receiveFunction(SOCKET& _clientSocket)
{
	char packet[PACKET_SIZE] = { 0 };

	while (true)
	{

		int Result = recv(_clientSocket, packet, sizeof(packet), 0);

		if (SOCKET_ERROR == Result)
		{
			std::cout << "Ŭ���̾�Ʈ�� ������ ����Ǿ����ϴ�.\n";
			GameEngineDebug::OutPutDebugString("Ŭ���̾�Ʈ�� ������ ����Ǿ����ϴ�.\n");

			locker_.lock();

			std::vector<SOCKET>::iterator findSocketIter = std::find(clientSocketList_.begin(), clientSocketList_.end(), _clientSocket);
			if (findSocketIter != clientSocketList_.end())
			{
				SOCKET findSocket = *findSocketIter;
				clientSocketList_.erase(findSocketIter);
			}



			auto findThreadIter = clientReceiveThreadList_.find(_clientSocket);
			if (findThreadIter != clientReceiveThreadList_.end())
			{
				findThreadIter->second.detach();

				clientReceiveThreadList_.erase(_clientSocket);

			}

			locker_.unlock();
			return;
		}

		std::cout << packet << std::endl;
		broadcast(packet);

		ZeroMemory(packet, PACKET_SIZE);
	}
}

void GameEngineSocketServer::broadcast(char _packet[])
{
	locker_.lock();
	for (auto clientSocket : clientSocketList_)
	{
		send(clientSocket, _packet, PACKET_SIZE, 0);
	}
	locker_.unlock();
}
