#pragma once

#include <vector>
#include <thread>
#include <map>
#include <mutex>
#include <WinSock2.h>
#include <WS2tcpip.h> // inet_pton �Լ��� ����ϱ� ���� ���
#include "GameEngineSocketInterface.h"
#pragma comment (lib, "ws2_32")

class GameEnginePacketBase;
class GameEnginePacketHandler;
class GameEngineSocketServer : public GameEngineSocketInterface
{
public:
	GameEngineSocketServer();
	~GameEngineSocketServer();
	GameEngineSocketServer(const GameEngineSocketServer& _other) = delete;
	GameEngineSocketServer(GameEngineSocketServer&& _other) = delete;
	GameEngineSocketServer& operator=(const GameEngineSocketServer& _rhs) = delete;
	GameEngineSocketServer& operator=(GameEngineSocketServer&& _rhs) = delete;

public:
	// ���� �ʱ�ȭ�� �մϴ�.
	void Initialize();
	// ���� ������ �ʱ�ȭ�ϰ�, accept �Լ��� ������ �����带 �����մϴ�.
	void OpenServer();
	// ���� ������ �ݰ�, ��� ������ �����ϴ�.
	void CloseServer();
	// ����� ��Ŷ�� �����մϴ�.
	void ProcessPacket();
	// ����� Ŭ���̾�Ʈ�鿡�� ��Ŷ�� �����մϴ�.
	void Send(GameEnginePacketBase* _packet) override;

	void AddPacketHandler(int _packetID, GameEnginePacketBase* _packetObject);

	template<typename T>
	void AddPacketHandler(T _packetID, GameEnginePacketBase* _packetObject);

public:
	bool IsOpened() { return bOpen_; }

private:
	// Ŭ���̾�Ʈ�� ������ �ޱ� ���� �Լ�. ������� ó���˴ϴ�.
	void acceptFunction();
	// Ŭ���̾�Ʈ�� ������������ �ޱ� ���� �Լ�. n ���� �����带 �����մϴ�.(n == ����� Ŭ���̾�Ʈ ��)
	void receiveFunction(SOCKET _clientSocket);

private:
	enum { PACKET_SIZE = 1024 };
	std::vector<SOCKET> clientSocketList_;
	std::map<SOCKET, std::thread> clientReceiveThreadList_;

	bool bOpen_;
	SOCKET serverSocket_;
	std::thread* acceptThread_;

	std::mutex locker_; // ����ȭ�� ���ؽ�

	GameEnginePacketHandler* packetHandler_;
};

template<typename T>
inline void GameEngineSocketServer::AddPacketHandler(T _packetID, GameEnginePacketBase* _packetObject)
{
	AddPacketHandler(static_cast<int>(_packetID), _packetObject);
}
