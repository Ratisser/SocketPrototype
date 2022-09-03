#pragma once

#include <string>
#include <vector>
#include <thread>
#include <map>
#include <mutex>
#include <WinSock2.h>
#include <WS2tcpip.h> // inet_pton �Լ��� ����ϱ� ���� ���
#include "GameEngineSocketInterface.h"
#pragma comment (lib, "ws2_32")

#include <iostream>

#include "GameEnginePacketBase.h"

class GameEnginePacketBase;
class GameEnginePacketHandler;
class GameEngineSocketClient : public GameEngineSocketInterface
{
public:
	GameEngineSocketClient();
	~GameEngineSocketClient();
	GameEngineSocketClient(const GameEngineSocketClient& _other) = delete;
	GameEngineSocketClient(GameEngineSocketClient&& _other) = delete;
	GameEngineSocketClient& operator=(const GameEngineSocketClient& _other) = delete;
	GameEngineSocketClient& operator=(const GameEngineSocketClient&& _other) = delete;

public:
	// ���� �ʱ�ȭ�� �մϴ�.
	void Initialize();
	// ������ �����մϴ�.
	void Connect(const std::string& _ip);
	// ������ ������ �����ϴ�.
	void Disconnect();
	// ������ �����͸� �����մϴ�.
	void Send(GameEnginePacketBase* _packet) override;
	// Ư�� ���Ͽ� �����͸� �����մϴ�. (��� ����...)
	void Send(SOCKET _receiver, GameEnginePacketBase* _packet) override;
	// ����� ��Ŷ�� �����մϴ�.
	void ProcessPacket();

	void AddPacketHandler(int _packetID, GameEnginePacketBase* _packetObject);

	template<typename T>
	void AddPacketHandler(T _packetID, GameEnginePacketBase* _packetObject);

public:
	bool IsConnected() { return bConneted_; }

private:
	void receiveFunction(SOCKET _serverSocket);

private:
	enum { PACKET_SIZE = 1024 };
	SOCKET socket_;

	std::thread* receiveThread_;

	bool bConneted_;

	GameEnginePacketHandler* packetHandler_;


};

template<typename T>
inline void GameEngineSocketClient::AddPacketHandler(T _packetID, GameEnginePacketBase* _packetObject)
{
	AddPacketHandler(static_cast<int>(_packetID), _packetObject);
}
