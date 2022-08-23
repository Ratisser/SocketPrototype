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

}

void GameEngineSocketClient::Initialize()
{
	WSAData wsaData;
	int errorCode = WSAStartup(MAKEWORD(2, 2), &wsaData);

	GameEngineDebug::OutPutDebugString("Winsock �ʱ�ȭ�� �ǽ��մϴ�.");
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
		GameEngineDebug::OutPutDebugString("������ �̹� �����ֽ��ϴ�.");
		return;
	}

	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socket_)
	{
		GameEngineDebug::OutPutDebugString("���� ������ �����߽��ϴ�.");
		return;
	}

	SOCKADDR_IN address = { 0, };
	address.sin_family = AF_INET; // ipv4
	address.sin_port = htons(30001);
	address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == inet_pton(AF_INET, _ip.c_str(), &address.sin_addr)) // ���� ȣ��Ʈ
	{
		GameEngineDebug::OutPutDebugString("�ּ� �ʱ�ȭ�� �����߽��ϴ�.");
		return;
	}

	int connetResult = connect(socket_, (sockaddr*)&address, sizeof(address));
	if (SOCKET_ERROR == connetResult)
	{
		GameEngineDebug::OutPutDebugString("������ ������ �� �����ϴ�.");
		return;
	}

	// ������ �ƴٸ�? ���� ������, ��� ������
	bConneted_ = true;
	receiveThread_ = new std::thread(std::bind(&GameEngineSocketClient::receiveFunction, this, socket_));
}

void GameEngineSocketClient::receiveFunction(SOCKET& _clientSocket)
{
	char packet[PACKET_SIZE] = { 0 };

	while (true)
	{

		int Result = recv(_clientSocket, packet, sizeof(packet), 0);

		if (SOCKET_ERROR == Result)
		{

			GameEngineDebug::OutPutDebugString("�����κ��� ������ ����Ǿ����ϴ�.");
			return;
		}

		
		ZeroMemory(packet, PACKET_SIZE);
	}
}