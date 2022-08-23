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

	std::cout << "Winsock 초기화를 실시합니다.\n";
	GameEngineDebug::OutPutDebugString("Winsock 초기화를 실시합니다.\n");
	if (SOCKET_ERROR == errorCode)
	{
		GameEngineDebug::MsgBoxError("WSAStartup falied");
		return;
	}
}

void GameEngineSocketServer::OpenServer()
{
	std::cout << "서버를 엽니다.\n";
	GameEngineDebug::OutPutDebugString("서버를 엽니다.\n");
	if (serverSocket_ != 0)
	{
		std::cout << "서버가 이미 열려있습니다.\n";
		GameEngineDebug::OutPutDebugString("서버가 이미 열려있습니다.\n");
		return;
	}

	serverSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == serverSocket_)
	{
		std::cout << "소켓 생성에 실패했습니다.\n";
		GameEngineDebug::OutPutDebugString("소켓 생성에 실패했습니다.\n");
		return;
	}

	SOCKADDR_IN address = { 0, };
	address.sin_family = AF_INET; // ipv4
	address.sin_port = htons(30001);
	address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == inet_pton(AF_INET, "0.0.0.0", &address.sin_addr)) // 로컬 호스트
	{
		std::cout << "주소 초기화에 실패했습니다.\n";
		GameEngineDebug::OutPutDebugString("주소 초기화에 실패했습니다.\n");
		return;
	}

	if (SOCKET_ERROR == bind(serverSocket_, (const sockaddr*)&address, sizeof(SOCKADDR_IN)))
	{
		std::cout << "ERROR: 소켓에 IP주소와 포트를 바인드 할 수 없습니다.\n";
		GameEngineDebug::OutPutDebugString("ERROR: 소켓에 IP주소와 포트를 바인드 할 수 없습니다.\n");
		return;
	}

	if (SOCKET_ERROR == listen(serverSocket_, 512))
	{
		std::cout << "ERROR: 리슨 상태로 전환할 수 없습니다.\n";
		GameEngineDebug::OutPutDebugString("ERROR: 리슨 상태로 전환할 수 없습니다.\n");
		return;
	}

	std::cout << "서버를 열었습니다.\n";
	GameEngineDebug::OutPutDebugString("서버를 열었습니다.\n");

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

		std::cout << "새로운 클라이언트가 접속했습니다.\n";
		GameEngineDebug::OutPutDebugString("새로운 클라이언트가 접속했습니다.\n");

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
			std::cout << "클라이언트의 접속이 종료되었습니다.\n";
			GameEngineDebug::OutPutDebugString("클라이언트의 접속이 종료되었습니다.\n");

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
