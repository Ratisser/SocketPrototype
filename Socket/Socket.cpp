// Socket.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include "GameEngineSocketServer.h"
#include "GameEngineSocketClient.h"
#include "GameEngineSerializer.h"

#include <Windows.h>
#include "GameEngineDebug.h"
#include "ChattingPacket.h"
#include "GameEnginePacketHandler.h"

#include <conio.h>

#define SERVER

int main()
{
	GameEngineDebug::LeakCheckOn();
#ifdef SERVER
	GameEngineSocketServer server;
	server.Initialize();
	server.OpenServer();


	while (true)
	{
		Sleep(1);
		int input = 0;
		input = GetAsyncKeyState('Q');
		server.ProcessPacket();
		if (input)
		{
			break;
		}
	}

	server.CloseServer();

	//system("pause");
#else
	GameEngineSocketClient c;
	c.Initialize();
	c.Connect("121.129.74.58");

	auto lambda = [&]()
	{
		while (c.IsConnected())
		{
			c.ProcessPacket();
		}
	};

	std::thread t(lambda);

	while (true)
	{
		Sleep(50);

		std::string text;
		std::cin >> text;
		if (text == "q" || !c.IsConnected())
		{
			break;
		}
		else if (text.size() > 0)
		{
			ChattingPacket* newPacket = new ChattingPacket();
			newPacket->SetText(text);
			c.Send(newPacket);
			delete newPacket;
}
	}

	c.Disconnect();
	t.join();
	system("pause");
#endif // DEBUG
}



