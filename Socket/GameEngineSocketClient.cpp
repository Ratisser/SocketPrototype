#include "GameEngineSocketClient.h"
#include "GameEngineDebug.h"

#include <functional>

GameEngineSocketClient::GameEngineSocketClient()
	: socket_(0)
	, bConneted_(false)
	, receiveThread_(nullptr)
{

}

GameEngineSocketClient::~GameEngineSocketClient()
{
	if (socket_ == 0)
	{
		std::cout << "연결된 서버가 없습니다.\n";
		GameEngineDebug::OutPutDebugString("연결된 서버가 없습니다.\n");
		return;
	}

	shutdown(socket_, SD_BOTH);
	closesocket(socket_);

	if (nullptr != receiveThread_)
	{
		receiveThread_->join();
		delete receiveThread_;
		receiveThread_ = nullptr;
	}
	socket_ = 0;
}

void GameEngineSocketClient::Initialize()
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

void GameEngineSocketClient::Connect(const std::string& _ip)
{
	if (socket_ != 0)
	{
		std::cout << "서버에 이미 연결되어 있습니다.\n";
		GameEngineDebug::OutPutDebugString("서버에 이미 연결되어 있습니다.\n");
		return;
	}

	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socket_)
	{
		std::cout << "소켓 생성에 실패했습니다.\n";
		GameEngineDebug::OutPutDebugString("소켓 생성에 실패했습니다.\n");
		return;
	}

	SOCKADDR_IN address = { 0, };
	address.sin_family = AF_INET; // ipv4
	address.sin_port = htons(30001);
	address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == inet_pton(AF_INET, _ip.c_str(), &address.sin_addr)) // 로컬 호스트
	{
		std::cout << "주소 초기화에 실패했습니다.\n";
		GameEngineDebug::OutPutDebugString("주소 초기화에 실패했습니다.\n");
		return;
	}

	int connetResult = connect(socket_, (sockaddr*)&address, sizeof(address));
	if (SOCKET_ERROR == connetResult)
	{
		std::cout << "서버에 연결할 수 없습니다.\n";
		GameEngineDebug::OutPutDebugString("서버에 연결할 수 없습니다.\n");
		return;
	}

	// 연결이 됐다면? 전송 스레드, 대기 스레드
	std::cout << "서버에 연결되었습니다.\n";
	GameEngineDebug::OutPutDebugString("서버에 연결되었습니다.\n");
	bConneted_ = true;
	if (receiveThread_ == nullptr)
	{
		receiveThread_ = new std::thread(std::bind(&GameEngineSocketClient::receiveFunction, this, socket_));
	}
}

void GameEngineSocketClient::Disconnect()
{
	if (socket_ == 0)
	{
		std::cout << "연결된 서버가 없습니다.\n";
		GameEngineDebug::OutPutDebugString("연결된 서버가 없습니다.\n");
		return;
	}

	closesocket(socket_);

	if (nullptr != receiveThread_)
	{
		receiveThread_->join();
		delete receiveThread_;
		receiveThread_ = nullptr;
	}
	socket_ = 0;
}

void GameEngineSocketClient::receiveFunction(SOCKET& _clientSocket)
{
	char packet[PACKET_SIZE] = { 0 };

	while (true)
	{

		int Result = recv(_clientSocket, packet, sizeof(packet), 0);

		if (SOCKET_ERROR == Result)
		{
			std::cout << "연결이 종료되었습니다.\n";
			GameEngineDebug::OutPutDebugString("연결이 종료되었습니다.\n");
			return;
		}

		std::cout << packet << std::endl;
		ZeroMemory(packet, PACKET_SIZE);
	}
}