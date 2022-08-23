#include "GameEnginePacketHandler.h"

#include "ChattingPacket.h"
#include <iostream>

GameEnginePacketHandler::GameEnginePacketHandler()
{

}

GameEnginePacketHandler::~GameEnginePacketHandler()
{

}

GameEnginePacketBase* GameEnginePacketHandler::AnalyzePacket(char* _data, int _size)
{
	int packetID = 0;
	int packetSize = 0;

	memcpy(&packetID, _data, sizeof(int));
	memcpy(&packetSize, _data + 4, sizeof(int));

	GameEnginePacketBase* newPacket = new GameEnginePacketBase(_data, _size);
	newPacket->SetPacketID(packetID);

	return newPacket;
}

void GameEnginePacketHandler::PushPacket(GameEnginePacketBase* packet)
{
	locker_.lock();
	packetList_.push_back(packet);
	locker_.unlock();
}

void GameEnginePacketHandler::ProcessPacket()
{
	locker_.lock();

	if (!packetList_.empty())
	{
		GameEnginePacketBase* packet = packetList_.back();
		packetList_.pop_back();

		switch (packet->GetPacketID())
		{
		case 1:
		{
			ChattingPacket* chatPacket = dynamic_cast<ChattingPacket*>(packet);
			if (nullptr != chatPacket)
			{
				chatPacket->Deserialize();
				std::cout << chatPacket->GetText() << std::endl;

				delete chatPacket;
			}
		}
		break;
		default:
			break;
		}
	}



	locker_.unlock();
}
