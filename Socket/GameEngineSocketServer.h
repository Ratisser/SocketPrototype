#pragma once

#include <vector>
#include <thread>
#include <map>
#include <mutex>
#include <WinSock2.h>
#include <WS2tcpip.h> // inet_pton �Լ��� ����ϱ� ���� ���
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
	void Initialize(); // ���� �ʱ�ȭ�� �մϴ�.
	void OpenServer(); // ���� ������ �ʱ�ȭ�ϰ�, accept �Լ��� ������ �����带 �����մϴ�.
	void CloseServer(); // ���� ������ �ݰ�, ��� ������ �����ϴ�.

private:
	// Ŭ���̾�Ʈ�� ������ �ޱ� ���� �Լ�. ������� ó���˴ϴ�.
	void acceptFunction();
	// Ŭ���̾�Ʈ�� ������������ �ޱ� ���� �Լ�. n ���� �����带 �����մϴ�.(n == ����� Ŭ���̾�Ʈ ��)
	void receiveFunction(SOCKET& _clientSocket);

	void broadcast(char _packet[]); // ����� Ŭ���̾�Ʈ�鿡�� ��Ŷ�� �����Ѵ�.

private:
	enum { PACKET_SIZE = 1024 };
	SOCKET serverSocket_;
	std::thread* acceptThread_;
	
	std::mutex locker_; // ����ȭ�� ���ؽ�

	std::vector<SOCKET> clientSocketList_;
	std::map<SOCKET, std::thread> clientReceiveThreadList_;
};

