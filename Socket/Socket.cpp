// Socket.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <conio.h>
#include "GameEngineSocketServer.h"


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