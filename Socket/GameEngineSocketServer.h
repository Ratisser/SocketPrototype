#pragma once

#include <vector>
#include <thread>
#include <map>
#include <mutex>
#include <WinSock2.h>
#include <WS2tcpip.h> // inet_pton 함수를 사용하기 위한 헤더
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
	// 소켓 초기화를 합니다.
	void Initialize();
	// 서버 소켓을 초기화하고, accept 함수를 실행할 스레드를 생성합니다.
	void OpenServer();
	// 서버 소켓을 닫고, 모든 연결을 끊습니다.
	void CloseServer();
	// 저장된 패킷을 실행합니다.
	void ProcessPacket();
	// 연결된 클라이언트들에게 패킷을 전송합니다.
	void Send(GameEnginePacketBase* _packet) override;

	void AddPacketHandler(int _packetID, GameEnginePacketBase* _packetObject);

	template<typename T>
	void AddPacketHandler(T _packetID, GameEnginePacketBase* _packetObject);

public:
	bool IsOpened() { return bOpen_; }

private:
	// 클라이언트의 연결을 받기 위한 함수. 스레드로 처리됩니다.
	void acceptFunction();
	// 클라이언트의 데이터전송을 받기 위한 함수. n 개의 스레드를 생성합니다.(n == 연결된 클라이언트 수)
	void receiveFunction(SOCKET _clientSocket);

private:
	enum { PACKET_SIZE = 1024 };
	std::vector<SOCKET> clientSocketList_;
	std::map<SOCKET, std::thread> clientReceiveThreadList_;

	bool bOpen_;
	SOCKET serverSocket_;
	std::thread* acceptThread_;

	std::mutex locker_; // 동기화용 뮤텍스

	GameEnginePacketHandler* packetHandler_;
};

template<typename T>
inline void GameEngineSocketServer::AddPacketHandler(T _packetID, GameEnginePacketBase* _packetObject)
{
	AddPacketHandler(static_cast<int>(_packetID), _packetObject);
}
