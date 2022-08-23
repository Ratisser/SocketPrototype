#pragma once

#include <string>
#include <vector>
#include <thread>
#include <map>
#include <mutex>
#include <WinSock2.h>
#include <WS2tcpip.h> // inet_pton �Լ��� ����ϱ� ���� ���
#pragma comment (lib, "ws2_32")

class GameEnginePacket;
class GameEngineSocketClient
{
public:
	GameEngineSocketClient();
	~GameEngineSocketClient();
	GameEngineSocketClient(const GameEngineSocketClient& _other) = delete;
	GameEngineSocketClient(GameEngineSocketClient&& _other) = delete;
	GameEngineSocketClient& operator=(const GameEngineSocketClient& _other) = delete;
	GameEngineSocketClient& operator=(const GameEngineSocketClient&& _other) = delete;

public:
	void Initialize(); // ���� �ʱ�ȭ�� �մϴ�.
	void Connect(const std::string& _ip);

	void Send(const GameEnginePacket* _packet);

private:
	void receiveFunction(SOCKET& _clientSocket);

private:
	enum { PACKET_SIZE = 1024 };
	SOCKET socket_;

	std::thread* receiveThread_;

	bool bConneted_;

};

