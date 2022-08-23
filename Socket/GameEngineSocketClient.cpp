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
		std::cout << "����� ������ �����ϴ�.\n";
		GameEngineDebug::OutPutDebugString("����� ������ �����ϴ�.\n");
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

	std::cout << "Winsock �ʱ�ȭ�� �ǽ��մϴ�.\n";
	GameEngineDebug::OutPutDebugString("Winsock �ʱ�ȭ�� �ǽ��մϴ�.\n");
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
		std::cout << "������ �̹� ����Ǿ� �ֽ��ϴ�.\n";
		GameEngineDebug::OutPutDebugString("������ �̹� ����Ǿ� �ֽ��ϴ�.\n");
		return;
	}

	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socket_)
	{
		std::cout << "���� ������ �����߽��ϴ�.\n";
		GameEngineDebug::OutPutDebugString("���� ������ �����߽��ϴ�.\n");
		return;
	}

	SOCKADDR_IN address = { 0, };
	address.sin_family = AF_INET; // ipv4
	address.sin_port = htons(30001);
	address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == inet_pton(AF_INET, _ip.c_str(), &address.sin_addr)) // ���� ȣ��Ʈ
	{
		std::cout << "�ּ� �ʱ�ȭ�� �����߽��ϴ�.\n";
		GameEngineDebug::OutPutDebugString("�ּ� �ʱ�ȭ�� �����߽��ϴ�.\n");
		return;
	}

	int connetResult = connect(socket_, (sockaddr*)&address, sizeof(address));
	if (SOCKET_ERROR == connetResult)
	{
		std::cout << "������ ������ �� �����ϴ�.\n";
		GameEngineDebug::OutPutDebugString("������ ������ �� �����ϴ�.\n");
		return;
	}

	// ������ �ƴٸ�? ���� ������, ��� ������
	std::cout << "������ ����Ǿ����ϴ�.\n";
	GameEngineDebug::OutPutDebugString("������ ����Ǿ����ϴ�.\n");
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
		std::cout << "����� ������ �����ϴ�.\n";
		GameEngineDebug::OutPutDebugString("����� ������ �����ϴ�.\n");
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
			std::cout << "������ ����Ǿ����ϴ�.\n";
			GameEngineDebug::OutPutDebugString("������ ����Ǿ����ϴ�.\n");
			return;
		}

		std::cout << packet << std::endl;
		ZeroMemory(packet, PACKET_SIZE);
	}
}