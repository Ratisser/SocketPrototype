#pragma once

#include <vector>
#include <thread>
#include <map>
#include <mutex>
#include <WinSock2.h>
#include <WS2tcpip.h> // inet_pton 함수를 사용하기 위한 헤더
#pragma comment (lib, "ws2_32")

class GameEngineSocketServer
{
public:
	GameEngineSocketServer();
	~GameEngineSocketServer();
	GameEngineSocketServer(const GameEngineSocketServer& _other) = delete;
	GameEngineSocketServer(GameEngineSocketServer&& _other) = delete;
	GameEngineSocketServer& operator=(const GameEngineSocketServer& _rhs) = delete;
	GameEngineSocketServer& operator=(GameEngineSocketServer&& _rhs) = delete;

public:
	void Initialize(); // 소켓 초기화를 합니다.
	void OpenServer(); // 서버 소켓을 초기화하고, accept 함수를 실행할 스레드를 생성합니다.
	void CloseServer(); // 서버 소켓을 닫고, 모든 연결을 끊습니다.

private:
	// 클라이언트의 연결을 받기 위한 함수. 스레드로 처리됩니다.
	void acceptFunction();
	// 클라이언트의 데이터전송을 받기 위한 함수. n 개의 스레드를 생성합니다.(n == 연결된 클라이언트 수)
	void receiveFunction(SOCKET& _clientSocket);

	void broadcast(char _packet[]); // 연결된 클라이언트들에게 패킷을 전송한다.

private:
	enum { PACKET_SIZE = 1024 };
	SOCKET serverSocket_;
	std::thread* acceptThread_;
	
	std::mutex locker_; // 동기화용 뮤텍스

	std::vector<SOCKET> clientSocketList_;
	std::map<SOCKET, std::thread> clientReceiveThreadList_;
};

