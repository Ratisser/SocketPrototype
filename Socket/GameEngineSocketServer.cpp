#include "GameEngineSocketServer.h"

#include <algorithm>
#include <functional>
#include <iostream>

#include "GameEngineDebug.h"
#include "GameEnginePacketHandler.h"
#include "GameEnginePacketBase.h"

GameEngineSocketServer::GameEngineSocketServer()
	: serverSocket_(0)
	, acceptThread_(nullptr)
	, bOpen_(false)
	, packetHandler_(nullptr)
{
}

GameEngineSocketServer::~GameEngineSocketServer()
{
	CloseServer();
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

	bOpen_ = true;
	packetHandler_ = new GameEnginePacketHandler(true);
	acceptThread_ = new std::thread(std::bind(&GameEngineSocketServer::acceptFunction, this));
}

void GameEngineSocketServer::CloseServer()
{
	locker_.lock();
	bOpen_ = false;

	if (nullptr != acceptThread_)
	{
		closesocket(serverSocket_);
		serverSocket_ = 0;
		acceptThread_->join();
		delete acceptThread_;
		acceptThread_ = nullptr;
	}

	for (SOCKET s : clientSocketList_)
	{
		closesocket(s);
	}

	clientSocketList_.clear();

	locker_.unlock();

	auto startIter = clientReceiveThreadList_.begin();
	auto endIter = clientReceiveThreadList_.end();

	while (startIter != endIter)
	{
		if (startIter->second.joinable())
		{
			startIter->second.join();
		}
		startIter++;
	}

	if (nullptr != packetHandler_)
	{
		delete packetHandler_;
		packetHandler_ = nullptr;
	}
}

void GameEngineSocketServer::ProcessPacket()
{
	if (nullptr != packetHandler_)
	{
		packetHandler_->ProcessPacket(this);
	}
}

void GameEngineSocketServer::Send(GameEnginePacketBase* _packet)
{
	if (_packet->GetSerializer().GetOffSet() == 0)
	{
		_packet->Serialize();
	}

	char sendData[PACKET_SIZE] = { 0, };

	memcpy(sendData, _packet->GetSerializer().GetDataPtr(), _packet->GetSerializer().GetOffSet());

	locker_.lock();
	for (auto clientSocket : clientSocketList_)
	{
		send(clientSocket, sendData, PACKET_SIZE, 0);
	}
	locker_.unlock();
}

void GameEngineSocketServer::AddPacketHandler(int _packetID, GameEnginePacketBase* _packetObject)
{
	if (packetHandler_ != nullptr)
	{
		packetHandler_->AddHandler(_packetID, _packetObject);
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

		std::thread newReceiveThread(std::bind(&GameEngineSocketServer::receiveFunction, this, socketNewUser));
		clientReceiveThreadList_.insert(std::pair<SOCKET, std::thread>(socketNewUser, std::move(newReceiveThread)));

		locker_.unlock();
	}
}

void GameEngineSocketServer::receiveFunction(SOCKET _clientSocket)
{
	char packet[PACKET_SIZE] = { 0 };

	while (true)
	{
		int result = recv(_clientSocket, packet, sizeof(packet), 0);

		if (0 < result)
		{
			packetHandler_->AnalyzePacketAndPush(packet, result);
		}
		else if (SOCKET_ERROR == result)
		{
			std::cout << "Ŭ���̾�Ʈ�� ������ ����Ǿ����ϴ�.\n";
			GameEngineDebug::OutPutDebugString("Ŭ���̾�Ʈ�� ������ ����Ǿ����ϴ�.\n");

			locker_.lock();
			if (bOpen_ == false)
			{
				locker_.unlock();
				return;
			}

			size_t clientCount = clientSocketList_.size();

			for (size_t i = 0; i < clientCount; i++)
			{
				if (_clientSocket == clientSocketList_[i])
				{
					clientSocketList_[i] = clientSocketList_.back();
					clientSocketList_.pop_back();
					break;
				}
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

		ZeroMemory(packet, PACKET_SIZE);
	}
}