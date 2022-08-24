#pragma once

#include <string>
#include <vector>
#include <thread>
#include <map>
#include <mutex>
#include <WinSock2.h>
#include <WS2tcpip.h> // inet_pton 함수를 사용하기 위한 헤더
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
	// 소켓 초기화를 합니다.
	void Initialize();
	// 서버에 연결합니다.
	void Connect(const std::string& _ip);
	// 서버와 연결을 끊습니다.
	void Disconnect();
	// 서버에 데이터를 전송합니다.
	void Send(GameEnginePacketBase* _packet) override;
	// 저장된 패킷을 실행합니다.
	void ProcessPacket();

public:
	bool IsConnected() { return bConneted_; }

private:
	void receiveFunction(SOCKET _clientSocket);

private:
	enum { PACKET_SIZE = 1024 };
	SOCKET socket_;

	std::thread* receiveThread_;

	bool bConneted_;

	GameEnginePacketHandler* packetHandler_;
};

