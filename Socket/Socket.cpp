// Socket.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <conio.h>
#include "GameEngineSocketServer.h"
#include "GameEngineSocketClient.h"

#include <Windows.h>

#define SERVER

#ifdef SERVER
int main()
{
	GameEngineSocketServer server;
	server.Initialize();
	server.OpenServer();


	while (true)
	{
		Sleep(1);
		int input = _getch();
		if (input == 'q')
		{
			break;
		}
	}

	server.CloseServer();
}
#else
int main()
{
	GameEngineSocketClient c;
	c.Initialize();
	c.Connect("121.129.74.58");


	while (true)
	{
		Sleep(1);
		int input = _getch();
		if (input == 'q')
		{
			break;
		}
	}

	c.Disconnect();
	system("pause");
}

#endif // DEBUG




