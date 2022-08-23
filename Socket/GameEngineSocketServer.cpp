#include "GameEngineSocketServer.h"

#include "GameEngineDebug.h"
#include <algorithm>
#include <functional>

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

	GameEngineDebug::OutPutDebugString("Winsock �ʱ�ȭ�� �ǽ��մϴ�.\n");
	if (SOCKET_ERROR == errorCode)
	{
		GameEngineDebug::MsgBoxError("WSAStartup falied");
		return;
	}
}

void GameEngineSocketServer::OpenServer()
{
	GameEngineDebug::OutPutDebugString("������ ���ϴ�.\n");
	if (serverSocket_ != 0)
	{
		GameEngineDebug::OutPutDebugString("������ �̹� �����ֽ��ϴ�.\n");
		return;
	}

	serverSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == serverSocket_)
	{
		GameEngineDebug::OutPutDebugString("���� ������ �����߽��ϴ�.\n");
		return;
	}

	SOCKADDR_IN address = { 0, };
	address.sin_family = AF_INET; // ipv4
	address.sin_port = htons(30001);
	address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == inet_pton(AF_INET, "0.0.0.0", &address.sin_addr)) // ���� ȣ��Ʈ
	{
		GameEngineDebug::OutPutDebugString("�ּ� �ʱ�ȭ�� �����߽��ϴ�.\n");
		return;
	}

	if (SOCKET_ERROR == bind(serverSocket_, (const sockaddr*)&address, sizeof(SOCKADDR_IN)))
	{
		GameEngineDebug::OutPutDebugString("ERROR: ���Ͽ� IP�ּҿ� ��Ʈ�� ���ε� �� �� �����ϴ�.\n");
		return;
	}

	if (SOCKET_ERROR == listen(serverSocket_, 512))
	{
		GameEngineDebug::OutPutDebugString("ERROR: ���� ���·� ��ȯ�� �� �����ϴ�.\n");
		return;
	}

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
			
			GameEngineDebug::OutPutDebugString("Ŭ���̾�Ʈ�� ������ ����Ǿ����ϴ�.\n");

			locker_.lock();
			std::vector<SOCKET>::iterator findSocketIter = std::find(clientSocketList_.begin(), clientSocketList_.end(), _clientSocket);
			SOCKET findSocket = *findSocketIter;
			clientSocketList_.erase(findSocketIter);
			
			auto findThreadIter = clientReceiveThreadList_.find(findSocket);
			findThreadIter->second.detach();

			clientReceiveThreadList_.erase(findThreadIter);
		
			locker_.unlock();
			return;
		}

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
